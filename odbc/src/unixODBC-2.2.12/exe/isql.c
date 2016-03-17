
/**************************************************
 * isql
 *
 **************************************************
 * This code was created by Peter Harvey @ CodeByDesign.
 * Released under GPL 18.FEB.99
 *
 * Contributions from...
 * -----------------------------------------------
 * Peter Harvey		- pharvey@codebydesign.com
 **************************************************/

#include "isql.h"
#ifdef HAVE_READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif

#ifdef HAVE_SETLOCALE
#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif 
#endif

int 	bVerbose					= 0;
int     nUserWidth                = 0;
SQLHENV	hEnv						= 0;
SQLHDBC	hDbc						= 0;

int main( int argc, char *argv[] )
{
	int 	nArg, count;
	int 	bHTMLTable					= 0;
	int		bBatch						= 0;
	int 	cDelimiter					= 0;
    int     bColumnNames                = 0;
	int rc = 0;
	char	*szDSN;
	char	*szUID;
	char	*szPWD;
	char	*szSQL;
	char    *pEscapeChar;
    int     buffer_size = 9000;

	szDSN = NULL;
	szUID = NULL;
	szPWD = NULL;

	if ( argc < 2 )
	{
		fprintf( stderr, szSyntax );
		exit( 1 );
	}

#ifdef HAVE_SETLOCALE
    /*
     * Default locale
     */
    setlocale( LC_ALL, "" );
#endif

	/****************************
	 * PARSE ARGS
	 ***************************/
	for ( nArg = 1, count = 1 ; nArg < argc; nArg++ )
	{
		if ( argv[nArg][0] == '-' )
		{
			/* Options */
			switch ( argv[nArg][1] )
			{
			case 'd':
				cDelimiter = argv[nArg][2];
				break;
            case 'm':
				nUserWidth = atoi( &(argv[nArg][2]) );
				break;
            case 's':
				buffer_size = atoi( &(argv[nArg][2]) );
				break;
			case 'w':
				bHTMLTable = 1;
				break;
			case 'b':
				bBatch = 1;
				break;
			case 'c':
				bColumnNames = 1;
				break;
			case 'v':
				bVerbose = 1;
				break;
			case '-':
				printf( "unixODBC " VERSION "\n" );
				exit(0);
#ifdef HAVE_STRTOL
            case 'x':
				cDelimiter = strtol( argv[nArg]+2, NULL, 0 );
                break;
#endif
#ifdef HAVE_SETLOCALE
            case 'l':
                if ( !setlocale( LC_ALL, argv[nArg]+2 ))
                {
                    fprintf( stderr, "isql: can't set locale to '%s'\n", argv[nArg]+2 );
                    exit ( -1 );
                }
                break;
#endif

			default:
				fprintf( stderr, szSyntax );
				exit( 1 );
			}
			continue;
		}
		else if ( count == 1 )
			szDSN = argv[nArg];
		else if ( count == 2 )
			szUID = argv[nArg];
		else if ( count == 3 )
			szPWD = argv[nArg];
		count++;
	}

    szSQL = calloc( 1, buffer_size + 1 );

	/****************************
	 * CONNECT
	 ***************************/
	
	if ( !OpenDatabase( &hEnv, &hDbc, szDSN, szUID, szPWD ) )
		exit( 1 );

	/****************************
	 * EXECUTE
	 ***************************/
	if ( !bBatch )
	{
		printf( "+---------------------------------------+\n" );
		printf( "| Connected!                            |\n" );
		printf( "|                                       |\n" );
		printf( "| sql-statement                         |\n" );
		printf( "| help [tablename]                      |\n" );
		printf( "| quit                                  |\n" );
		printf( "|                                       |\n" );
		printf( "+---------------------------------------+\n" );
	}
	do
	{
		if ( !bBatch )
#ifndef HAVE_READLINE
			printf( "SQL> " );
#else
		{
			char *line;
            int malloced;

			line=readline("SQL> ");
            if ( !line )        /* EOF - ctrl D */
            {
                malloced = 1;
                line = strdup( "quit" );
            }
            else
            {
                malloced = 0;
            }
			strncpy(szSQL, line, buffer_size );
			add_history(line);
            if ( malloced )
            {
                free(line);
            }
		} else 
#endif
        {
			char *line;
            int malloced;

		    line = fgets( szSQL, buffer_size, stdin );
            if ( !line )        /* EOF - ctrl D */
            {
                malloced = 1;
                line = strdup( "quit" );
            }
            else
            {
                malloced = 0;
            }
			strncpy(szSQL, line, buffer_size );
            if ( malloced )
            {
                free(line);
            }
        }

		/* strip away escape chars */
		while ( (pEscapeChar=(char*)strchr(szSQL, '\n')) != NULL || (pEscapeChar=(char*)strchr(szSQL, '\r')) != NULL )
			*pEscapeChar = ' ';

		if ( szSQL[1] != '\0' )
		{
			if ( strncmp( szSQL, "quit", 4 ) == 0 )
				szSQL[1] = '\0';
			else if ( strncmp( szSQL, "help", 4 ) == 0 )
				ExecuteHelp( hDbc, szSQL, cDelimiter, bColumnNames, bHTMLTable );
			else if (memcmp(szSQL, "--", 2) != 0) 
				ExecuteSQL( hDbc, szSQL, cDelimiter, bColumnNames, bHTMLTable );
		}

	} while ( szSQL[1] != '\0' );

	/****************************
	 * DISCONNECT
	 ***************************/
	CloseDatabase( hEnv, hDbc );

	exit( 0 );
}

