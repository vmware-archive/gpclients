/****************************************
 *
 * modifications Lorne Sunley July 2004
 *
 * change to return SQL datatypes in new field nType in SQPDATATYPE
 *
 ****************************************/

#include "sqp.h"
#include <sqlext.h>

void sqpStoreDataType( char *pszType, int nPrecision, int nScale )
{
    HSQPDATATYPE    hDataType;
	
#ifdef SQPDEBUG
	printf( "[SQP][%s][%d] BEGIN: %s ( %d, %d )\n", __FILE__, __LINE__, pszType, nPrecision, nScale );
#endif

    if ( g_hDataType )
    {
        printf( "[SQP][%s][%d] END: Warning; Possible memory leak.\n", __FILE__, __LINE__ );
        return;
    }

	hDataType = (HSQPDATATYPE)malloc( sizeof(SQPDATATYPE) );
	hDataType->pszType	    = (char*)strdup( pszType );
    hDataType->nPrecision   = nPrecision;
	hDataType->nScale       = nScale;
    hDataType->nType = sqpStringTypeToSQLTYPE(pszType);

    g_hDataType = hDataType;

#ifdef SQPDEBUG
	printf( "[SQP][%s][%d] END:\n", __FILE__, __LINE__ );
#endif

}


/* dumb little routine to return SQL datatype from sqp parsed datatype */

short sqpStringTypeToSQLTYPE (char * pszType)

{


	if (stricmp( pszType, "CHAR") == 0)
		return SQL_CHAR;
	if (stricmp( pszType, "VARCHAR") == 0)
		return SQL_VARCHAR;
	if (stricmp( pszType, "LONG VARCHAR") == 0)
		return SQL_LONGVARCHAR;
	if (stricmp( pszType, "NUMERIC") == 0)
		return SQL_NUMERIC;
	if (stricmp( pszType, "DECIMAL") == 0)
		return SQL_DECIMAL;
	if (stricmp( pszType, "SMALLINT") == 0)
		return SQL_SMALLINT;
	if (stricmp( pszType, "INTEGER") == 0)
		return SQL_INTEGER;
	if (stricmp( pszType, "REAL") == 0)
		return SQL_REAL;
	if (stricmp( pszType, "FLOAT") == 0)
		return SQL_DOUBLE;
	if (stricmp( pszType, "DOUBLE PRECISION") == 0)
		return SQL_DOUBLE;
	if (stricmp( pszType, "BIT") == 0)
		return SQL_BIT;
	if (stricmp( pszType, "TINYINT") == 0)
		return SQL_TINYINT;
	if (stricmp( pszType, "BIGINT") == 0)
		return SQL_BIGINT;
	if (stricmp( pszType, "BINARY") == 0)
		return SQL_BINARY;
	if (stricmp( pszType, "VARBINARY") == 0)
		return SQL_VARBINARY;
	if (stricmp( pszType, "LONG VARBINARY") == 0)
		return SQL_LONGVARBINARY;
	if (stricmp( pszType, "DATE") == 0)
		return SQL_DATE;
	if (stricmp( pszType, "TIME") == 0)
		return SQL_TIME;
	if (stricmp( pszType, "TIMESTAMP") == 0)
		return SQL_TIMESTAMP;
	return SQL_CHAR;


}
