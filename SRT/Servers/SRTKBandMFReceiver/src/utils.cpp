/*******************************************************************************\
 *  Author Infos
 *  ============
 *  Name:         Marco Buttu
 *  E-mail:       mbuttu@oa-cagliari.inaf.it
 *  Personal Web: http://www.pypeople.com/
\*******************************************************************************/

#include "utils.h"
#include <sstream>
#include <cctype>
#include <LogFilter.h>

int hex2dec(BYTE *buff, int idx, int len) throw (ComponentErrors::SocketErrorExImpl) {

    unsigned int dec_array[len] ; // This array stores decimal values
    unsigned long dec_value = 0; // The result as decimal value 

    for(int i=0; i<len; i++) {
        dec_array[i] = int(buff[idx + i]) ;
        // If the ASCII decimal code is related to a letter char than I shift it of 55 to obtain
        // the corresponding decimal value of hexadecimal letter. For instance the letter 'A'  
        // has a 65 decimal ASCII code: I strore 65-55 = 10 <==> dec(A) == 10
        if(dec_array[i] > 64 && dec_array[i] < 71)
            dec_array[i] -= 55 ;
        else if(dec_array[i] > 47 && dec_array[i] < 58)
            // This is the case the ASCII decimal code is related to a digit
            dec_array[i] -= 48 ;
        else {
            ACS_DEBUG("icdSocket::hex2dec", "ICD hexadecimal ASCII code wrong");
            ComponentErrors::SocketErrorExImpl exImpl(__FILE__, __LINE__, 
                    "icdSocket::hex2dec, ICD communication problem: wrong hexadecimal ASCII code received");
            throw exImpl;
        }

        dec_value += static_cast<unsigned int>(dec_array[i]*pow(16, len - 1 - i)) ;
    }
    return dec_value;
}


void dec2hexStr(int dec, unsigned char hex[], int len) {

    int quotient;
    char rev_hex[len];
    int remainder;
    int i = 0;

    quotient = dec;
    while(quotient > 0) {
        remainder = quotient % 16;
        quotient = quotient / 16;
        if(remainder > 9)
        	rev_hex[i] = (char)remainder + 55;
        else 
        	rev_hex[i] = (char)remainder + 48;
        i++;
    }
    // Reverse String
    for(int j = 0; j < i; j++)
        hex[i-j-1] = rev_hex[j];
    
    hex[i] = '\0';
}


void word2binChar(char *dst, unsigned char bytes[], int len) {
    unsigned char bit;

    for (int i=0; i < len; i++) {
        for ( bit = 1 << (CHAR_BIT - 1); bit; bit >>= 1 ) {
        *dst++ = bytes[i] & bit ? '1' : '0';
        }
    }
    *dst = '\0';
}


int str2int(const string &s) {
    string s_copy(s);
    strip(s_copy);
    istringstream istm;
    istm.precision(__UTILS_STREAM_PREC__);
    istm.clear();
    istm.str("");
    int num;
    istm.str(s_copy);
    istm >> num;
    return num;
}

unsigned long str2ulong(const string &s) {
    string s_copy(s);
    strip(s_copy);
    istringstream istm;
    istm.precision(__UTILS_STREAM_PREC__);
    istm.clear();
    istm.str("");
    unsigned long num;
    istm.str(s_copy);
    istm >> num;
    return num;

}

double str2double(const string &s) {
    string s_copy(s);
    strip(s_copy);
    istringstream istm;
    istm.precision(__UTILS_STREAM_PREC__);
    istm.clear();
    istm.str("");
    double num;
    istm.str(s_copy);
    istm >> num;
    return num;
}


string char2string(const char c) {
    stringstream ss;
    string str;
    ss << c;
    ss >> str;
    return str;
}


vector<string> split(const string &s, const string &match, bool removeEmpty, bool fullMatch)
{
    vector<string> result; // return container for tokens
    string::size_type start = 0, skip = 1; // starting position for searches and positions to skip after a match
    find_t pfind = &string::find_first_of; // search algorithm for matches

    if (fullMatch)
    {
        // use the whole match string as a key
        // instead of individual characters
        // skip might be 0. see search loop comments
        skip = match.length();
        pfind = &string::find;
    }

    while (start != string::npos)
    {
        // get a complete range [start..end)
        string::size_type end = (s.*pfind)(match, start);

        // null strings always match in string::find, but
        // a skip of 0 causes infinite loops. pretend that
        // no tokens were found and extract the whole string
        if (skip == 0) end = string::npos;

        string token = s.substr(start, end - start);
        bool found = false;
        for(string::size_type idx = 0; idx < token.size(); idx++)
            if(isalnum(token[idx]))
                    found = true;

        if(found && !(removeEmpty && token.empty()))
            // extract the token and add it to the result list
            result.push_back(token);

        // start the next range
        if ((start = end) != string::npos) start += skip;
    }

    return result;
}