/****************************
 * OptimalDisplayWidth
 ***************************/
static SQLUINTEGER
OptimalDisplayWidth( SQLHSTMT hStmt, SQLINTEGER nCol, int nUserWidth )
{
	SQLUINTEGER	nLabelWidth                     = 10;
	SQLULEN	nDataWidth                      = 10;
	SQLUINTEGER	nOptimalDisplayWidth            = 10;
	SQLCHAR     szColumnName[MAX_DATA_WIDTH+1];	

	*szColumnName = '\0';

    SQLColAttribute( hStmt, nCol, SQL_DESC_DISPLAY_SIZE, NULL, 0, NULL, &nDataWidth );
    SQLColAttribute( hStmt, nCol, SQL_DESC_LABEL, szColumnName, sizeof(szColumnName), NULL, NULL );
    nLabelWidth = strlen((char*) szColumnName );

    nOptimalDisplayWidth = max( nLabelWidth, nDataWidth );

    if ( nUserWidth > 0 )
        nOptimalDisplayWidth = min( nOptimalDisplayWidth, nUserWidth );

    if ( nOptimalDisplayWidth > MAX_DATA_WIDTH ) 
        nOptimalDisplayWidth = MAX_DATA_WIDTH;

    return nOptimalDisplayWidth;
}

/****************************
 * OpenDatabase - do everything we have to do to get a viable connection to szDSN
 ***************************/
static int
OpenDatabase( SQLHENV *phEnv, SQLHDBC *phDbc, char *szDSN, char *szUID, char *szPWD )
{
	if ( SQLAllocEnv( phEnv ) != SQL_SUCCESS )
	{
		fprintf( stderr, "[ISQL]ERROR: Could not SQLAllocEnv\n" );
		return 0;
	}

	if ( SQLAllocConnect( *phEnv, phDbc ) != SQL_SUCCESS )
	{
        if ( bVerbose ) DumpODBCLog( hEnv, 0, 0 );
		fprintf( stderr, "[ISQL]ERROR: Could not SQLAllocConnect\n" );
		SQLFreeEnv( *phEnv );
		return 0;
	}

    if ( !SQL_SUCCEEDED( SQLConnect( *phDbc, (SQLCHAR*)szDSN, SQL_NTS, (SQLCHAR*)szUID, SQL_NTS, (SQLCHAR*)szPWD, SQL_NTS )))
	{
        if ( bVerbose ) DumpODBCLog( hEnv, hDbc, 0 );
		fprintf( stderr, "[ISQL]ERROR: Could not SQLConnect\n" );
		SQLFreeConnect( *phDbc );
		SQLFreeEnv( *phEnv );
		return 0;
	}

	return 1;
}

/****************************
 * ExecuteSQL - create a statement, execute the SQL, and get rid of the statement
 *            - show results as per request; bHTMLTable has precedence over other options
 ***************************/
