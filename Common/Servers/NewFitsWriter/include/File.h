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
#include <IRATools.h>
#include <list>
#include <CCfits>
#include <algorithm>
#include <boost/preprocessor/iteration/local.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/comparison/equal.hpp>
#include <boost/mpl/if.hpp>

#define _FILE_ENTRY_CACHE_SIZE 100


#define _FILE_DOUBLE_TYPE FitsWriter_private::CDoubleTypeEntry
#define _FILE_STRING_TYPE FitsWriter_private::CStringTypeEntry
#define _FILE_LONG_TYPE FitsWriter_private::CLongTypeEntry

#define _FILE_SINGLE_ENTRY 1
#define _FILE_MULTI_ENTRY 2

#define _FILE_EXPAND_HEADER_ENTRY(n) SCHEMA_HEADER_ENTRY##n
#define _FILE_CREATEHEADER(n) _FILE_LOCAL_FILE->addEntry<BOOST_PP_TUPLE_ELEM(5, 1, _FILE_EXPAND_HEADER_ENTRY(n)), \
														  !BOOST_PP_EQUAL(BOOST_PP_TUPLE_ELEM(5,2,_FILE_EXPAND_HEADER_ENTRY(n)),1) > \
														( BOOST_PP_TUPLE_ELEM(5, 3, _FILE_EXPAND_HEADER_ENTRY(n)), \
													      BOOST_PP_TUPLE_ELEM(5, 4, _FILE_EXPAND_HEADER_ENTRY(n)), \
													      BOOST_PP_TUPLE_ELEM(5, 0, _FILE_EXPAND_HEADER_ENTRY(n)) );
#define _FILE_EXPAND_TABLE(n) SCHEMA_TABLE##n
#define _FILE_CREATETABLE(n) _FILE_LOCAL_FILE->addTable ( BOOST_PP_TUPLE_ELEM(2, 0, _FILE_EXPAND_TABLE(n)), \
															BOOST_PP_TUPLE_ELEM(2, 1, _FILE_EXPAND_TABLE(n)) );


#define _FILE_EXPAND_COLUMN(n) SCHEMA_COLUMN##n
#define _FILE_CREATECOLUMNS(n) _FILE_LOCAL_FILE->addColumn<BOOST_PP_TUPLE_ELEM(5, 1, _FILE_EXPAND_COLUMN(n)), \
														   !BOOST_PP_EQUAL(BOOST_PP_TUPLE_ELEM(5,2,_FILE_EXPAND_COLUMN(n)),1) > \
														 ( BOOST_PP_TUPLE_ELEM(5, 0, _FILE_EXPAND_COLUMN(n)), \
														   BOOST_PP_TUPLE_ELEM(5, 3, _FILE_EXPAND_COLUMN(n)), \
														   BOOST_PP_TUPLE_ELEM(5, 4, _FILE_EXPAND_COLUMN(n)), \
														   BOOST_PP_TUPLE_ELEM(5, 2, _FILE_EXPAND_COLUMN(n)) );
#define _FILE_EXPAND_DATA_COLUMN(n) SCHEMA_DATA_COLUMN##n
#define _FILE_CREATEDATACOLUMNS(n) _FILE_LOCAL_FILE->addDataColumn( BOOST_PP_TUPLE_ELEM(3, 0, _FILE_EXPAND_DATA_COLUMN(n)), \
														   BOOST_PP_TUPLE_ELEM(3, 1, _FILE_EXPAND_DATA_COLUMN(n)), \
														   BOOST_PP_TUPLE_ELEM(3, 2, _FILE_EXPAND_DATA_COLUMN(n)));

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
	static char format;
	typedef double TEntryReference;
	static TFileTypeEnumeration typeSpec;
};

class CStringTypeEntry {
public:
	//static IRA::CString defaultValue;
	//typedef IRA::CString TEntryType;
	static std::string defaultValue;
	typedef std::string TEntryType;
	static char format;
	//typedef const char * TEntryReference;
	typedef std::string TEntryReference;
	static TFileTypeEnumeration typeSpec;
};

