#ifndef _CALLBACK_H_
#define _CALLBACK_H_

/* ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/* $Id: Callback.h,v 1.2 2010-10-14 12:23:36 a.orlati Exp $										         */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                            when             What                                                       */
/* Andrea Orlati(aorlati@ira.inaf.it)  18/03/2008     Creation                                                  */

#include <baci.h>

class CBossCore;

class CCallback : public virtual POA_ACS::CBvoid
{
public:
	enum TOperation {
		STOW,
		UNSTOW,
		SETUP
	};
	CCallback(const TOperation& op) :  m_operation(op) {}
	~CCallback() {}
	void working (const ACSErr::Completion &c, const ACS::CBDescOut &desc) throw (CORBA::SystemException);
	void done (const ACSErr::Completion &c, const ACS::CBDescOut &desc) throw (CORBA::SystemException);
	CORBA::Boolean negotiate (ACS::TimeInterval time_to_transmit, const ACS::CBDescOut &desc) throw (CORBA::SystemException) {
		return true;
	}
	void setBossCore(CBossCore* core) { m_core=core; }
private:
	TOperation m_operation;
	CBossCore *m_core;
};

#endif /*CALLBACK_H_*/