static int
ExecuteSQL( SQLHDBC hDbc, char *szSQL, char cDelimiter, int bColumnNames, int bHTMLTable )
{
	SQLHSTMT		hStmt;
    SQLSMALLINT     cols;
    SQLLEN          nRows                   = 0;
    SQLINTEGER      ret;
	SQLCHAR			*szSepLine;

	szSepLine = calloc(1, 32001);

	/****************************
	 * EXECUTE SQL
	 ***************************/
	if ( SQLAllocStmt( hDbc, &hStmt ) != SQL_SUCCESS )
	{
        if ( bVerbose ) DumpODBCLog( hEnv, hDbc, 0 );
		fprintf( stderr, "[ISQL]ERROR: Could not SQLAllocStmt\n" );
		free(szSepLine);
		return 0;
	}

	if ( SQLPrepare( hStmt, (SQLCHAR*)szSQL, SQL_NTS ) != SQL_SUCCESS )
	{
        if ( bVerbose ) DumpODBCLog( hEnv, hDbc, hStmt );
		fprintf( stderr, "[ISQL]ERROR: Could not SQLPrepare\n" );
		SQLFreeStmt( hStmt, SQL_DROP );
		free(szSepLine);
		return 0;
	}

    ret =  SQLExecute( hStmt );

    if ( ret == SQL_NO_DATA )
    {
		fprintf( stderr, "[ISQL]INFO: SQLExecute returned SQL_NO_DATA\n" );
    }
    else if ( ret == SQL_SUCCESS_WITH_INFO )
    {
        if ( bVerbose ) DumpODBCLog( hEnv, hDbc, hStmt );
		fprintf( stderr, "[ISQL]INFO: SQLExecute returned SQL_SUCCESS_WITH_INFO\n" );
    }
    else if ( ret != SQL_SUCCESS )
	{
        if ( bVerbose ) DumpODBCLog( hEnv, hDbc, hStmt );
		fprintf( stderr, "[ISQL]ERROR: Could not SQLExecute\n" );
		SQLFreeStmt( hStmt, SQL_DROP );
		free(szSepLine);
		return 0;
	}

    /*
     * Loop while SQLMoreResults returns success
     */

    do
    {
        strcpy ( szSepLine, "" ) ;

        /*
         * check to see if it has generated a result set
         */
    
        if ( SQLNumResultCols( hStmt, &cols ) != SQL_SUCCESS )
        {
            if ( bVerbose ) DumpODBCLog( hEnv, hDbc, hStmt );
		    fprintf( stderr, "[ISQL]ERROR: Could not SQLNumResultCols\n" );
		    SQLFreeStmt( hStmt, SQL_DROP );
	   	    free(szSepLine);
		    return 0;
        }
    
        if ( cols > 0 )
        {
            /****************************
             * WRITE HEADER
             ***************************/
            if ( bHTMLTable )
                WriteHeaderHTMLTable( hStmt );
            else if ( cDelimiter == 0 )
                WriteHeaderNormal( hStmt, szSepLine );
            else if ( cDelimiter && bColumnNames )
                WriteHeaderDelimited( hStmt, cDelimiter );
    
            /****************************
             * WRITE BODY
             ***************************/
            if ( bHTMLTable )
                WriteBodyHTMLTable( hStmt );
            else if ( cDelimiter == 0 )
                nRows = WriteBodyNormal( hStmt );
            else
                WriteBodyDelimited( hStmt, cDelimiter );
        }
    
        /****************************
         * WRITE FOOTER
         ***************************/
        if ( bHTMLTable )
            WriteFooterHTMLTable( hStmt );
        else if ( cDelimiter == 0 )
            WriteFooterNormal( hStmt, szSepLine, nRows );
    }
    while ( SQL_SUCCEEDED( SQLMoreResults( hStmt )));

	/****************************
	 * CLEANUP
	 ***************************/
	SQLFreeStmt( hStmt, SQL_DROP );
	free(szSepLine);

	return 1;
}

/****************************
 * ExecuteHelp - create a statement, execute the SQL, and get rid of the statement
 *             - show results as per request; bHTMLTable has precedence over other options
 ***************************/
