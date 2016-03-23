/* ************************************************************************************************************* */
/* IRA Istituto di Radioastronomia                                                                               */
/*                                                                                                               */
/* This code is under GNU General Public Licence (GPL).                                                          */
/*                                                                                                               */
/* Who                                    when            What                                                      */
/* Andrea Orlati (a.orlati@ira.inaf.it)   29/04/2015      created                         */


#ifndef FILE_H_
#define FILE_H_

#include <IRA>
#include <list>
#include <CCfits>
#include <boost/preprocessor/iteration/local.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/comparison/equal.hpp>


#define _FILE_DOUBLE_TYPE CDoubleTypeEntry
#define _FILE_STRING_TYPE CStringTypeEntry
#define _FILE_LONG_TYPE CLongTypeEntry

#define _FILE_SINGLE_ENTRY 1
#define _FILE_MULTI_ENTRY 2

#define _FILE_EXPAND_HEADER_ENTRY(n) SCHEMA_HEADER_ENTRY##n
#define _FILE_CREATEHEADER(n) _FILE_LOCAL_FILE->createEntry<BOOST_PP_TUPLE_ELEM(5, 1, _FILE_EXPAND_HEADER_ENTRY(n)), \
														  !BOOST_PP_EQUAL(BOOST_PP_TUPLE_ELEM(5,2,_FILE_EXPAND_HEADER_ENTRY(n)),1) > \
														( BOOST_PP_TUPLE_ELEM(5, 3, _FILE_EXPAND_HEADER_ENTRY(n)), \
													      BOOST_PP_TUPLE_ELEM(5, 4, _FILE_EXPAND_HEADER_ENTRY(n)), \
													      BOOST_PP_TUPLE_ELEM(5, 0, _FILE_EXPAND_HEADER_ENTRY(n)) );
#define _FILE_EXPAND_TABLE(n) SCHEMA_TABLE##n
#define _FILE_CREATETABLE(n) _FILE_LOCAL_FILE->createTable ( BOOST_PP_TUPLE_ELEM(2, 0, _FILE_EXPAND_TABLE(n)), \
															BOOST_PP_TUPLE_ELEM(2, 1, _FILE_EXPAND_TABLE(n)) );


//#define _FILE_COMPARE(H,K) BOOST_PP_EQUAL(H,K)

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

class CTable {
public:
	CTable(const IRA::CString& nn,const long& id): tableId(id), name(nn), reference(NULL) { };
	virtual ~CTable() {
	};
	virtual const char* getTableName() const { return (const char *)name; };
	virtual const long& getTableId() const { return tableId; };
	virtual CCfits::Table* getTableRef() const { return reference; }
	virtual void setTableRef(CCfits::Table *const ref) { reference=ref; }
private:
	long tableId;
	IRA::CString name;
	CCfits::Table *reference;
};

class CEntry {
public:
	virtual ~CEntry() {};
	virtual const char *getKeyword() const { return (const char *)keyword; };
	virtual const char *getDescription() const { return (const char*)description; }
	virtual TFileTypeEnumeration getType() const { return typeSpec; }
	virtual bool isSimple() const { return !multi; }
	virtual long getTable() const { return tableId; }
protected:
	CEntry(): keyword(""), description("") { };
	//CEntry(const IRA::CString& key,const IRA::CString& desc): keyword(key), description(desc) { };
	CEntry(const IRA::CString& key,const IRA::CString& desc,const TFileTypeEnumeration& t,const bool& m,const long& table): keyword(key), description(desc),
			typeSpec(t), multi(m), tableId(table) { };
	IRA::CString keyword;
	IRA::CString description;
	TFileTypeEnumeration typeSpec;
	bool multi;
	long tableId;
};

