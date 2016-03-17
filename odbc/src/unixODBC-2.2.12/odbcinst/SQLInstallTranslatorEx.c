/**************************************************
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

BOOL SQLInstallTranslatorEx(	LPCSTR	pszTranslator,
								LPCSTR	pszPathIn,
								LPSTR	pszPathOut,
								WORD	nPathOutMax,
								WORD	*pnPathOut,
								WORD	nRequest,
								LPDWORD	pnUsageCount )

{
	return FALSE;
}

BOOL INSTAPI SQLInstallTranslatorExW(LPCWSTR lpszTranslator,
                                  LPCWSTR lpszPathIn,
                                  LPWSTR  lpszPathOut,
                                  WORD   cbPathOutMax,
                                  WORD  *pcbPathOut,
                                  WORD   fRequest,
                                  LPDWORD   lpdwUsageCount)
{
	return FALSE;	
}
