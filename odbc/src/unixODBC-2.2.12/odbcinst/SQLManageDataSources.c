/**************************************************
 * SQLManageDataSources
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

void get_lib_file( char *lib, char *path )
{
	if ( path ) 
	{
#ifdef SHLIBEXT
 #if defined(__APPLE__) && defined(__GNUC__)
		sprintf( lib, "%s/libodbcinstQ.so", path);
 #else
       	if ( strlen( SHLIBEXT ) > 0 )
        	sprintf( lib, "%s/libodbcinstQ%s.1", path, SHLIBEXT );
        else
            sprintf( lib, "%s/libodbcinstQ.so.1", path );
 #endif
#else
        sprintf( lib, "%s/libodbcinstQ.so.1", path );
#endif        
	}
	else 
	{
#ifdef SHLIBEXT
 #if defined(__APPLE__) && defined(__GNUC__)
		sprintf( lib, "%s/libodbcinstQ.so", DEFLIB_PATH);
 #else
       	if ( strlen( SHLIBEXT ) > 0 )
        	sprintf( lib, "libodbcinstQ%s.1", SHLIBEXT );
        else
            sprintf( lib, "libodbcinstQ.so.1" );
 #endif
#else
        sprintf( lib, "libodbcinstQ.so.1" );
#endif        
	}
}

BOOL SQLManageDataSources( HWND hWnd )
{
	BOOL	nReturn;
	char    szGUILibFile[FILENAME_MAX];
	void 	*hDLL;
	BOOL	(*pSQLManageDataSources)( HWND	);
	char 	*p;

    /*
     * SANITY CHECKS
     *
     */
	if ( !hWnd )
	{
        inst_logPushMsg( __FILE__, __FILE__, __LINE__, LOG_CRITICAL, ODBC_ERROR_INVALID_HWND, "No hWnd" );
		return FALSE;
	}

    if ( lt_dlinit() )
    {
        inst_logPushMsg( __FILE__, __FILE__, __LINE__, LOG_CRITICAL, ODBC_ERROR_GENERAL_ERR, "lt_dlinit() failed" );
		return FALSE;
    }

    /*
     * DETERMINE PATH FOR GUI PLUGIN
	 *
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
    hDLL = lt_dlopen( szGUILibFile );
	if ( hDLL )
	{
        /* change the name, as it avoids it finding it in the calling lib */
		pSQLManageDataSources = (BOOL (*)(HWND))lt_dlsym( hDLL, "QTSQLManageDataSources" );
		if ( pSQLManageDataSources )
			nReturn = pSQLManageDataSources(NULL);
		else
			inst_logPushMsg( __FILE__, __FILE__, __LINE__, LOG_CRITICAL, ODBC_ERROR_GENERAL_ERR, (char*)lt_dlerror() );
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
            pSQLManageDataSources = (BOOL (*)(HWND))lt_dlsym( hDLL, "QTSQLManageDataSources" );
            if ( pSQLManageDataSources )
                nReturn = pSQLManageDataSources(NULL);
            else
                inst_logPushMsg( __FILE__, __FILE__, __LINE__, LOG_CRITICAL, ODBC_ERROR_GENERAL_ERR, (char*)lt_dlerror() );
        }

		inst_logPushMsg( __FILE__, __FILE__, __LINE__, LOG_CRITICAL, ODBC_ERROR_GENERAL_ERR, (char*)lt_dlerror() );
    }

	return nReturn;
}

/*
 * Used by ODBCConfig
 */

BOOL ODBCINSTSQLManageDataSources( HWND hWnd )
{
	BOOL	nReturn;
	char    szGUILibFile[FILENAME_MAX];
	void 	*hDLL;
	BOOL	(*pSQLManageDataSources)( HWND	);
    HODBCINSTWND    hODBCINSTWnd;

    /*
     * SANITY CHECKS
     *
     */
	if ( !hWnd )
	{
        inst_logPushMsg( __FILE__, __FILE__, __LINE__, LOG_CRITICAL, ODBC_ERROR_INVALID_HWND, "No hWnd" );
		return FALSE;
	}

    hODBCINSTWnd = (HODBCINSTWND)hWnd;
	if ( !hODBCINSTWnd->hWnd )
	{
        inst_logPushMsg( __FILE__, __FILE__, __LINE__, LOG_CRITICAL, ODBC_ERROR_INVALID_HWND, "No hODBCINSTWnd->hWnd" );
		return FALSE;
	}

    if ( lt_dlinit() )
    {
        inst_logPushMsg( __FILE__, __FILE__, __LINE__, LOG_CRITICAL, ODBC_ERROR_GENERAL_ERR, "lt_dlinit() failed" );
		return FALSE;
    }

    /*
     * DETERMINE PATH FOR GUI PLUGIN
     */
    if ( strncasecmp( hODBCINSTWnd->szGUI, "QT", 2 ) == 0 )
    {
        char *p;
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
    }
    else
    {
        inst_logPushMsg( __FILE__, __FILE__, __LINE__, LOG_CRITICAL, 
                ODBC_ERROR_INVALID_HWND, "Unsupported hODBCINSTWnd->szGUI" );
		return FALSE;
    }

    /*
     * USE libtool TO LOAD PLUGIN AND CALL FUNCTION
     */
    nReturn = FALSE;
    hDLL = lt_dlopen( szGUILibFile );
	if ( hDLL )
	{
        /* change the name, as it avoids it finding it in the calling lib */
		pSQLManageDataSources = (BOOL (*)(HWND))lt_dlsym( hDLL, "QTSQLManageDataSources" );
		if ( pSQLManageDataSources )
			nReturn = pSQLManageDataSources( (HWND)hODBCINSTWnd->hWnd );
		else
			inst_logPushMsg( __FILE__, __FILE__, __LINE__, LOG_CRITICAL, ODBC_ERROR_GENERAL_ERR, (char*)lt_dlerror() );
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
            pSQLManageDataSources = (BOOL (*)(HWND))lt_dlsym( hDLL, "QTSQLManageDataSources" );
            if ( pSQLManageDataSources )
                nReturn = pSQLManageDataSources( (HWND)hODBCINSTWnd->hWnd );
            else
                inst_logPushMsg( __FILE__, __FILE__, __LINE__, LOG_CRITICAL, ODBC_ERROR_GENERAL_ERR, (char*)lt_dlerror() );
        }

		inst_logPushMsg( __FILE__, __FILE__, __LINE__, LOG_CRITICAL, ODBC_ERROR_GENERAL_ERR, (char*)lt_dlerror() );
    }

	return nReturn;
}