void strip(string &s, const string &token) {
    // ridx (lidx) is the index of first occurence starting from right (left)
    string::size_type ridx, lidx;
    string base_token("\n");
    while(true) {
        if(startswith(s, "\n") || startswith(s, " ")) {
            s.erase(0, 1);
            continue;
        }
        if(endswith(s, "\n") || endswith(s, " ")) {
            s.erase(s.size()-1);
            continue;
        }
        ridx = s.rfind(token);
        lidx = s.find(token);
        if (ridx == s.size() - 1)
            // Delete the last char
            s.erase(ridx);
        else
            if (lidx == 0)
                // Delete the one char starting from index lidx == 0
                s.erase(lidx, 1);
            else
                break;
    }
}


bool startswith(const string & s, const string & token) {
    if(s.size() < token.size())
        return false;
    
    for (vector<string>::size_type idx = 0; idx != token.size(); ++idx)
        if(token[idx] != s[idx])
            return false;

    return true;
    
}


bool endswith(const string & s, const string & token) {
    if(s.size() < token.size())
        return false;

    for(vector<string>::size_type idx = 0; idx != token.size(); ++idx)
        if(token[idx] != s[s.size() - token.size() + idx])
            return false;

    return true;
}


string remove(string str, const char c) {
    stringstream ss;
    string token;
    ss << c;
    ss >> token;
    string result("");
    for(vector<string>::size_type idx = 0; idx != str.size(); ++idx) {
        string item;
        ss.clear();
        ss << str[idx];
        ss >> item;
        if(item != token)
            result += item;
    }
    return result;
}
    

DDWORD getNextTime(unsigned long seconds, unsigned long mseconds) {
    TIMEVALUE now;
    TIMEDIFFERENCE duration;
    IRA::CIRATools::getTime(now);
    if(seconds) duration.second(seconds);
    if(mseconds) duration.microSecond(mseconds * 1000);
    if(seconds || mseconds) now.add(duration.value());
    return seconds + mseconds ? (now.value()).value : 0; // Return 0 if seconds + mseconds == 0
}


vector<PositionItem>::size_type findPositionIndex(
        CSecAreaResourceWrapper<vector<PositionItem> > vptr, 
        ACS::Time exe_time,
        bool clear)
    throw (PosNotFoundEx)
{
    vector<PositionItem> pos_list = *vptr;
    vector<PositionItem>::size_type idx = 0;
    bool found = false;
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

    if(!found)
        throw PosNotFoundEx("Position not found");
    else 
        if(clear) {
            vector<PositionItem>::iterator ibegin = (*vptr).begin();
            (*vptr).erase(ibegin, ibegin + idx);
            return 0;
        }

    return idx;
}


PositionItem getPosItemFromHistory(
        CSecAreaResourceWrapper<vector<PositionItem> > vptr, 
        ACS::Time exe_time
        )
    throw (PosNotFoundEx, IndexErrorEx)
{
    vector<PositionItem> * pos_list = &(*vptr);
    PositionItem result;

    if(pos_list->empty()) {
        throw PosNotFoundEx("The position history vector is empty.");
    }

    const size_t size = pos_list->size();
    size_t idx_begin = 0;
    size_t idx_end = size - 1;
    if(exe_time > ((*pos_list)[size-1]).exe_time || exe_time == 0) {
        idx_begin = idx_end = size - 1;
    }
    else if(exe_time < ((*pos_list)[0]).exe_time) {
        idx_begin = idx_end = 0;
    }
    else {
        size_t min_size = 16; // Bisection, till we get a chunk with size < min_size
        while((idx_end - idx_begin) > min_size) {
            size_t idx_ref = idx_begin + size_t((idx_end - idx_begin) / 2);
            if((*pos_list)[idx_ref].exe_time == exe_time) {
                idx_begin = idx_end = idx_ref;
                break;
            }
            else {
                if((*pos_list)[idx_ref].exe_time < exe_time)
                    idx_begin = idx_ref;
                else
                    idx_end = idx_ref;
            }
        }
    }
    
    if(idx_begin == idx_end) {
        result.position = ((*pos_list)[idx_begin]).position;
        result.exe_time = ((*pos_list)[idx_begin]).exe_time;
        return result;
    }
    else { // Look for the position
        vector<PositionItem>::size_type idx = 0;
        for(idx=idx_begin; idx<idx_end; idx++) {
            if(((*pos_list)[idx]).exe_time <= exe_time) {
                if(idx + 1 < idx_end) {
                    if(((*pos_list)[idx + 1]).exe_time > exe_time) {
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
            // Set the timestamp
            result.exe_time = exe_time;
            // Get the position, performing a linear interpolation
            double pa, pb; // pa means `position a` (idx), pb means `position b` (idx+1) 
            ACS::Time ta, tb; // ta -> timestamp of the (idx) item; tb -> timestamp of (idx+1)
            ta = ((*pos_list)[idx]).exe_time;
            tb = ((*pos_list)[idx+1]).exe_time;
            pa = ((*pos_list)[idx]).position;
            pb = ((*pos_list)[idx+1]).position;
            if((tb - ta) > 0) {
                result.position = pa + (exe_time - ta)*(pb - pa)/(tb - ta);
            }
            else {
                throw PosNotFoundEx("Time difference not positive.");
            }
        }
        else {
            throw IndexErrorEx("Index out of range");
            // Exception: index out of range
        }
    }

    return result;
}