class CLongTypeEntry {
public:
	static long defaultValue;
	typedef long TEntryType;
	static char format;
	typedef long TEntryReference;
	static TFileTypeEnumeration typeSpec;
};

class CEntry {
public:
	virtual ~CEntry() {};
	virtual const char *getName() const { return (const char *)name; };
	virtual const char *getDescription() const { return (const char*)description; }
	virtual const char *getFormat() const { return (const char*)format; }
	virtual const long &getMultiplicity() const { return multiplicity; }
	virtual const long &getTable() const { return tableId; }
	virtual const TFileTypeEnumeration& getType() const { return typeSpec; }
	virtual bool isSimple() const { return !multi; }
protected:
	CEntry(): name(""), description(""), format(""), multiplicity(0), tableId(0), multi(false)  { };

	CEntry(const IRA::CString& key,const IRA::CString& desc,const IRA::CString& form,const long& mult,const long& table,
			const bool& m,const TFileTypeEnumeration& t): name(key), description(desc),format(form), multiplicity(mult),
			tableId(table), multi(m), typeSpec(t) { };
	IRA::CString name;
	IRA::CString description;
	IRA::CString format;
	long multiplicity;
	long tableId;
	bool multi;
	TFileTypeEnumeration typeSpec;
};

template <class T,bool MULTI> class CColumnEntry : public virtual CEntry {
public:
	typedef typename T::TEntryType _simple;
	typedef typename std::valarray<_simple> _multi;
	typedef typename boost::mpl::if_c<MULTI, _multi, _simple>::type _type;
	typedef const _type& _ref;
	typedef std::vector < _type > _cache;
	typedef const _cache& _cacheref;

	CColumnEntry() {
	};
	CColumnEntry(const IRA::CString& name,const IRA::CString& unit,const long& multiplicity,const long& table,const long& rowref):
		CEntry(name,unit,T::format,multiplicity,table,MULTI,T::typeSpec), m_row(rowref)
	{
		m_cache.resize(_FILE_ENTRY_CACHE_SIZE);
		initCache();
	};
	virtual ~CColumnEntry() {
		m_cache.clear();
	};
	void nextRow() {
		initCache();
	}
	void insertValue(_ref val) {
		m_cache[m_row%_FILE_ENTRY_CACHE_SIZE]=val;
	}
	_cacheref popValues() {
		m_outBuffer=m_cache;
		m_outBuffer.resize((m_row%_FILE_ENTRY_CACHE_SIZE)+1);
		return m_outBuffer;
	}

protected:
	template<class Q=T, bool M=MULTI> struct _default {
		static void value(_simple& val,const long& mul) { val=Q::defaultValue; }
	};
	template<class Q>struct _default<Q,true> {
		static void value(_multi& val,const long& mul) { val.resize(mul); val=Q::defaultValue; }
	};
	const long& m_row;
	_cache m_cache;
	_cache m_outBuffer;
	void initCache() {
		_default<T,MULTI>::value(m_cache[m_row%_FILE_ENTRY_CACHE_SIZE],getMultiplicity()); //initialize to default value
	}
};

template <class T,bool MULTI> class CHeaderEntry : public virtual CEntry {
public:
	typedef typename T::TEntryType _simple;
	typedef typename std::list<_simple> _multi;
	typedef typename boost::mpl::if_c<MULTI, _multi, _simple>::type _type;
	typedef const _type& _ref;

	CHeaderEntry(const IRA::CString& name,const IRA::CString& desc,const long& table) :
		CEntry(name,desc,T::format,1,table,MULTI,T::typeSpec)
	{
		_default<T,MULTI>::value(value);
	};

	virtual ~CHeaderEntry() {
	};

	CHeaderEntry& operator=(_ref val) {
		//_adder<T,MULTI>::value(value,val);
		value=val;
		return *this;
	};
	_ref getValue() const { return value; }

protected:
	template<class Q=T, bool M=MULTI> struct _default {
		static void value(_simple& val) { val=Q::defaultValue; }
	};
	template<class Q>struct _default<Q,true> {
		static void value(_multi& val) { val.clear(); val.push_back(Q::defaultValue); }
	};
	template<class Q=T, bool M=MULTI> struct _adder {
		static void value(_simple& val,const _simple& newVal) { val=newVal; }
	};
	template<class Q>struct _adder<Q,true> {
		static void value(_multi& val,const _simple& newVal) { val.push_back(newVal); }
	};
	_type value;
};

