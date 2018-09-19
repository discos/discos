#ifndef BASE_CONVERTER_H
#define BASE_CONVERTER_H


/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                  when            What                                              */
/* Andrea Orlati(andrea.orlati@inaf.it) 24/07/2018      Creation                                          */


#include "String.h"

#include <vector>
#include <string>
#include <iostream>

namespace IRA{
	
	/**
	* This class contains some tools method to convert the base representing integer numbers.
 	* Initially some tools will be missing, they will be implemented as soon as they are required.
	*/
	class CBaseConverter {
	public:

		/**
		 * This static method converts a decimal number to its binary representation
		 * @param number decimal number to be converted
		 */	
		template<typename T> static IRA::CString decToBin(T number) {
			std::vector<char> result;
			decToBitSequence(number,result);
    		return CBaseConverter::binary(result);
		}		

		/**
		 * This static method converts a decimal number to its Hex representation
		 * @param number decimal number to be converted
		*/	
		template<typename T> static IRA::CString decToHex(T number) {
			std::vector<char> result;
			decToBitSequence(number,result);
    		return CBaseConverter::hex(result);
		}		

	private:
	
		template<typename T> static void decToBitSequence(T number,std::vector<char>& result) {
			unsigned bytes=sizeof(T);
			unsigned digits=bytes*8;
			result.empty();
			result.resize(digits,'0');
			unsigned pos=0;
    		do {
    			if ((number & 1)==0)
    				result[pos]='0';
    			else
    				result[pos]='1';
    			pos++;
    		} while ((number>>=1) && (pos<digits));
		}
		static IRA::CString binary(const std::vector<char>& v);
		static IRA::CString hex(const std::vector<char>& v);
	};
	
};



#endif