static int
ExecuteHelp( SQLHDBC hDbc, char *szSQL, char cDelimiter, int bColumnNames, int bHTMLTable )
{
    SQLHSTMT hStmt;
    SQLLEN nRows = 0;
    SQLRETURN nReturn;
    SQLCHAR *szSepLine;
    char *args[10];
    int n_args;
    
    if (!(szSepLine = calloc(1, 32001))) {
        fprintf(stderr, "[ISQL]ERROR: Failed to allocate line");
        return 0;
    }
        
    /****************************
     * EXECUTE SQL
     ***************************/
    if ( SQLAllocStmt( hDbc, &hStmt ) != SQL_SUCCESS )
    {
        if ( bVerbose ) DumpODBCLog( hEnv, hDbc, 0 );
        fprintf( stderr, "[ISQL]ERROR: Could not SQLAllocStmt\n" );
        free(szSepLine);
        return 0;
    }
    n_args = get_args(szSQL, &args[0], sizeof(args) / sizeof(args[0]));

    if (n_args == 2 ) {
        if (strcmp(args[1], "help") == 0) {
            output_help();
            free(szSepLine);
            return 0;
        }
        
        /* COLUMNS */
        nReturn = SQLColumns( hStmt, NULL, 0, NULL, 0, args[1], SQL_NTS, NULL, 0 );
        if ( (nReturn != SQL_SUCCESS) && (nReturn != SQL_SUCCESS_WITH_INFO) )
        {
            if ( bVerbose ) DumpODBCLog( hEnv, hDbc, hStmt );
            fprintf( stderr, "[ISQL]ERROR: Could not SQLColumns\n" );
            SQLFreeStmt( hStmt, SQL_DROP );
            free(szSepLine);
            return 0;
        }
    } else {
        SQLCHAR *catalog = NULL;
        SQLCHAR *schema = NULL;
        SQLCHAR *table = NULL;
        SQLCHAR *type = NULL;

        if (n_args > 2) {
            catalog = args[1];
            schema = args[2];
            table = args[3];
            type = args[4];
        }
        
        /* TABLES */
        nReturn = SQLTables( hStmt, catalog, SQL_NTS, schema, SQL_NTS,
                             table, SQL_NTS, type, SQL_NTS );
        if ( (nReturn != SQL_SUCCESS) && (nReturn != SQL_SUCCESS_WITH_INFO) )
        {
            if ( bVerbose ) DumpODBCLog( hEnv, hDbc, hStmt );
            fprintf( stderr, "[ISQL]ERROR: Could not SQLTables\n" );
            SQLFreeStmt( hStmt, SQL_DROP );
            free(szSepLine);
            free_args(args, sizeof(args) / sizeof(args[0]));
            return 0;
        }
    }

    /****************************
     * WRITE HEADER
     ***************************/
    if ( bHTMLTable )
        WriteHeaderHTMLTable( hStmt );
    else if ( cDelimiter == 0 )
        WriteHeaderNormal( hStmt, szSepLine );
    else if ( cDelimiter && bColumnNames )
        WriteHeaderDelimited( hStmt, cDelimiter );

    /****************************
     * WRITE BODY
     ***************************/
    if ( bHTMLTable )
        WriteBodyHTMLTable( hStmt );
    else if ( cDelimiter == 0 )
        nRows = WriteBodyNormal( hStmt );
    else
        WriteBodyDelimited( hStmt, cDelimiter );

    /****************************
     * WRITE FOOTER
     ***************************/
    if ( bHTMLTable )
        WriteFooterHTMLTable( hStmt );
    else if ( cDelimiter == 0 )
        WriteFooterNormal( hStmt, szSepLine, nRows );

    /****************************
     * CLEANUP
     ***************************/
    SQLFreeStmt( hStmt, SQL_DROP );
    free(szSepLine);
    free_args(args, sizeof(args) / sizeof(args[0]));
    return 1;
}


/****************************
 * CloseDatabase - cleanup in prep for exiting the program
 ***************************/
static int CloseDatabase( SQLHENV hEnv, SQLHDBC hDbc )
{
	SQLDisconnect( hDbc );
	SQLFreeConnect( hDbc );
	SQLFreeEnv( hEnv );

	return 1;
}


/****************************
 * WRITE HTML
 ***************************/
