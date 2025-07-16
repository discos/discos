#ifndef __SRTMINORSERVOOFFSETSQUEUE_H__
#define __SRTMINORSERVOOFFSETSQUEUE_H__

/**
 * SRTMinorServoOffsetsQueue.h
 * Giuseppe Carboni (giuseppe.carboni@inaf.it)
 */

#include <map>
#include <vector>
#include <shared_mutex>


namespace MinorServo
{
    class SRTMinorServoOffsetsQueue : private std::map<ACS::Time, std::pair<std::vector<double>, std::vector<double>>>
    {
    public:
        SRTMinorServoOffsetsQueue() : m_queue_size(0), m_vector_size(0) {}

        SRTMinorServoOffsetsQueue(size_t queue_size) : m_queue_size(queue_size), m_vector_size(0)
        {
            std::unique_lock<std::shared_mutex> lock(m_mutex);
            queueLazyInit(queue_size);
        }

        SRTMinorServoOffsetsQueue(size_t queue_size, size_t vector_size) : m_queue_size(queue_size), m_vector_size(vector_size)
        {
            std::unique_lock<std::shared_mutex> lock(m_mutex);
            queueLazyInit(queue_size);
            vectorLazyInit(vector_size);
        }

        SRTMinorServoOffsetsQueue& operator=(const SRTMinorServoOffsetsQueue& other)
        {
            if(this != &other)
            {
                std::unique_lock<std::shared_mutex> lockThis(m_mutex, std::defer_lock);
                std::unique_lock<std::shared_mutex> lockOther(other.m_mutex, std::defer_lock);
                std::lock(lockThis, lockOther);
                m_queue_size = other.m_queue_size;
                m_vector_size = other.m_vector_size;
                std::map<ACS::Time, std::pair<std::vector<double>, std::vector<double>>>::operator=(other);
            }
            return *this;
        }

        void put(ACS::Time key, const std::vector<double>& v1, const std::vector<double>& v2)
        {
            std::unique_lock<std::shared_mutex> lock(m_mutex);

            vectorLazyInit(v1.size());
            vectorLazyInit(v2.size());
            queueLazyInit(m_queue_size);

            if(std::map<ACS::Time, std::pair<std::vector<double>, std::vector<double>>>::size() == m_queue_size)
            {
                this->erase(this->begin());
            }
            this->emplace(std::make_pair(key, std::make_pair(v1, v2)));
        }

        std::vector<double> getUserOffsets(ACS::Time key)
        {
            return get(key).first;
        }

        std::vector<double> getSystemOffsets(ACS::Time key)
        {
            return get(key).second;
        }

        size_t size() const
        {
            std::shared_lock<std::shared_mutex> lock(m_mutex);
            return this->std::map<ACS::Time, std::pair<std::vector<double>, std::vector<double>>>::size();
        }

        void clear()
        {
            std::unique_lock<std::shared_mutex> lock(m_mutex);
            this->std::map<ACS::Time, std::pair<std::vector<double>, std::vector<double>>>::clear();
        }

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
        void queueLazyInit(size_t queue_size)
        {
            if(m_queue_size == 0)
            {
                if(queue_size == 0 || queue_size > this->max_size())
                {
                    throw std::length_error("Invalid queue size.");
                }
                m_queue_size = queue_size;
            }
        }

        void vectorLazyInit(size_t vector_size)
        {
            if(vector_size == 0 || vector_size > 6)
            {
                throw std::length_error("Invalid vector size.");
            }
            if(m_vector_size == 0)
            {
                m_vector_size = vector_size;
            }
            else if(m_vector_size != vector_size)
            {
                throw std::length_error("Vector size mismatch.");
            }
        }

        std::pair<std::vector<double>, std::vector<double>> get(ACS::Time key)
        {
            std::shared_lock<std::shared_mutex> lock(m_mutex);

            auto it = this->upper_bound(key);
            if(this->empty() || it == this->begin())
            {
                return std::make_pair(std::vector<double>(m_vector_size, 0.0), std::vector<double>(m_vector_size, 0.0));
            }

            --it;
            return it->second;
        }

        size_t m_queue_size;
        size_t m_vector_size;
        mutable std::shared_mutex m_mutex;
    };
}

#endif
