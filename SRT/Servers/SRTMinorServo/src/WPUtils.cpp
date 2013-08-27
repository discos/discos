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
    vector<PositionItem> pos_list;
    vector<PositionItem>::size_type idx = 0;
    bool found = false;
    if((*vptr).count(address)) {
        pos_list = (*vptr)[address];
        for(; idx != pos_list.size(); idx++) {
            if((pos_list[idx]).exe_time <= exe_time) {
                if(idx + 1 < pos_list.size()) {
                    if((pos_list[idx + 1]).exe_time > exe_time) {
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


PositionItem getPosItemFromHistory(
        CSecAreaResourceWrapper< map<int, vector<PositionItem> > > vptr, 
        ACS::Time exe_time,
        unsigned short address
        )
    throw (PosNotFoundEx, IndexErrorEx)
{
    vector<PositionItem> pos_list;
    PositionItem result;
    if((*vptr).count(address)) {
        pos_list = (*vptr)[address];

        if(pos_list.empty()) {
            // cout << "list empty" << endl;
            throw PosNotFoundEx("The position history vector is empty.");
        }

        const size_t size = pos_list.size();
        size_t idx_begin = 0;
        size_t idx_end = size - 1;
        if(exe_time > (pos_list[size-1]).exe_time || exe_time == 0) {
            idx_begin = idx_end = size - 1;
        }
        else if(exe_time < (pos_list[0]).exe_time) {
            idx_begin = idx_end = 0;
        }
        else {
            size_t min_size = 16; // Bisection, till we get a chunk with size < min_size
            while((idx_end - idx_begin) > min_size){
                size_t idx_ref = size_t((idx_end - idx_begin) / 2);
                if(pos_list[idx_ref].exe_time == exe_time) {
                    idx_begin = idx_end = idx_ref;
                    break;
                }
                else {
                    if(pos_list[idx_ref].exe_time < exe_time)
                        idx_begin = idx_ref;
                    else
                        idx_end = idx_ref;
                }
            }
        }
        
        if(idx_begin == idx_end) {
            result.position = (pos_list[idx_begin]).position;
            (result.offsets).user = ((pos_list[idx_begin]).offsets).user;
            (result.offsets).system = ((pos_list[idx_begin]).offsets).system;
            result.exe_time = (pos_list[idx_begin]).exe_time;
            return result;
        }
        else { // Look for the position
            vector<PositionItem>::size_type idx = 0;
            for(idx=idx_begin; idx<idx_end; idx++) {
                if((pos_list[idx]).exe_time <= exe_time) {
                    if(idx + 1 < idx_end) {
                        if((pos_list[idx + 1]).exe_time > exe_time) {
                            break;
                        }
                        else {
                            continue;
                        }
                    }
                    else {
                        break;
                    }
                }
                else {
                    break;
                }
            }

            if(idx >=0 && (idx + 1) <= size-1) {
                (result.position).length(((pos_list[idx]).position).length());
                ((result.offsets).user).length((((pos_list[idx]).offsets).user).length());
                ((result.offsets).system).length((((pos_list[idx]).offsets).system).length());
                // Set the timestamp
                result.exe_time = exe_time;
                // Get the position, performing a linear interpolation
                double pa, pb; // pa means `position a` (idx), pb means `position b` (idx+1) 
                double uoa, uob; // user offset
                double soa, sob; // system offset
                ACS::Time ta, tb; // ta -> timestamp of the (idx) item; tb -> timestamp of (idx+1)
                for(size_t i=0; i<(result.position).length(); i++) {
                    ta = (pos_list[idx]).exe_time;
                    tb = (pos_list[idx+1]).exe_time;
                    pa = (pos_list[idx]).position[i];
                    pb = (pos_list[idx+1]).position[i];
                    uoa = ((pos_list[idx]).offsets).user[i];
                    uob = ((pos_list[idx+1]).offsets).user[i];
                    soa = ((pos_list[idx]).offsets).system[i];
                    sob = ((pos_list[idx+1]).offsets).system[i];
                    if((tb - ta) > 0) {
                        (result.position)[i] = pa + (exe_time - ta)*(pb - pa)/(tb - ta);
                        ((result.offsets).user)[i] = uoa + (exe_time - ta)*(uob - uoa)/(tb - ta);
                        ((result.offsets).system)[i] = soa + (exe_time - ta)*(sob - soa)/(tb - ta);
                    }
                    else {
                        // cout << "time diff not pos" << endl;
                        throw PosNotFoundEx("Time difference not positive.");
                    }
                }
            }
            else {
                throw IndexErrorEx("Index out of range");
                // Exception: index out of range
            }
        }
    }
    else {
        // cout << "cannot get the history" << endl;
        throw PosNotFoundEx("Cannot get the history.");
    }

    return result;
}
