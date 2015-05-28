/* ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                    when            What                                                       */
/* Andrea Orlati (a.orlati@ira.inaf.it)   29/04/2015      created                         */


#ifndef FILE_H_
#define FILE_H_

#include <IRA>
#include <list>
#include <boost/preprocessor/iteration/local.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/comparison/equal.hpp>


#define _FILE_DOUBLE_TYPE CDoubleTypeEntry
#define _FILE_STRING_TYPE CStringTypeEntry
#define _FILE_LONG_TYPE CLongTypeEntry

#define _FILE_SINGLE_ENTRY 1
#define _FILE_MULTI_ENTRY 2

#define _FILE_EXPAND(n) SCHEMA_ENTRY##n
#define _FILE_CREATEFILE(n) _FILE_LOCAL_FILE->createEntry<BOOST_PP_TUPLE_ELEM(4, 0, _FILE_EXPAND(n)), \
														  !BOOST_PP_EQUAL(BOOST_PP_TUPLE_ELEM(4,1,_FILE_EXPAND(n)),1) > \
														( BOOST_PP_TUPLE_ELEM(4, 2, _FILE_EXPAND(n)), \
													      BOOST_PP_TUPLE_ELEM(4, 3, _FILE_EXPAND(n)) );

// note, the blank between the macro and the parenthesis is mandatory
#define BOOST_PP_LOCAL_LIMITS (1, SCHEMA_ENTRY_NUMBER)
#define BOOST_PP_LOCAL_MACRO(n) _FILE_CREATEFILE(n)

#define _FILE_COMPARE(H,K) BOOST_PP_EQUAL(H,K)

namespace FitsWriter_private {

typedef enum {
	_FILE_STRING_TYPE_S,
	_FILE_DOUBLE_TYPE_S,
	_FILE_LONG_TYPE_S
} TFileTypeEnumeration;

class CDoubleTypeEntry {
public:
	static double defaultValue;
	typedef double TEntryType;
	typedef double TEntryReference;
	static TFileTypeEnumeration typeSpec;
};

class CStringTypeEntry {
public:
	static IRA::CString defaultValue;
	typedef IRA::CString TEntryType;
	typedef const char * TEntryReference;
	static TFileTypeEnumeration typeSpec;
};

class CLongTypeEntry {
public:
	static long defaultValue;
	typedef long TEntryType;
	typedef long TEntryReference;
	static TFileTypeEnumeration typeSpec;
};

class CEntry {
public:
	virtual ~CEntry() {};
	virtual const char *getKeyword() const { return (const char *)keyword; };
	virtual const char *getDescription() const { return (const char*)description; }
	virtual TFileTypeEnumeration getType() const { return typeSpec; }
	virtual bool isSimple() const { return !multi; }
protected:
	CEntry(): keyword(""), description("") { };
	//CEntry(const IRA::CString& key,const IRA::CString& desc): keyword(key), description(desc) { };
	CEntry(const IRA::CString& key,const IRA::CString& desc,const TFileTypeEnumeration& t,const bool& m): keyword(key), description(desc),
			typeSpec(t), multi(m) { };
	IRA::CString keyword;
	IRA::CString description;
	TFileTypeEnumeration typeSpec;
	bool multi;
};

template <class T,bool MULTI> class CHeaderEntry : public virtual CEntry {
public:
	typedef typename T::TEntryType _type;
	typedef typename T::TEntryReference _ref;
	CHeaderEntry() { };
	CHeaderEntry(const IRA::CString& key,const IRA::CString& desc): CEntry(key,desc,T::typeSpec,MULTI), value(T::defaultValue)
	{
	};
	CHeaderEntry(const IRA::CString& key,const IRA::CString& desc,const T& val): CEntry(key,desc,T::typeSpec,MULTI), value(val)
	{
	};
	virtual ~CHeaderEntry() {
	};
	CHeaderEntry& operator=(const _type& val) {
		value=val;
		return *this;
	};
	_ref getValue() const { return value; }
private:
	_type value;
};

template <class T> class CHeaderEntry<T,true>: public virtual CEntry {
public:
	typedef typename T::TEntryType _type;
	typedef typename T::TEntryReference _ref;
	typedef typename std::list<_type> TSequence;
	typedef typename TSequence::iterator TScroll;
	typedef typename std::list<_ref> TRefSequence;
	typedef typename TRefSequence::iterator TRefScroll;
	CHeaderEntry(const IRA::CString& key,const IRA::CString& desc) : CEntry(key,desc,T::typeSpec,true) {
		vocabulary.empty();
		vocabulary.push_back(T::defaultValue);
	}
	CHeaderEntry(const IRA::CString& key,const IRA::CString& desc,const TSequence& val) : CEntry(key,desc,T::typeSpec,true) {
		vocabulary=val;
	}
	~CHeaderEntry() {
		vocabulary.empty();
	}
	CHeaderEntry& operator=(const TSequence& val) {
		TScroll it;
		vocabulary=val;
		ret.empty();
		for (it=vocabulary.begin();it!=vocabulary.end();it++) {  // this is done to cope with the (const char *) conversion in the CString case
			ret.push_back((_ref)*it);
		}
		return *this;
	};
	const TRefSequence& getValue() const {
		return ret;
	}
private:
	TSequence vocabulary;
	TRefSequence ret;
};

class CFile {
public:
	CFile();

