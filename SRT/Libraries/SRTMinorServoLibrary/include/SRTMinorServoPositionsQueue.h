#ifndef __SRTMINORSERVOPOSITIONSQUEUE_H__
#define __SRTMINORSERVOPOSITIONSQUEUE_H__

/**
 * SRTMinorServoPositionsQueue.h
 * Giuseppe Carboni (giuseppe.carboni@inaf.it)
 */

#include <map>
#include <vector>
#include <shared_mutex>


namespace MinorServo
{
    /**
     * This class implements a queue of time tagged positions. it extends a simple std::map with some specific methods.
     */
    class SRTMinorServoPositionsQueue : private std::map<ACS::Time, const std::vector<double>>
    {
    public:
        /**
         * Default constructor. Used only for lazy initialization.
         */
        SRTMinorServoPositionsQueue() : std::map<ACS::Time, const std::vector<double>>(), m_queue_size(0), m_vector_size(0), m_mutex() {}

        /**
         * Constructor with queue size as parameter.
         * @param queue_size the maximum size of the queue. Once this value is reached the oldest entry gets discarded.
         */
        SRTMinorServoPositionsQueue(size_t queue_size) : std::map<ACS::Time, const std::vector<double>>(), m_queue_size(queue_size), m_vector_size(0), m_mutex()
        {
            std::unique_lock<std::shared_mutex> lock(m_mutex);
            queueLazyInit(queue_size);
        }

        /**
         * Constructor with queue size and vector size as parameters.
         * @param queue_size the maximum size of the queue. Once this value is reached the oldest entry gets discarded.
         * @param vector_size the length of the vectors that this object will store. Once set it cannot be changed. This assures we are always dealing with the same number of virtual axes.
         */
        SRTMinorServoPositionsQueue(size_t queue_size, size_t vector_size) : std::map<ACS::Time, const std::vector<double>>(), m_queue_size(queue_size), m_vector_size(vector_size), m_mutex()
        {
            std::unique_lock<std::shared_mutex> lock(m_mutex);
            queueLazyInit(queue_size);
            vectorLazyInit(vector_size);
        }

        /**
         * Custom assignment operator. It locks both the current object and the passed one with a mutex in order for the assignment operation to be thread safe.
         * @param other the other SRTMinorServoPositionsQueue we are assigning its value to the current object.
         * @return the newly populated SRTMinorServoPositionsQueue object.
         */
        SRTMinorServoPositionsQueue& operator=(const SRTMinorServoPositionsQueue& other)
        {
            if(this != &other)
            {
                std::unique_lock<std::shared_mutex> lockThis(m_mutex, std::defer_lock);
                std::unique_lock<std::shared_mutex> lockOther(other.m_mutex, std::defer_lock);
                std::lock(lockThis, lockOther);
                m_queue_size = other.m_queue_size;
                m_vector_size = other.m_vector_size;
                std::map<ACS::Time, const std::vector<double>>::operator=(other);
            }

            return *this;
        }

        /**
         * Put method, with initializer list argument.
         * @param key the time the given coordinates are related to.
         * @param values the given set of coordinates.
         */
        void put(ACS::Time key, const std::initializer_list<double>& values)
        {
            put(key, std::vector<double>(values));
        }

        /**
         * Put method, with ACS::doubleSeq argument.
         * @param key the time the given coordinates are related to.
         * @param values the given set of coordinates.
         */
        void put(ACS::Time key, const ACS::doubleSeq& values)
        {
            put(key, std::vector<double>(values.get_buffer(), values.get_buffer() + values.length()));
        }

        /**
         * Put method, with std::vector<double> argument.
         * @param key the time the given coordinates are related to.
         * @param values the given set of coordinates.
         */
        void put(ACS::Time key, const std::vector<double>& values)
        {
            std::unique_lock<std::shared_mutex> lock(m_mutex);

            vectorLazyInit(values.size());
            queueLazyInit(m_queue_size);
            if(std::map<ACS::Time, const std::vector<double>>::size() == m_queue_size)
            {
                // Remove the oldest one
                this->erase(this->begin());
            }
            this->emplace(std::make_pair(key, values));
        }