static void WriteHeaderHTMLTable( SQLHSTMT hStmt )
{
    SQLINTEGER    	nCol            				= 0;
	SQLSMALLINT		nColumns						= 0;
	SQLCHAR			szColumnName[MAX_DATA_WIDTH+1];	

	*szColumnName = '\0';

	printf( "<table BORDER>\n" );
	printf( "<tr BGCOLOR=#000099>\n" );

    if ( SQLNumResultCols( hStmt, &nColumns ) != SQL_SUCCESS )
        nColumns = -1;

	for( nCol = 1; nCol <= nColumns; nCol++ )
	{
		SQLColAttribute( hStmt, nCol, SQL_DESC_LABEL, szColumnName, sizeof(szColumnName), NULL, NULL );
		printf( "<td>\n" );
		printf( "<font face=Arial,Helvetica><font color=#FFFFFF>\n" );
		printf( "%s\n", szColumnName );
		printf( "</font></font>\n" );
		printf( "</td>\n" );
	}
	printf( "</tr>\n" );
}

static void WriteBodyHTMLTable( SQLHSTMT hStmt )
{
    SQLINTEGER    	nCol            				= 0;
	SQLSMALLINT		nColumns						= 0;
	SQLLEN		    nIndicator						= 0;
	SQLCHAR			szColumnValue[MAX_DATA_WIDTH+1];
	SQLRETURN		nReturn							= 0;
    SQLRETURN       ret;

	*szColumnValue = '\0';

    if ( SQLNumResultCols( hStmt, &nColumns ) != SQL_SUCCESS )
        nColumns = -1;

    while( (ret = SQLFetch( hStmt )) == SQL_SUCCESS ) /* ROWS */
    {
		printf( "<tr>\n" );
		
        for( nCol = 1; nCol <= nColumns; nCol++ ) /* COLS */
        {
			printf( "<td>\n" );
			printf( "<font face=Arial,Helvetica>\n" );

            nReturn = SQLGetData( hStmt, nCol, SQL_C_CHAR, (SQLPOINTER)szColumnValue, sizeof(szColumnValue), &nIndicator );
            if ( nReturn == SQL_SUCCESS && nIndicator != SQL_NULL_DATA )
            {
                fputs((char*) szColumnValue, stdout );
            }
	    else if ( nReturn == SQL_ERROR ) {
		ret = SQL_ERROR;
		break;
	    }
            else
                printf( "%s\n", "" );

			printf( "</font>\n" );
			printf( "</td>\n" );
        }
	if (ret != SQL_SUCCESS)
	    break;
		printf( "</tr>\n" );
    }
}

static void WriteFooterHTMLTable( SQLHSTMT hStmt )
{
	printf( "</table>\n" );
}

/****************************
 * WRITE DELIMITED
 * - this output can be used by the ODBC Text File driver
 * - last column no longer has a delimit char (it is implicit)...
 *   this is consistent with odbctxt
 ***************************/
static void WriteHeaderDelimited( SQLHSTMT hStmt, char cDelimiter )
{
    SQLINTEGER    	nCol            				= 0;
	SQLSMALLINT		nColumns						= 0;
	SQLCHAR			szColumnName[MAX_DATA_WIDTH+1];	

	*szColumnName = '\0';

    if ( SQLNumResultCols( hStmt, &nColumns ) != SQL_SUCCESS )
        nColumns = -1;

	for( nCol = 1; nCol <= nColumns; nCol++ )
	{
		SQLColAttribute( hStmt, nCol, SQL_DESC_LABEL, szColumnName, sizeof(szColumnName), NULL, NULL );
        fputs((char*) szColumnName, stdout );
        if ( nCol < nColumns )
            putchar( cDelimiter );
	}
    putchar( '\n' );
}

static void WriteBodyDelimited( SQLHSTMT hStmt, char cDelimiter )
{
    SQLINTEGER    	nCol            				= 0;
	SQLSMALLINT		nColumns						= 0;
	SQLLEN		    nIndicator						= 0;
	SQLCHAR			szColumnValue[MAX_DATA_WIDTH+1];
	SQLRETURN		nReturn							= 0;
    SQLRETURN       ret;

	*szColumnValue = '\0';

    if ( SQLNumResultCols( hStmt, &nColumns ) != SQL_SUCCESS )
        nColumns = -1;

	/* ROWS */
    while(( ret = SQLFetch( hStmt )) == SQL_SUCCESS )
    {
		/* COLS */
        for( nCol = 1; nCol <= nColumns; nCol++ )
        {
            nReturn = SQLGetData( hStmt, nCol, SQL_C_CHAR, (SQLPOINTER)szColumnValue, sizeof(szColumnValue), &nIndicator );
            if ( nReturn == SQL_SUCCESS && nIndicator != SQL_NULL_DATA )
            {
                fputs((char*) szColumnValue, stdout );
                if ( nCol < nColumns )
                    putchar( cDelimiter );
            }
	        else if ( nReturn == SQL_ERROR ) 
            {
		        ret = SQL_ERROR;
		        break;
	        }
            else
            {
                if ( nCol < nColumns )
                    putchar( cDelimiter );
            }
        }
	    if (ret != SQL_SUCCESS)
	        break;
        printf( "\n" );
    }
    if ( ret == SQL_ERROR )
    {  
        if ( bVerbose ) DumpODBCLog( 0, 0, hStmt );
    }
}

