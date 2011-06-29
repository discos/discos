#ifndef _DBTABLE_H
#define _DBTABLE_H

/* **************************************************************************************************** */
/* IRA Istituto di Radioastronomia                                                                      */
/* $Id: DBTable.h,v 1.4 2010-08-10 13:54:30 a.orlati Exp $														     */
/*                                                                                                      */
/* This code is under GNU General Public Licence (GPL).                                                 */
/*                                                                                                      */
/* Who                                when            What                                              */
/* Andrea Orlati(aorlati@ira.inaf.it) 22/06/2006      Creation                                          */
/* Andrea Orlati(aorlati@ira.inaf.it) 15/04/2010      Now the table also suppport calls from maci simple clients   */

#include <vector>
#include <baciValue.h>
#include "IRA"
#include "DataField.h"
#include <expat.h>
#include <maciSimpleClient.h>

namespace IRA {

/**
 * This class implements a CDB table. Together with the <i>CDataFile</i> class it allows to access CDB data specified trough XML schemas. 
 * A typical supported XML schema sounds like this:
 * <pre>
 * <xs:complexType name="SiteType">
 *		<xs:sequence>
 *			<xs:element name="siteName" type="xs:string" />
 *			<xs:element name="latitude" type="xs:double" />
 *			<xs:element name="longitude" type="xs:double" />
 *			<xs:element name="height" type="xs:double" />
 *		</xs:sequence>
 *		<xs:attribute name="siteNumber" type="xs:integer" default="10" />
 *	</xs:complexType>
 * ...................
 * <xs:element name="observatorySite" type="SiteType" minOccurs="1" maxOccurs="unbounded" />
 * </pre>
 * This schema can be used to validate the following XML document:
 * <pre>
 *	&ltobservatorySite siteNumber="1"&gt
 *		&ltsiteName&gt"Medicina VLBI telescope"&lt/siteName&gt
 *		&ltlatitude&gt0.77703&lt/latitude&gt
 *		&ltlongitude&gt-0.20328&lt/longitude&gt
 *		&ltheight&gt28&lt/height&gt
 *	&lt/observatorySite&gt 
 *	&ltobservatorySite siteNumber="2"&gt
 *		&ltsiteName&gt"Noto VLBI telescope"&lt/siteName&gt
 *		&ltlatitude&gt0.64361&lt/latitude&gt
 *		&ltlongitude&gt-0.26161&lt/longitude&gt
 *		&ltheight&gt147&lt/height&gt
 *	&lt/observatorySite&gt  
 * </pre>
 * A "data" document like the previuos one can generate a table like:
 * 
 * <table border=2>
 * <tr><th>siteNumber</th><th>        siteName         </th><th>latitude</th><th>longitude</th><th>height</th></tr>
 * <tr><td>    1     </td><td>"Medicina VLBI telescope"</td><td> 0.77703</td><td> -0.20328</td><td>  28  </td></tr>  
 * <tr><td>    2     </td><td>  "Noto VLBI telescope"  </td><td> 0.64361</td><td> -0.26161</td><td> 147  </td></tr>    
 * </table> 
 * At present only attributes declared for the "main" element (observatorySite in the example) are supported. In that case they are handled as standard table fields. 
 * The "main" element is the name of the table and will be used to identify the table inside the file. If an element is not valued inside the XML document and no defualt
 * was provided by the schema the corresponding field will contain a Null (@sa <i>CDataField::isNull()</i>).
 * Few line of code are usually necessary to access table records and record fields. For example this code will put into log all the fields of all entries of the above table,
 * assumed that the XML document is stored into the file "OBSERV.xml":
 * <pre>
 *		IRA::CDBTable table(getContainerServices(),"observatorySite","OBSERV");
 *		err.Reset();
 *		if (!table.addField(err,"siteName",CDataField::STRING)) {
 *			//ERROR
 *		}
 *		if (!table.addField(err,"siteNumber",CDataField::LONGLONG)) {
 *			//ERROR
 *		}
 *		if (!table.addField(err,"latitude",CDataField::DOUBLE)) {
 *			//ERROR
 *		}
 *    ................
 *		if (!table.openTable(err)) {
 *			//ERRORE
 *		}
 *		table.First();
 *		for (unsigned i=0;i<table.recordCount();i++) {
 *			if (table["siteName"]->isNull()) printf("Site is Null\n");
 *			else printf("Site %s\n",(const char*)table["siteName"]->asString());
 *			if (table["latitude"]->isNull()) printf("latitude is Null\n");		
 *			else printf("latitude %f\n",table["latitude"]->asDouble());
 *			table.Next();
 *		}
 * </pre>
 * The present implementation of this class does not provide any method to modify the value of the record fields, so we can say that these CDB tables are read-only.
 * @author <a href=mailto:a.orlati@ira.inaf.it>Andrea Orlati</a>,
 * Istituto di Radioastronomia, Italia
 * <br> 
*/
class CDBTable {
public:
	/**
	 * Constructor.
	 * @param services link to the services offered by the container, it will be used to obtain reference of the DAL.
     * @param tableName the name of the table, it will be the identifier of the the table inside the CDB.
     * @param name name of the document inside which the table will be searched. If an empty string is passed (default) it will be used 
	 *             the name of the current component.
	 * @param domain the CDB domain.
	*/
	CDBTable(maci::ContainerServices *services,const CString& tableName,const CString& name="",const CString& domain="alma/");
	