        /**
         * Get method. It retrieves a set of coordinates from a given ACS::Time, giving back the time as well.
         * @param key the time the user wants to retrieve the related coordinates.
         * @param exact a boolean indicating whether the user wants to interpolate (false) or not (true).
         * @throw std::logic_error when the queue is empty.
         * @throw std::out_of_range when the exact point was not found in the queue.
         * @return a std::pair containing the ACS::Time as first element and the set of coordinates as second element.
         */
        std::pair<ACS::Time, const std::vector<double>> get(ACS::Time key, bool exact = false)
        {
            std::shared_lock<std::shared_mutex> lock(m_mutex);

            if(this->empty())
            {
                throw std::logic_error("The queue is empty!");
            }

            if(const SRTMinorServoPositionsQueue::iterator point = this->find(key); point != this->end())
            {
                return *point;
            }
            else if(exact)
            {
                // Exact point not found, we throw an exception
                throw std::out_of_range("Exact point not found!");
            }
            else
            {
                // Key not found, should check outside the boundaries or interpolate
                if(key <= this->begin()->first)
                {
                    // Aksed for a timestamp older than the earliest point in the queue
                    return *this->cbegin();
                }
                else if(key >= this->rbegin()->first)
                {
                    // Asked for a timestamp newer than the latest point in the queue
                    return *this->crbegin();
                }
                else
                {
                    std::vector<double> positions(m_vector_size, 0.0);
                    SRTMinorServoPositionsQueue::iterator p0, p1;
                    p1 = this->lower_bound(key);
                    p0 = p1;
                    p0--;

                    // Calculate the linear fit for each position
                    double fraction = (key - p0->first) / (p1->first - p0->first);

                    for(size_t i = 0; i < m_vector_size; i++)
                    {
                        positions[i] = p0->second[i] + fraction * (p1->second[i] - p0->second[i]);
                    }

                    return std::make_pair(key, (const std::vector<double>)positions);
                }
            }
        }

        /**
         * Size method thread safe override.
         * @return the number of elements in the queue.
         */
        size_t size() const
        {
            std::shared_lock<std::shared_mutex> lock(m_mutex);
            return std::map<ACS::Time, const std::vector<double>>::size();
        }

        /**
         * Clear method, thread safe. It empties the queue.
         */
        void clear()
        {
            std::unique_lock<std::shared_mutex> lock(m_mutex);
            std::map<ACS::Time, const std::vector<double>>::clear();
        }

        /**
         * This method returns the number of points having a higher tag time than the one passed as argument.
         * @param t the time threshold. This method counts and returns the number of points having a higher time than this value.
         * @throw std::logic_error when the queue is empty.
         * @return the number of points having a higher time than the given one.
         */
        size_t getRemainingPoints(ACS::Time t)
        {
            std::shared_lock<std::shared_mutex> lock(m_mutex);
            if(this->empty())
            {
                throw std::logic_error("The queue is empty!");
            }
            return std::distance(this->lower_bound(t), this->end());
        }

    private:
        /**
         * This method gets called by the constructors for the lazy intialization of the queue size value.
         * @param queue_size the desired maximum queue size.
         * @throw std::length_error when the desired queue size is equal to 0 or when it is greater than the maximum size that can be currently allocated.
         */
        void queueLazyInit(size_t queue_size)
        {
            if(m_queue_size == 0)
            {
                if(queue_size == 0)
                {
                    // The maximum queue size was not set yet
                    throw std::length_error("Queue length cannot be 0.");
                }
                else if(queue_size > this->max_size())
                {
                    // The requested size is greater than the maximum possible queue size
                    throw std::length_error("Queue length cannot exceed " + std::to_string(this->max_size()) + ".");
                }
                else
                {
                    m_queue_size = queue_size;
                }
            }
        }

        /**
         * This method gets called by the constructors for the lazy initialization of the vector size value.
         * @param vector_size the desired length of the vector containing the set of points.
         * @throw std::length_error when the desired vector size is equal to 0 or when it is longer than 6. We don't have more than 6 axes, so we hard cap this value to 6.
         *                          It also throws this when the user tries to insert a vector with different lenght then the already defined one. This ensures all stored vecors are of equal length.
         */
        void vectorLazyInit(size_t vector_size)
        {
            if(vector_size == 0)
            {
                throw std::length_error("Vector length cannot be 0.");
            }
            else if(vector_size > 6)
            {
                // Hard cap to 6, we don't need more
                throw std::length_error("Vector length cannot be longer than 6.");
            }
            else if(m_vector_size == 0)
            {
                m_vector_size = vector_size;
            }
            else if(m_vector_size != vector_size)
            {
                throw std::length_error("New vector length does not match the initial one.");
            }
        }

        /**
         * The maximum size of the queue.
         */
        size_t m_queue_size;

        /**
         * The desired length of the vectors stored by this object.
         */
        size_t m_vector_size;

        /**
         * The shared mutex used for access synchronization.
         */
        mutable std::shared_mutex m_mutex;
    };
}

#endif
