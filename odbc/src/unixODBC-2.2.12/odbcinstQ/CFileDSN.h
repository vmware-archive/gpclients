/**************************************************
 * 
 *
 **************************************************
 * This code was created by Peter Harvey @ CodeByDesign. 
 * Released under GPL 31.JAN.99
 *
 * Contributions from...
 * -----------------------------------------------
 * Peter Harvey		- pharvey@codebydesign.com
 * Nick Gorham      - nick@easysoft.com
 **************************************************/
#ifndef CFileDSN_included
#define CFileDSN_included

#include <qwidget.h>
#include <qpushbt.h>
#include <qpixmap.h>
#include <qlayout.h>

#include "CFileList.h"

class CFileDSN : public QWidget
{
    Q_OBJECT

public:

    CFileDSN( QWidget* parent = NULL, const char* name = NULL, QString *cwd = NULL );
    ~CFileDSN();

public slots:
	void NewDir();

protected:
    QPushButton* pbAdd;
    QPushButton* pbRemove;
    QPushButton* pbConfigure;
    QPushButton* pbDir;
    CFileList* pFileList;
    QString path;
    QLabel* dirlab;
};
#endif