class CFile;

class CTable {
public:
	friend class CFile;
	CTable(const IRA::CString& nn,const long& id): m_tableId(id), m_tabName(nn), m_reference(NULL), m_row(0), m_cachePointer(0) {
		m_names.clear();
		m_units.clear();
		m_format.clear();
		m_dataBaseName.clear();
		m_dataDescr.clear();
	};
	virtual ~CTable() {
		TCscroll it;
		for (it=m_columns.begin();it!=m_columns.end();it++) {
			if (it->second) delete it->second;
		}
		TCDscroll dit;
		for (dit=m_dataColumns.begin();dit!=m_dataColumns.end();dit++) {
			if (dit->second) delete dit->second;
		}
		m_columns.clear();
		m_dataColumns.clear();
		m_names.clear();
		m_format.clear();
		m_units.clear();
		m_dataBaseName.clear();
		m_dataDescr.clear();
	};
	template <class T>bool setColumn(const IRA::CString& name,const typename T::TEntryType& val,IRA::CString& lastError) {
		CColumnEntry<T,false> *p;
		try {
			p=dynamic_cast<CColumnEntry<T,false> *>(m_columns.at(name));
		}
		catch (...) {
			lastError.Format("column %s does not exist in table",(const char *)name);
			return false;
		}
		if (!p) {
			lastError.Format("column %s does not exist in table",(const char *)name);
			return false;
		}
		else {
			p->insertValue(val);
		}
		return true;
	}
	template <class T>bool setColumn(const IRA::CString& name,const std::valarray< typename T::TEntryType>& val,IRA::CString& lastError) {
		CColumnEntry<T,true> *p;
		try {
			p=dynamic_cast<CColumnEntry<T,true> *>(m_columns.at(name));
		}
		catch (...) {
			lastError.Format("column %s does not exist in table",(const char *)name);
			return false;
		}
		if (!p) {
			lastError.Format("column %s does not exist in table",(const char *)name);
			return false;
		}
		if (val.size()!=(unsigned)p->getMultiplicity()) {
			lastError.Format("the dimension of column %s is not %ld",(const char *)name,val.size());
			return false;
		}
		else {
			p->insertValue(val);
		}
		return true;
	}

	bool setDataColumn(const IRA::CString& name,const std::valarray<double>& val,IRA::CString& lastError) {
		CColumnEntry<_FILE_DOUBLE_TYPE,true> *p;
		try {
			p=dynamic_cast<CColumnEntry<_FILE_DOUBLE_TYPE,true> *>(m_dataColumns.at(name));
		}
		catch (...) {
			lastError.Format("column %s does not exist in table",(const char *)name);
			return false;
		}
		if (!p) {
			lastError.Format("column %s does not exist in table",(const char *)name);
			return false;
		}
		if (val.size()!=(unsigned)p->getMultiplicity()) {
			lastError.Format("the dimension of column %s is not %ld",(const char *)name,val.size());
			return false;
		}
		else {
			p->insertValue(val);
		}
		return true;
	}
	bool setDataColumn(const IRA::CString& name,const double& val,IRA::CString& lastError) {
		CColumnEntry<_FILE_DOUBLE_TYPE,false> *p;
		try {
			p=dynamic_cast<CColumnEntry<_FILE_DOUBLE_TYPE,false> *>(m_dataColumns.at(name));
		}
		catch (...) {
			lastError.Format("column %s does not exist in table",(const char *)name);
			return false;
		}
		if (!p) {
			lastError.Format("column %s does not exist in table",(const char *)name);
			return false;
		}
		else {
			p->insertValue(val);
		}
		return true;
	}

