#include <WPUtils.h>
#include <LogFilter.h>
#include <IRA>

vector<PositionItem>::size_type findPositionIndex(
        CSecAreaResourceWrapper< map<int, vector<PositionItem> > > vptr, 
        ACS::Time exe_time,
        unsigned short address,
        bool clear)
    throw (PosNotFoundEx)
{
    vector<PositionItem> cmd_list;
    vector<PositionItem>::size_type idx = 0;
    bool found = false;
    if((*vptr).count(address)) {
        cmd_list = (*vptr)[address];
        for(; idx != cmd_list.size(); idx++) {
            if((cmd_list[idx]).exe_time <= exe_time) {
                if(idx + 1 < cmd_list.size()) {
                    if((cmd_list[idx + 1]).exe_time > exe_time) {
                        found = true;
                        break;
                    }
                    else {
                        continue;
                    }
                }
                else {
                    found = true;
                    break;
                }
            }
            else {
                break;
            }
        }
    }
    if(!found)
        throw PosNotFoundEx("Position not found");
    else 
        if(clear && (*vptr).count(address)) {
            vector<PositionItem>::iterator ibegin = ((*vptr)[address]).begin();
            ((*vptr)[address]).erase(ibegin, ibegin + idx);
            return 0;
        }

    return idx;
}

