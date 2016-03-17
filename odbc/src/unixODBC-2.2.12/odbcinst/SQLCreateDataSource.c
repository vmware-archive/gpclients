/**************************************************
 * SQLCreateDataSource
 *
 * This 100% GUI so simply pass it on to odbcinst's GUI
 * shadow share.
 *
 **************************************************
 * This code was created by Peter Harvey @ CodeByDesign.
 * Released under LGPL 28.JAN.99
 *
 * Contributions from...
 * -----------------------------------------------
 * Peter Harvey		- pharvey@codebydesign.com
 **************************************************/
#include <odbcinstext.h>

/*
 * Take a wide string consisting of null terminated sections, and copy to a ASCII version
 */

char* _multi_string_alloc_and_copy( LPCWSTR in )
{
	char *chr;
	int len = 0;

	while( in[ len ] != 0 || in[ len + 1 ] != 0 )
	{
		len ++;
	}

	chr = malloc( len + 2 );

	len = 0;
	while( in[ len ] != 0 || in[ len + 1 ] != 0 )
	{
		chr[ len ] = 0xFF & in[ len ];
		len ++;
	}
	chr[ len ++ ] = '\0';
	chr[ len ++ ] = '\0';

	return chr;
}

char* _single_string_alloc_and_copy( LPCWSTR in )
{
	char *chr;
	int len = 0;

	while( in[ len ] != 0 )
	{
		len ++;
	}

	chr = malloc( len + 1 );

	len = 0;
	while( in[ len ] != 0 )
	{
		chr[ len ] = 0xFF & in[ len ];
		len ++;
	}
	chr[ len ++ ] = '\0';

	return chr;
}

void _single_string_copy_to_wide( SQLWCHAR *out, LPCSTR in, int len )
{
	while( len > 0 && *in ) 
	{
		*out = *in;
		out++;
		in++;
		len --;
	}
	*out = 0;
}

void _single_copy_to_wide( SQLWCHAR *out, LPCSTR in, int len )
{
	while( len >= 0 ) 
	{
		*out = *in;
		out++;
		in++;
		len --;
	}
}

void _multi_string_copy_to_wide( SQLWCHAR *out, LPCSTR in, int len )
{
	while( len > 0 && ( in[ 0 ] || in[ 1 ] )) 
	{
		*out = *in;
		out++;
		in++;
		len --;
	}
	*out++ = 0;
	*out++ = 0;
}

BOOL SQLCreateDataSource(		HWND	hWnd,
								LPCSTR	pszDS )
{
	BOOL	nReturn;
	void 	*hDLL;
	BOOL	(*pSQLCreateDataSource)(HWND, LPCSTR);
	char 	*p;
	char    szGUILibFile[FILENAME_MAX];
	int		found;

	if ( !hWnd )
	{
        inst_logPushMsg( __FILE__, __FILE__, __LINE__, LOG_CRITICAL, ODBC_ERROR_INVALID_HWND, "" );
		return FALSE;
	}

    /*
     * initialize libtool
     */

    lt_dlinit();

    /*
     * DETERMINE PATH FOR GUI PLUGIN
     *
     */

    /*
     * first look in the environment
     */

	p  = getenv( "ODBCINSTQ" );
	if ( p )
	{
		strcpy( szGUILibFile, p );
	}
	else
	{
		SQLGetPrivateProfileString( "ODBC", "ODBCINSTQ", "", szGUILibFile, sizeof( szGUILibFile ), "odbcinst.ini" );

		if ( strlen( szGUILibFile ) == 0 )
		{
			/*
			 * we need to find the extension to use as well
			 */

			get_lib_file( szGUILibFile, NULL );

			if ( lt_dladdsearchdir( DEFLIB_PATH ) )
			{
				inst_logPushMsg( __FILE__, __FILE__, __LINE__, LOG_CRITICAL, 
						ODBC_ERROR_GENERAL_ERR, (char*)lt_dlerror() );
			}
		}
	}

    /*
     * USE libtool TO LOAD PLUGIN AND CALL FUNCTION
     */

    nReturn = FALSE;
	found = FALSE;
    hDLL = lt_dlopen( szGUILibFile );
	if ( hDLL )
	{
        /* change the name, as it avoids it finding it in the calling lib */
		pSQLCreateDataSource = (BOOL (*)(HWND, LPSTR))lt_dlsym( hDLL, "QTSQLCreateDataSources" );
		if ( pSQLCreateDataSource )
			nReturn = pSQLCreateDataSource(NULL,pszDS);
		else
			inst_logPushMsg( __FILE__, __FILE__, __LINE__, LOG_CRITICAL, ODBC_ERROR_GENERAL_ERR, (char*)lt_dlerror() );

		found = TRUE;
	}
	else
   	{
        /*
         * try after adding the explicit path
         */

		get_lib_file( szGUILibFile, DEFLIB_PATH );
        hDLL = lt_dlopen( szGUILibFile );
        if ( hDLL )
        {
            /* change the name, as it avoids it finding it in the calling lib */
            pSQLCreateDataSource = (BOOL (*)(HWND))lt_dlsym( hDLL, "QTSQLCreateDataSources" );
            if ( pSQLCreateDataSource )
                nReturn = pSQLCreateDataSource(NULL,pszDS);
            else
                inst_logPushMsg( __FILE__, __FILE__, __LINE__, LOG_CRITICAL, ODBC_ERROR_GENERAL_ERR, (char*)lt_dlerror() );

			found = TRUE;
        }
    }

	if ( !found )
	{
		/* TRY TO PASS THROUGH TO GUI SHADOW LIB */
		if ( (hDLL = lt_dlopen( "libodbcinstG.so" ))  )
		{
			pSQLCreateDataSource = (BOOL(*)(HWND,LPSTR))lt_dlsym( hDLL, "SQLCreateDataSource" );
			if ( pSQLCreateDataSource )
				nReturn = pSQLCreateDataSource( NULL,pszDS );
			else
				inst_logPushMsg( __FILE__, __FILE__, __LINE__, LOG_CRITICAL, ODBC_ERROR_GENERAL_ERR, "" );

			found = TRUE;
		}
	}

	if ( !found )
	{
		inst_logPushMsg( __FILE__, __FILE__, __LINE__, LOG_CRITICAL, ODBC_ERROR_GENERAL_ERR, "" );
	}

	return nReturn;
}

BOOL INSTAPI SQLCreateDataSourceW     (HWND       hwndParent,
                                      LPCWSTR     lpszDSN)
{
	BOOL ret;
	char *ms = _multi_string_alloc_and_copy( lpszDSN );

	ret = SQLCreateDataSource( hwndParent, ms );

	free( ms );

	return ret;
}
