#ifndef __WPUTILS_H__
#define __WPUTILS_H__

#include <IRA>
#include <MSParameters.h>
using namespace IRA;


class PosNotFoundEx: public std::logic_error {
    public:
        PosNotFoundEx(const std::string &s): std::logic_error(s) {};
        virtual ~PosNotFoundEx() throw() {};
};


vector<PositionItem>::size_type findPositionIndex(
        CSecAreaResourceWrapper< map<int, vector<PositionItem> > > vptr, 
        ACS::Time exe_time,
        unsigned short address,
        bool clear=false
) throw (PosNotFoundEx);

#endif
