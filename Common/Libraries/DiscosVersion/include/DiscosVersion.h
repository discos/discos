/* ************************************************************************************************************* */
/* DISCOS project                                                                               */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                    when            What                                                       */
/* Andrea Orlati (a.orlati@ira.inaf.it)   25/05/2017
*/

#include <IRA>

#ifndef DISCOSVERSION_H_
#define DISCOSVERSION_H_



#ifdef COMPILE_TARGET_MED

#define _STATION "Medicina"

#elif COMPILE_TARGET_NT

#define _STATION "Noto"

#else

#define _STATION "SRT"

#endif 

#define _CURRENT_VERSION_MAJOR 1
#define _CURRENT_VERSION_MINOR 0

#define _CURRENT_PRODUCT "DISCOS"

namespace DiscosVersion {

class CurrentVersion {
public:
	static WORD major;
	static WORD minor;
	static IRA::CString station;
	static IRA::CString product;
	static IRA::CString getVersion();
};
 
}


#endif /* DISCOSVERSION_H_ */
