#ifndef FASTQUEUE_H_
#define FASTQUEUE_H_

/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it) 06/11/2015      Creation                                          */

namespace IRA {

/**
 * This template class implements a fast queue (First In First out container). The implementation is based on a circular array and it permits to avoid re-allocation to address
 * performance issues
*/
template<class T> class CFastQueue {
public:

	/**
	 * Constructor.
	 * It construct the buffer with the specified number of elements. The array cannot be resized.
	 * @param positions the size of the buffer.
	*/
	CFastQueue(const unsigned& positions) : m_positions(positions+1), m_head(0), m_end(0), m_array(NULL)
	{
		m_array=new T[m_positions] ;
	}
	
	/**
	 * Destructor.
	*/
	~CFastQueue() {
		if (m_array) delete []m_array;
	}
	
	/**
	 * @return true is the queue is full
	 */
	inline bool isFull() const {
		return (size()==m_positions-1);
	}

	/**
	 * @return true is the queue is empty
	 */
	inline bool isEmpty() const {
		return (size()==0);
	}

	inline unsigned size() const {
		return (m_head-m_end+m_positions)%m_positions;
	}

	/**
	 * puts an element in the tail of the queue.
	 * @param element new element to be added
	 * @return false if the container is full and no other elements could be added
	 */
	bool pushBack(const T& element) {
		if (isFull()) return false;
		m_array[m_head] = element;
		m_head = (m_head+ 1)%m_positions; // if full an element is overwritten
		return true;
	}

	/**
	 * pop  an element from the front of the queue
	 * @element the next element is the queue
	 * @return false if the queue is empty and the element could not be pop
	 */
	bool  popFront(T& element) {
	    if (isEmpty()) return false;
	    element=m_array[m_end];
	    m_end=(m_end+1)%m_positions; // if it is not empty
	    return true;
	}
	
	/**
	 * @return the reference to the element that is going to come out of the queue
	 */
	const T& front() const {
		return m_array[m_end];
	}

protected:
	unsigned m_positions;
	/** beginning of the array */
	unsigned m_head;
	/** end of the array */
	unsigned m_end;
	T *m_array;
private:
	CFastQueue(const CFastQueue&);
	const CFastQueue& operator =(const CFastQueue& src);
};
	
}

#endif /*FASTQUEUE_H_*/