	bool defineDataColumn(const IRA::CString& baseName,const std::vector<long>& dimension,IRA::CString& lastError) {
		std::vector<IRA::CString>::iterator it;
		IRA::CString str;
		//cout << "cerco la base column name :" << (const char *)baseName << endl;
		//cout << "nome al primo elemento :" << m_dataBaseName.size() << endl;
		it=std::find(m_dataBaseName.begin(),m_dataBaseName.end(),baseName);
		if (it!=m_dataBaseName.end()) {
			unsigned size=dimension.size();
			//cout << "trovato" << "size is " << size << endl;
			for (unsigned k=0;k<size;k++) {
				str.Format("%s%d",(const char *)baseName,k);
				//printf("nome data column, descr: %s %s\n",(const char *)str,(const char *)m_dataDescr[it-m_dataBaseName.begin()]);
				if (dimension[k]>1) {
					m_dataColumns[str]=(CEntry *)new CColumnEntry<_FILE_DOUBLE_TYPE,true>(str,m_dataDescr[it-m_dataBaseName.begin()],
						dimension[k],m_tableId,m_cachePointer);
				}
				else {
					m_dataColumns[str]=(CEntry *)new CColumnEntry<_FILE_DOUBLE_TYPE,false>(str,m_dataDescr[it-m_dataBaseName.begin()],
						dimension[k],m_tableId,m_cachePointer);
				}
			}
		}
		else {
			lastError.Format("column %s does not exists",(const char *)baseName);
			return false;
		}
		return true;
	}

	bool nextRow(IRA::CString& errMsg);
protected:
	typedef std::map<IRA::CString,CEntry *> TColumns;
	typedef TColumns::iterator TCscroll;
	typedef std::map<IRA::CString,CEntry *> TDataColumns;
	typedef TDataColumns::iterator TCDscroll;
	long m_tableId;
	IRA::CString m_tabName;
	CCfits::Table *m_reference;
	TColumns m_columns;
	TDataColumns m_dataColumns;
	long m_row;
	long m_cachePointer;
	std::vector<string> m_names;
	std::vector<string> m_units;
	std::vector<string> m_format;
	std::vector<IRA::CString> m_dataBaseName;
	std::vector<IRA::CString> m_dataDescr;
	virtual void setTableRef(CCfits::Table *const ref) { m_reference=ref; }

	void flushAll();

	template <class T,int MULTI>void addColumn(const IRA::CString& name,const IRA::CString& unit,const long& mul) {
		std::stringstream form;
		CColumnEntry<T,MULTI> *entry;
		entry=new CColumnEntry<T,MULTI>(name,unit,mul,m_tableId,m_cachePointer);
		m_columns[name]=entry;
		m_names.push_back((const char *)name);
		m_units.push_back((const char *)unit);
		if (entry->isSimple()) {
			form << (const char *)entry->getFormat();
		}
		else {
			form << entry->getMultiplicity() << (const char *)entry->getFormat();
		}
		m_format.push_back(form.str());
	}

	void allocateDataColumn(const IRA::CString& baseName,const IRA::CString& desc) {
		m_dataBaseName.push_back(baseName);
		m_dataDescr.push_back(desc);
		//printf("data columns allocated: %s %s \n",(const char *)baseName,(const char *)desc);
		//cout << "rileggo :" << (const char *)m_dataBaseName[0] << endl;
		//cout << "size :" << m_dataBaseName.size() << endl;

	}

	bool write(IRA::CString& errMsg);

