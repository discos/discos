#include "BaseConverter.h"


using namespace IRA;

IRA::CString CBaseConverter::binary(const std::vector<char> & v)
{
	std::string out="";
	for (unsigned iter=v.size();iter>0;iter--) { 
		out+=v[iter-1];
	}
	return IRA::CString(out.c_str());
}

IRA::CString CBaseConverter::hex(const std::vector<char> & v)
{
	std::string binToHex="",tmp="0000";
	for (int j=v.size()-1;j>=0;j-=4){
		for(unsigned i=0;i<4;i++) {
        	tmp[i]=v[j-i];  
        }
        if (!tmp.compare("0000")) binToHex+="0";
        else if (!tmp.compare("0001")) binToHex+="1";
        else if (!tmp.compare("0010")) binToHex+="2";
        else if (!tmp.compare("0011")) binToHex+="3";
        else if (!tmp.compare("0100")) binToHex+="4";
        else if (!tmp.compare("0101")) binToHex+="5";
        else if (!tmp.compare("0110")) binToHex+="6";
        else if (!tmp.compare("0111")) binToHex+="7";
        else if (!tmp.compare("1000")) binToHex+="8";
        else if (!tmp.compare("1001")) binToHex+="9";
        else if (!tmp.compare("1010")) binToHex+="A";
        else if (!tmp.compare("1011")) binToHex+="B";
        else if (!tmp.compare("1100")) binToHex+="C";
        else if (!tmp.compare("1101")) binToHex+="D";
        else if (!tmp.compare("1110")) binToHex+="E";
        else if (!tmp.compare("1111")) binToHex+="F";
        else continue;
    }
    return IRA::CString(binToHex.c_str());
}

