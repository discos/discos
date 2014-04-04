/*******************************************************************************\
 *  Author Infos
 *  ============
 *  Name:         Marco Buttu
 *  E-mail:       mbuttu@oa-cagliari.inaf.it
 *  Personal Web: http://www.pypeople.com/
\*******************************************************************************/

#ifndef __UTILS_H__
#define __UTILS_H__

#include <string>
#include <vector>
#include <ComponentErrors.h>
#include <IRA>

#define BYTE unsigned char
#define NEXT_FUTURE_DELTA 100000 // In one hundred ns (10ms)

#define __UTILS_STREAM_PREC__ 16

using namespace std;

typedef string::size_type (string::*find_t)(const string &delim, string::size_type offset) const;


/**
 * Convert a `readdata` word (stored in an array slice of hexadecimal ASCII codes) to a decimal value
 *
 * @param buff array containing hexadecimal ASCII codes
 * @param idx index of first byte of the readdata word to convert
 * @param len length of readdata 
 * @return decimal vaule of readdata
 * @throw ComponentErrors::SocketErrorExImpl
 */
int hex2dec(BYTE *buff, int idx, int len) throw (ComponentErrors::SocketErrorExImpl);


void dec2hexStr(unsigned long dec, unsigned char hex[], int len);


/**
 * The position bytes are stored as chars, so word2binChar converts the
 * position bytes to binary and save the result in binChar. Every item
 * of binChar array is a char that stand for a bit ('0' or '1')
 * @arg *dst destination buffer in which to store the binary string position
 * @arg bytes[] array of bytes in which is stored the position 
 * @len word length
 */
void word2binChar(char *dst, unsigned char bytes[], int len);


/**
 * Return the integer value stored in a string.
 *
 * How to use it
 * =============
 * This code:
 *
 *     cout << str2int("44")/2 << endl;
 *
 * produces the following output:
 *
 *     22
 *
 * Parameters and returned value
 * =============================
 *
 * @param s: the string being evaluate
 * @return the integer value stored in the string
 */
int str2int(const string &s);


/**
 * Return the double value stored in a string.
 *
 * How to use it
 * =============
 * This code:
 *
 *     cout << str2double("44.22")/2 << endl;
 *
 * produces the following output:
 *
 *     22.11
 *
 * Parameters and returned value
 * =============================
 *
 * @param s: the string being evaluate
 * @return the double value stored in the string
 */
double str2double(const string &s);


string char2string(const char c);


/**
 * Return the number as a string.
 *
 * How to use it
 * =============
 * This code:
 *
 *     cout << "After conversion: " + numbert2str(44) << endl;
 *
 * produces the following output:
 *
 *     After conversion: 44
 *
 * Parameters and returned value
 * =============================
 *
 * @param num: a number
 * @return the number as a string
 */
template<class T>
string number2str(T num) {
    ostringstream ostr;
    ostr.precision(__UTILS_STREAM_PREC__);
    ostr.str("");
    ostr << num;
    return ostr.str();
 }

template<class T>
T abs_diff(T first, T second) {
    if(first >= second)
        return first - second;
    else
        return second - first;
}

template<class T>
T abs_(T num) {
    if(num > 0)
        return num;
    else
        if(num < 0)
            return -num;
        else
            return 0;
 }


/**
 * Return true if the vector `v` starts with the `token`.
 *
 * @param v: the vector being evaluate
 * @param token: the item to match with the first element of v
 */
template<typename T>
bool vstartswith(const vector<T> v, const T token) {
    if(v.empty())
        return false;
    else
        return (v[0] == token);
}



/**
 * Split the string s on the given delimiter(s) and return a list of tokens 
 * without the delimiter(s).
 *
 * How to use it
 * =============
 * This code:
 *
 *     string foo("Hi, the comma (,) is a typical example of token.");
 *     string token(",");
 *     vector<string> new_string = split(foo, token);
 *     for (vector<string>::size_type idx = 0; idx != new_string.size(); ++idx)
 *         cout << new_string[idx] << endl;
 *
 * produces the following output:
 *
 *     Hi
 *      the comma (
 *     ) is a typical example of token.
 *
 *
 * Parameters and returned value
 * =============================
 *
 * @param s: the string being split
 * @param match: the delimiter(s) for splitting
 * @param removeEmpty: remove empty tokens from the list
 * @param fullMatch: true if the whole match string is a match, false if
 *                   any character in the match string is a match.
 * @return a list of tokens
 */
vector<string> split(const string &s, const string &match, bool removeEmpty = true, bool fullMatch = false);


/**
 * Strip the string s on the given token (default is " ")  and return the
 * tripped string.
 *
 * How to use it
 * =============
 * This code:
 *
 *     string foo("    This is a typical string to strip.");
 *     cout << "The string before and after stripping:" << endl << endl;
 *     cout << foo << endl;
 *     strip(foo);
 *     cout << foo << endl;
 *
 * produces the following output:
 *
 *     The string before and after stripping:
 *
 *         This is a typical string to strip.
 *     This is a typical string to strip.
 *
 *
 * Parameters and returned value
 * =============================
 *
 * @param s: the string being strip
 * @param token: the string to remove at the beginning and at the end of s
 */
void strip(string & s, const string & token = " ");


/**
 * Return true if the string `s` starts with the `token`.
 *
 * How to use it
 * =============
 * This code:
 *
 *     string mystring("My string...");
 *     string token("My");
 *     if (startswith(mystring, token))
 *         cout << "Ok, it does!" << endl;
 *
 * produces the following output:
 *
 *     Ok, it does!
 *
 * Parameters and returned value
 * =============================
 *
 * @param s: the string being evaluate
 * @param token: the string to match with the beginning of s
 */
bool startswith(const string & s, const string & token);


/**
 * Return true if the string `s` ends with the `token`.
 *
 * How to use it
 * =============
 * This code:
 *
 *     string mystring("My string...");
 *     string token("..");
 *     if (startswith(mystring, token))
 *         cout << "Ok, it does!" << endl;
 *
 * produces the following output:
 *
 *     Ok, it does!
 *
 * Parameters and returned value
 * =============================
 *
 * @param s: the string being evaluate
 * @param token: the string of characters to match with the end of s
 */
bool endswith(const string & s, const string & token);


DDWORD getNextTime(unsigned long seconds=0, unsigned long mseconds=0);

#endif