template <class T,bool MULTI> class CHeaderEntry : public virtual CEntry {
public:
	typedef typename T::TEntryType _type;
	typedef typename T::TEntryReference _ref;
	CHeaderEntry() { };
	CHeaderEntry(const IRA::CString& key,const IRA::CString& desc,const long& table): CEntry(key,desc,T::typeSpec,MULTI,table), value(T::defaultValue)
	{
	};
	CHeaderEntry(const IRA::CString& key,const IRA::CString& desc,const long& table,const T& val): CEntry(key,desc,T::typeSpec,MULTI,table), value(val)
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
	CHeaderEntry(const IRA::CString& key,const IRA::CString& desc,const long& table) : CEntry(key,desc,T::typeSpec,true,table) {
		vocabulary.clear();
		vocabulary.push_back(T::defaultValue);
	}
	CHeaderEntry(const IRA::CString& key,const IRA::CString& desc,const long& table,const TSequence& val) : CEntry(key,desc,T::typeSpec,true,table) {
		vocabulary=val;
	}
	~CHeaderEntry() {
		vocabulary.clear();
	}
	CHeaderEntry& operator=(const TSequence& val) {
		TScroll it;
		vocabulary=val;
		ret.clear();
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

	//bool write(CCfits::FITS * const fits,IRA::CString& lastError);
	bool write(CCfits::FITS * const fits,IRA::CString& lastError);

	void reset();

	template <class T,bool MULTI> void createEntry(const IRA::CString& keyword,const IRA::CString& desc,const long& table) {
		CHeaderEntry<T,MULTI> *p=new CHeaderEntry<T,MULTI>(keyword,desc,table);
		m_mainHeader[keyword]=(CEntry *)p;
	};

	void createTable(const IRA::CString name,const long& id) {
		CTable* p=new CTable(name,id);
		m_tables[id]=p;
	}

	void setKeyword(const IRA::CString& key,const _FILE_STRING_TYPE::TEntryType& val) {
		TMHscroll it=m_mainHeader.find(key);
		if (it!=m_mainHeader.end()) {
			if (it->second->getType()==_FILE_STRING_TYPE::typeSpec) {
				CHeaderEntry<_FILE_STRING_TYPE,false> *p=dynamic_cast<CHeaderEntry<_FILE_STRING_TYPE,false> *>(it->second);
				if (p!=NULL) {
					(*p)=val;
				}
			}
		}
	}

	void setKeyword(const IRA::CString& key,const std::list<_FILE_STRING_TYPE::TEntryType>& val) {
		TMHscroll it=m_mainHeader.find(key);
		if (it!=m_mainHeader.end()) {
			if (it->second->getType()==_FILE_STRING_TYPE::typeSpec) {
				CHeaderEntry<_FILE_STRING_TYPE,true> *p=dynamic_cast<CHeaderEntry<_FILE_STRING_TYPE,true> *>(it->second);
				if (p!=NULL) {
					(*p)=val;
				}
			}
		}
	}

	void setKeyword(const IRA::CString& key,const _FILE_DOUBLE_TYPE::TEntryType& val) {
		TMHscroll it=m_mainHeader.find(key);
		if (it!=m_mainHeader.end()) {
			if (it->second->getType()==_FILE_DOUBLE_TYPE::typeSpec) {
				CHeaderEntry<_FILE_DOUBLE_TYPE,false> *p=dynamic_cast<CHeaderEntry<_FILE_DOUBLE_TYPE,false> *>(it->second);
				if (p!=NULL) {
					(*p)=val;
				}
			}
		}
	}

	void setKeyword(const IRA::CString& key,const std::list<_FILE_DOUBLE_TYPE::TEntryType>& val) {
		TMHscroll it=m_mainHeader.find(key);
		if (it!=m_mainHeader.end()) {
			if (it->second->getType()==_FILE_DOUBLE_TYPE::typeSpec) {
				CHeaderEntry<_FILE_DOUBLE_TYPE,true> *p=dynamic_cast<CHeaderEntry<_FILE_DOUBLE_TYPE,true> *>(it->second);
				if (p!=NULL) {
					(*p)=val;
				}
			}
		}
	}

	void setKeyword(const IRA::CString& key,const _FILE_LONG_TYPE::TEntryType& val) {
		TMHscroll it=m_mainHeader.find(key);
		if (it!=m_mainHeader.end()) {
			if (it->second->getType()==_FILE_LONG_TYPE::typeSpec) {
				CHeaderEntry<_FILE_LONG_TYPE,false> *p=dynamic_cast<CHeaderEntry<_FILE_LONG_TYPE,false> *>(it->second);
				if (p!=NULL) {
					(*p)=val;
				}
			}
		}
	}

	void setKeyword(const IRA::CString& key,const std::list<_FILE_LONG_TYPE::TEntryType>& val) {
		TMHscroll it=m_mainHeader.find(key);
		if (it!=m_mainHeader.end()) {
			if (it->second->getType()==_FILE_LONG_TYPE::typeSpec) {
				CHeaderEntry<_FILE_LONG_TYPE,true> *p=dynamic_cast<CHeaderEntry<_FILE_LONG_TYPE,true> *>(it->second);
				if (p!=NULL) {
					(*p)=val;
				}
			}
		}
	}

private:
	typedef std::map<IRA::CString,CEntry *> TMainHeader;
	typedef TMainHeader::iterator TMHscroll;
	typedef std::map<int,CTable *> TTables;
	typedef TTables::iterator TTscroll;
	TMainHeader m_mainHeader;
	TMHscroll m_mhIter;
	TTables m_tables;
	TTscroll m_tIter;

	void begin() {
		m_mhIter=m_mainHeader.begin();
		m_tIter=m_tables.begin();
	}

	CEntry *getNextMHEntry() {
		CEntry *temp;
		if (m_mhIter!=m_mainHeader.end()) {
			temp=m_mhIter->second;
			m_mhIter++;
		}
		else {
			temp=NULL;
		}
		return temp;
	}

	CTable *getNextTEntry() {
		CTable *temp;
		if (m_tIter!=m_tables.end()) {
			temp=m_tIter->second;
			m_tIter++;
		}
		else {
			temp=NULL;
		}
		return temp;
	}

};

};


#endif /* FILE_H_ */