/****************************
 * WRITE NORMAL
 ***************************/
static void WriteHeaderNormal( SQLHSTMT hStmt, SQLCHAR *szSepLine )
{
    SQLINTEGER    	nCol            				= 0;
	SQLSMALLINT		nColumns						= 0;
	SQLCHAR			szColumn[MAX_DATA_WIDTH+20];	
	SQLCHAR			szColumnName[MAX_DATA_WIDTH+1];	
	/*SQLCHAR			szHdrLine[32001]				= "";	*/
	SQLCHAR			*szHdrLine;
	SQLUINTEGER		nOptimalDisplayWidth            = 10;

	szHdrLine = calloc(1, 32001);

	*szColumn = '\0';
	*szColumnName = '\0';

    if ( SQLNumResultCols( hStmt, &nColumns ) != SQL_SUCCESS )
        nColumns = -1;

	for( nCol = 1; nCol <= nColumns; nCol++ )
	{
            int sret;
            
        nOptimalDisplayWidth = OptimalDisplayWidth( hStmt, nCol, nUserWidth );
        SQLColAttribute( hStmt, nCol, SQL_DESC_LABEL, szColumnName, sizeof(szColumnName), NULL, NULL );

		/* SEP */
        memset( szColumn, '\0', sizeof(szColumn) );
		memset( szColumn, '-', nOptimalDisplayWidth + 1 );
		strcat((char*) szSepLine, "+" );
		strcat((char*) szSepLine,(char*) szColumn );

		/* HDR */
		sret = sprintf( (char*)szColumn, "| %-*.*s",
                                nOptimalDisplayWidth, nOptimalDisplayWidth, szColumnName );
                if (sret < 0)
                    sprintf((char *)szColumn, "| %-*.*s",
                            nOptimalDisplayWidth, nOptimalDisplayWidth, "**ERROR**");
        strcat( (char*)szHdrLine,(char*) szColumn );
	}
	strcat((char*) szSepLine, "+\n" );
	strcat((char*) szHdrLine, "|\n" );
	
	printf((char*) szSepLine );
	printf((char*) szHdrLine );
	printf((char*) szSepLine );
	free(szHdrLine);
}

static SQLLEN WriteBodyNormal( SQLHSTMT hStmt )
{
    SQLINTEGER    	nCol            				= 0;
	SQLSMALLINT		nColumns						= 0;
	SQLLEN		    nIndicator						= 0;
	SQLCHAR			szColumn[MAX_DATA_WIDTH+20];
	SQLCHAR			szColumnValue[MAX_DATA_WIDTH+1];
	SQLRETURN		nReturn							= 0;
    SQLLEN          nRows                           = 0;
	SQLUINTEGER		nOptimalDisplayWidth            = 10;

	*szColumn = '\0';
	*szColumnValue = '\0';

    nReturn = SQLNumResultCols( hStmt, &nColumns );
    if ( nReturn != SQL_SUCCESS && nReturn != SQL_SUCCESS_WITH_INFO )
        nColumns = -1;

	/* ROWS */
    nReturn = SQLFetch( hStmt );
    while ( nReturn == SQL_SUCCESS || nReturn == SQL_SUCCESS_WITH_INFO )
    {
		/* COLS */
        for( nCol = 1; nCol <= nColumns; nCol++ )
        {
            int sret;
            
            nOptimalDisplayWidth = OptimalDisplayWidth( hStmt, nCol, nUserWidth );
            nReturn = SQLGetData( hStmt, nCol, SQL_C_CHAR, (SQLPOINTER)szColumnValue, sizeof(szColumnValue), &nIndicator );
            szColumnValue[MAX_DATA_WIDTH] = '\0';

            if ( nReturn == SQL_SUCCESS && nIndicator != SQL_NULL_DATA )
            {
                sret = sprintf( (char*)szColumn, "| %-*.*s",
                                nOptimalDisplayWidth, nOptimalDisplayWidth, szColumnValue );
                if (sret < 0) sprintf( (char*)szColumn, "| %-*.*s",
                                nOptimalDisplayWidth, nOptimalDisplayWidth, "**ERROR**" );
                    
            }
	        else if ( nReturn == SQL_ERROR ) 
            {
		        break;
	        }
            else
            {
                sprintf( (char*)szColumn, "| %-*s", nOptimalDisplayWidth, "" );
            }
			fputs( (char*)szColumn, stdout );
        } /* for columns */

        nRows++;
        printf( "|\n" );
        nReturn = SQLFetch( hStmt );
    } /* while rows */

    if ( nReturn == SQL_ERROR )
    {
        if ( bVerbose ) DumpODBCLog( 0, 0, hStmt );
    }

    return nRows;
}