	~CFile();

	void reset();

	template <class T,bool MULTI> void createEntry(const IRA::CString& keyword,const IRA::CString& desc) {
		CHeaderEntry<T,MULTI> *p=new CHeaderEntry<T,MULTI>(keyword,desc);
		m_file[keyword]=(CEntry *)p;
	};

	void setKeyword(const IRA::CString& key,const _FILE_STRING_TYPE::TEntryType& val) {
		TScroll it=m_file.find(key);
		if (it!=m_file.end()) {
			if (it->second->getType()==_FILE_STRING_TYPE::typeSpec) {
				CHeaderEntry<_FILE_STRING_TYPE,false> *p=dynamic_cast<CHeaderEntry<_FILE_STRING_TYPE,false> *>(it->second);
				if (p!=NULL) {
					(*p)=val;
				}
			}
		}
	}

	void setKeyword(const IRA::CString& key,const std::list<_FILE_STRING_TYPE::TEntryType>& val) {
		TScroll it=m_file.find(key);
		if (it!=m_file.end()) {
			if (it->second->getType()==_FILE_STRING_TYPE::typeSpec) {
				CHeaderEntry<_FILE_STRING_TYPE,true> *p=dynamic_cast<CHeaderEntry<_FILE_STRING_TYPE,true> *>(it->second);
				if (p!=NULL) {
					(*p)=val;
				}
			}
		}
	}

	void setKeyword(const IRA::CString& key,const _FILE_DOUBLE_TYPE::TEntryType& val) {
		TScroll it=m_file.find(key);
		if (it!=m_file.end()) {
			if (it->second->getType()==_FILE_DOUBLE_TYPE::typeSpec) {
				CHeaderEntry<_FILE_DOUBLE_TYPE,false> *p=dynamic_cast<CHeaderEntry<_FILE_DOUBLE_TYPE,false> *>(it->second);
				if (p!=NULL) {
					(*p)=val;
				}
			}
		}
	}

	void setKeyword(const IRA::CString& key,const std::list<_FILE_DOUBLE_TYPE::TEntryType>& val) {
		TScroll it=m_file.find(key);
		if (it!=m_file.end()) {
			if (it->second->getType()==_FILE_DOUBLE_TYPE::typeSpec) {
				CHeaderEntry<_FILE_DOUBLE_TYPE,true> *p=dynamic_cast<CHeaderEntry<_FILE_DOUBLE_TYPE,true> *>(it->second);
				if (p!=NULL) {
					(*p)=val;
				}
			}
		}
	}

	void setKeyword(const IRA::CString& key,const _FILE_LONG_TYPE::TEntryType& val) {
		TScroll it=m_file.find(key);
		if (it!=m_file.end()) {
			if (it->second->getType()==_FILE_LONG_TYPE::typeSpec) {
				CHeaderEntry<_FILE_LONG_TYPE,false> *p=dynamic_cast<CHeaderEntry<_FILE_LONG_TYPE,false> *>(it->second);
				if (p!=NULL) {
					(*p)=val;
				}
			}
		}
	}

	void setKeyword(const IRA::CString& key,const std::list<_FILE_LONG_TYPE::TEntryType>& val) {
		TScroll it=m_file.find(key);
		if (it!=m_file.end()) {
			if (it->second->getType()==_FILE_LONG_TYPE::typeSpec) {
				CHeaderEntry<_FILE_LONG_TYPE,true> *p=dynamic_cast<CHeaderEntry<_FILE_LONG_TYPE,true> *>(it->second);
				if (p!=NULL) {
					(*p)=val;
				}
			}
		}
	}

	CEntry *getNextEntry() {
		CEntry *temp;
		if (m_iter!=m_file.end()) {
			temp=m_iter->second;
			m_iter++;
		}
		else {
			temp=NULL;
		}
		return temp;
	}

	void begin() { m_iter=m_file.begin(); }

private:
	typedef std::map<IRA::CString,CEntry *> TFile;
	typedef TFile::iterator TScroll;
	TFile m_file;
	TScroll m_iter;
};

};


#endif /* FILE_H_ */
