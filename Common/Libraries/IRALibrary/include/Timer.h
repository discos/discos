
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id: DateTime.h,v 1.10 2014-06-18 18:56:56 m.bartolini Exp $										                                                            */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                 when           What                                              */
/* Marco Bartolini (bartolini@ira.inaf.it)  18/06/2014     Creation                                     */

#ifndef TIMER_H
#define TIMER_H

#include "IRATools.h"
#include "DateTime.h"
#include <acstimeEpochHelper.h>

namespace IRA{
    class CTimer{
        public:
            CTimer();
            ACS::Time elapsed();
            void reset();
        private:
            ACS::Time _start_time;
    };
}

#endif