static void
WriteFooterNormal( SQLHSTMT hStmt, SQLCHAR	*szSepLine, SQLLEN nRows )
{
    SQLLEN  nRowsAffected	= -1;

	printf( (char*)szSepLine );

    SQLRowCount( hStmt, &nRowsAffected );
    printf( "SQLRowCount returns %d\n", nRowsAffected );

    if ( nRows )
    {
    	printf( "%d rows fetched\n", nRows );
    }
}



static int DumpODBCLog( SQLHENV hEnv, SQLHDBC hDbc, SQLHSTMT hStmt )
{
	SQLCHAR		szError[501];
	SQLCHAR		szSqlState[10];
    SQLINTEGER  nNativeError;
    SQLSMALLINT	nErrorMsg;

	if ( hStmt )
	{
		while ( SQLError( hEnv, hDbc, hStmt, szSqlState, &nNativeError, szError, 500, &nErrorMsg ) == SQL_SUCCESS )
		{
			printf( "[%s]%s\n", szSqlState, szError );
		}
	}

	if ( hDbc )
	{
		while ( SQLError( hEnv, hDbc, 0, szSqlState, &nNativeError, szError, 500, &nErrorMsg ) == SQL_SUCCESS )
		{
			printf( "[%s]%s\n", szSqlState, szError );
		}
	}

	if ( hEnv )
	{
		while ( SQLError( hEnv, 0, 0, szSqlState, &nNativeError, szError, 500, &nErrorMsg ) == SQL_SUCCESS )
		{
			printf( "[%s]%s\n", szSqlState, szError );
		}
	}

	return 1;
}

static int get_args(char *string, char **args, int maxarg) {
    int nargs = 0;
    char *copy;
    char *p;
    const char *sep = " ";
    char *arg;
    unsigned int i;
    
    if (!string || !args) return 0;
    
    if (!(copy = strdup(string))) return 0;

    for (i = 0; i < maxarg; i++) {
        args[i] = NULL;
    }
    
    p = copy;
    while ((arg = strtok(p, sep))) {
        p = NULL;

        if (strcmp(arg, "\"\"") == 0)
            args[nargs++] = strdup("");
        else if (strcmp(arg, "null") == 0)
            args[nargs++] = NULL;
        else
            args[nargs++] = strdup(arg);
        if (nargs > maxarg) return maxarg;
    }
    free(copy);
    return nargs;
}

static void free_args(char **args, int maxarg) {
    unsigned int i;
    
    for (i = 0; i < maxarg; i++) {
        if (args[i]) {
            free(args[i]);
            args[i] = NULL;
        }
    }
}

static void output_help(void) {
    fprintf(stderr, \
"help usage:\n\n" \
"help help - output this help\n" \
"help - call SQLTables and output the result-set\n" \
"help table_name - call SQLColumns for table_name and output the result-set\n" \
"help catalog schema table type - call SQLTables with these arguments\n" \
"  where any argument may be specified as \"\" (for the empty string) \n" \
"  or null to pass a null pointer argument.\n" \
"\n" \
" e.g.\n" \
" help %% \"\" \"\" \"\" - output list of catalogs\n" \
" help \"\" %% \"\" \"\" - output list of schemas\n" \
" help null null b%% null - output all tables beginning with b\n" \
" help null null null VIEW - output list of views\n" \
            "\n");
}

