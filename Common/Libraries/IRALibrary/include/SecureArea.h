#ifndef _SECUREAREA_H
#define _SECUREAREA_H

/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.cnr.it)  03/01/2005      Creation                                          */
/* Andrea Orlati(aorlati@ira.cnr.it)  22/02/2005      Restructured in order to avoid deadlock if a mutex*/ 
/*                                                    was not released     								*/
/* Andrea Orlati(aorlati@ira.cnr.it)  16/08/2005      Added Init method to CSecureArea					*/ 


// $Id: SecureArea.h,v 1.3 2011-06-03 18:02:37 a.orlati Exp $

#include <new>
#include "baciThread.h"

namespace IRA {

/**
 * This template class is used by the <i>CSecureArea</i> to make available the resource protected by a mutex mechanism.
 * This class is not intended to be used "stand-alone" and has been introduced in order to avoid deadlock if the mutex was not 
 * released. On object CSecureArea can be used as it is a pointer to the protected object (template typename). If the mutex is not 
 * released esplicitally it will be released when the <i>CSecAreaResourceWrapper</i> object get out of scope. For more information
 * see the <i>CSecureArea</i> documentation.
 * @author <a href=mailto:a.orlati@ira.cnr.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br>
*/	
template <class X> class CSecAreaResourceWrapper {
public:
	/**
	 * Constructor. 
	 * @param Mutex pointer to the mutex that must be allocated in order to synchronize the resource.
	 * @param Res pointer to the resource
	*/
	CSecAreaResourceWrapper(BACIMutex *Mutex,X *Res);
   /**
	 * Destructor
   */
	~CSecAreaResourceWrapper();
	/**
	 * Releases esplicitally the lock if it is still acquired, otherwise it does nothing.
	*/
	void Release();
	/**
	 * @return the pointer of the protected resource. If the lock is already released that means that
	 *         th program has a bug and the process is terminated (assert)
	*/
	X* operator->();
	/**
	 * @return the reference to the protected resource. If the lock is already released that means that
	 *         the program has a bug and the process is terminated (assert)
	*/
	X& operator*();

	/**
	 * it could be used to get direct access to the protected resource....very dangerous because
	 * you could access a resource which is already freed
	 */
	operator const X *() const {
		assert(m_pLockResource!=NULL);
		return m_pLockResource;
	}
private:
	/** Pointer to the mutex */
	BACIMutex* m_pLockMutex;
	/** Pointer to the protected resource */
	X* m_pLockResource;
   /** Indicates if the lock has been released or not */
	bool m_bReleased;
   void operator=(const CSecAreaResourceWrapper& rSrc);  // no implementation given	
};

/**
 * This template class contains the mechanism that can be used to protect a resource and synchronize all the threads that need to
 * make use of it. A thread must call <i>Get()</i> to be granted the priviledge to use the resource. An object <i>CSecAreaResourceWrapper</i>
 * is returned that let the developer use the resources and eventually forget to release the mutex lock. An example of usage:
 * <pre>
 *		//allocate the secure area and protect an integer.....
 *		CSecureArea<int>Area(new int(10));
 *		//obtain the access to the resource....
 *		CSecAreaResourceWrapper<int> Value=Area.Get();
 *		//make use of it....
 *		printf("%d",*Value);
 *		*Value=40;
 * </pre>
 * @author <a href=mailto:a.orlati@ira.cnr.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br>
*/
template <class T> class CSecureArea {
public:
	/**
	 * Constructor
	 * @param Obj pointer to the object/resource that must be protected
   */
	CSecureArea(T* Obj);
	/** Constructor
	 * @param Alloc this boolean tells the constructor if it has to allocate the resource automatically or not.
	*/
	CSecureArea(bool Alloc=false);
	/**
    * Destructor.
    * It also takes charge of deleting the resource.
	*/
	~CSecureArea();
	/**
    * Call this member function to pass the pointer to the object you need to protect. If the object/resoruce was already 
	* initialized, this function has no effect.
	* @param Obj pointer to the object/resource that must be protected
	*/
	void Init(T* Obj);
	/**
    * Call this member function to be granted the possibility to access the resource. If the lock of the resource is already
    * allocated this function will wait util it is released. 
    * @return the resource wrapper that permits the developer to make use of the property.
	*/
	CSecAreaResourceWrapper<T> Get();
private:
	/** Mutex used to lock/unlock the resource */
	BACIMutex m_Mutex;
   	/** Pointer to the resource */
	T *m_pResource;
	CSecureArea(const CSecureArea& rSrc); // no implementation given
   	void operator=(const CSecureArea& rSrc);  // no implementation given
};

}

using namespace IRA;

template <class X> CSecAreaResourceWrapper<X>::CSecAreaResourceWrapper(BACIMutex *Mutex,X *Res): m_pLockMutex(Mutex), m_pLockResource(Res), m_bReleased(true) {
	m_pLockMutex->acquire();
	m_bReleased=false;
}

template <class X> CSecAreaResourceWrapper<X>::~CSecAreaResourceWrapper() {
	Release();
}

template <class X> void CSecAreaResourceWrapper<X>::Release() {
	if (!m_bReleased) {
		m_pLockMutex->release();
		m_pLockResource=NULL;
		m_bReleased=true;
	}
}

template <class X> X* CSecAreaResourceWrapper<X>::operator->() {
	assert(m_pLockResource!=NULL);
	return m_pLockResource;
}

template <class X> X& CSecAreaResourceWrapper<X>::operator*() {
	assert(m_pLockResource!=NULL);
	return *m_pLockResource;
}

template <class T> CSecureArea<T>::CSecureArea(T* Obj): m_pResource(Obj)
{
}

template <class T> CSecureArea<T>::CSecureArea(bool Alloc): m_pResource(NULL)
{
	if (Alloc) {
		try {
			m_pResource=(T *)new T();
		}
		catch (std::bad_alloc& ex) {
			m_pResource=NULL;
		}
	}
}

template <class T> CSecureArea<T>::~CSecureArea()
{
	if (m_pResource) {
		m_Mutex.acquire(); //before closing the resource it is safe to acquire the mutex
		delete m_pResource;
		m_pResource=NULL;
	}
}

template <class T> void CSecureArea<T>::Init(T* Obj)
{
	if (!m_pResource) {
		m_pResource=Obj;
	}
}

template <class T> CSecAreaResourceWrapper<T> CSecureArea<T>::Get()
{
	return CSecAreaResourceWrapper<T>(&m_Mutex,m_pResource);
}

#endif /* _SECUREAREA_H */