	void getColumnDefinition(std::vector<string>& name,std::vector<string>& form,std::vector<string>& unit) {
		//std::vector<string>::iterator it;
		/*for(unsigned i=0;i<m_dataBaseName.size();i++) {
			string name=m_dataBaseName[i];
			unsigned size=m_dataSize[name].size();
		}*/
		std::stringstream format;
		TCDscroll dit;
		name=m_names;
		unit=m_units;
		form=m_format;
		for (dit=m_dataColumns.begin();dit!=m_dataColumns.end();dit++) {
		//for (dit=m_dataColumns.begin();dit!=m_dataColumns.end();dit++) {
			CEntry *p=dit->second;
			if (p->isSimple()) {
				format << (const char *)p->getFormat();
			}
			else {
				format << p->getMultiplicity() << (const char *)p->getFormat();
			}
			form.push_back(format.str());
			format.str("");
			name.push_back(p->getName());
			unit.push_back(p->getDescription());
		}

	}
	virtual const char* getTableName() const { return (const char *)m_tabName; };
	virtual const long& getTableId() const { return m_tableId; };
	virtual CCfits::Table* getTableRef() const { return m_reference; }
};


class CFile {
public:
	CFile();

	~CFile();

	/**
	 * This method creates the file and build all the tables inside it.
	 */
	bool open(IRA::CString& lastError);

	/**
	 * This method writes all the keywords of the file
	 */
	bool create(IRA::CString& lastError);

	/**
	 * Retrieve the reference to a binary table providing the name of the table
	 * @return NULL if the table is not present
	 */
	CTable *getTable(const IRA::CString& name,IRA::CString& lastError);

	/**
	 * Retrieve the reference to a binary table providing the identifier of the table
	 * @return NULL if the table is not present
	 */
	CTable *getTable(const long& id,IRA::CString& lastError);

	inline void setFileName(const IRA::CString& name) { m_fileName=name; }

	inline IRA::CString getFileName() const { return m_fileName; }

	template <class T,bool MULTI> void addEntry(const IRA::CString& name,const IRA::CString& desc,const long& table) {
		CHeaderEntry<T,MULTI> *p=new CHeaderEntry<T,MULTI>(name,desc,table);
		m_mainHeader[name]=(CEntry *)p;
	};

	void addTable(const IRA::CString name,const long& id) {
		CTable* p=new CTable(name,id);
		m_tables[id]=p;
	}

	template <class T,bool MULTI> void addColumn(const long& tabId,const IRA::CString& colName,const IRA::CString& unit,const long& mult) {
		IRA::CString err;
		CTable *tab=getTable(tabId,err);
		if (tab) {
			tab->addColumn<T,MULTI>(colName,unit,mult);
		}
	}

	void addDataColumn(const long& tableId,const IRA::CString& baseName,const IRA::CString& desc) {
		IRA::CString err;
		CTable *tab=getTable(tableId,err);
		if (tab) {
			tab->allocateDataColumn(baseName,desc);
		}
	}

	template <class T>bool setKeyword(const IRA::CString& name,const typename T::TEntryType& val,IRA::CString& lastError) {
		CHeaderEntry<T,false> *p;
		try {
			p=dynamic_cast<CHeaderEntry<T,false> *>(m_mainHeader.at(name));
		}
		catch (...) {
			lastError.Format("keyword %s does not exist in file",(const char *)name);
			return false;
		}
		if (!p) {
			lastError.Format("keyword %s does not exist in file",(const char *)name);
			return false;
		}
		else {
			(*p)=val;
		}
		return true;
	}

	template <class T>bool setKeyword(const IRA::CString& name,const std::list<typename T::TEntryType>& val,IRA::CString& lastError) {
		CHeaderEntry<T,true> *p;
		try {
			p=dynamic_cast<CHeaderEntry<T,true> *>(m_mainHeader.at(name));
		}
		catch (...) {
			lastError.Format("keyword %s does not exist in file",(const char *)name);
			return false;
		}
		if (!p) {
			lastError.Format("keyword %s does not exist in file",(const char *)name);
			return false;
		}
		else {
			(*p)=val;
		}
		return true;
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
	CCfits::FITS * m_fits;
	IRA::CString m_fileName;

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