	/**
	 * Constructor.
	 * @param services pointer to the maci simpleclient, it will be used to obtain reference of the DAL.
     * @param tableName the name of the table, it will be the identifier of the the table inside the CDB.
     * @param name name of the document inside which the table will be searched. 
	 * @param domain the CDB domain.
	*/
	CDBTable(maci::SimpleClient *client,const CString& tableName,const CString& name,const CString& domain="alma/");

	/**
	 * Destructor.
	*/
	~CDBTable();

	/**
	 * Call this method to open the table and enable all the operations over this dataset.
	 * @param err error information is returned through this argument.
	 * @return true if the operation succeeds. if false use the err arguments for error information.
	*/
	bool openTable(CError& err);

	/**
     * This method closes the dataset.
	*/
	void closeTable();

	/**
	 * This method is used to compose the fields list (columns) of the recordset (table). It must be used to supply a pre-description of the table
	 * before opening it. Once opened the recordset definition cannot be changed any more.
	 * @param err error information is returned through this argument.
	 * @param fieldName name of the field (column).
	 * @param type field type (@sa CDataField::TFieldType).
	 * @return true if the operation was succesful.
	*/
	bool addField(CError& err,const CString& fieldName,const CDataField::TFieldType& type);
	
	/**
	 * This method is used to know exactly how many records are present in the table.
	 * @return the numebr of records
	*/
	WORD recordCount() const;
	
	/** 
	 * Use this method to set the record pointer to the first record of the table.
	*/
	void First();

	/** 
	 * Use this method to set the record pointer to the last record of the table.
	*/
	void Last(); 
	
	/** 
	 * Use this method to set the record pointer to the next available record. If the current record
	 * is the last one, nothing happens.
	*/
	void Next();

	/** 
	 * Use this method to set the record pointer to the previous avilable record. If the current record
	 * is the first one, nothing happens.
	*/
	void Prev();
	
	/** 
	 * This operator can be used to quickly access the fields of the current record by their name. 
	 * @param fieldName the field name, i.e. the column of the current record.
	 * @return the pointer to the field, if the field name is not known a NULL is returned.
	*/
	const CDataField* operator [] (const CString& fieldName) const;
	
private:
	typedef std::vector<CDataField *> TFields;
	typedef struct {
		TFields* fieldList;
		CDataField* actualField;
		CString tableName;
		bool started;
		unsigned counter;
		IRA::CString fieldValue;
	} TParserStruct;
	maci::ContainerServices* m_containerServices;
	maci::SimpleClient *m_simpleClient;
	CString m_tableName;
	CString m_domain;
	TFields m_fieldList;
	CString m_fullName;
	bool m_opened;
	unsigned m_recCounter;
	TParserStruct m_parserStruct;
	static void startHandler(void* data,const XML_Char* el, const XML_Char** attr);
	static void endHandler(void* data,const XML_Char* el);
	static void charHandler(void* data,const XML_Char* s,int len);
	static CDataField* getField(const CString& name,const TFields *fields);
};

}

#endif
