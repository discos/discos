#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id: Protocol.h,v 1.1 2011-03-14 15:16:22 a.orlati Exp $																								*/
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it) 10/06/2010      Creation                                         */

#include <IRA>

class CProtocol {
public:
	static float getRandomValue(const double& att);
	static float getRandomValue();
};


#endif /*_PROTOCOL_H_*/
