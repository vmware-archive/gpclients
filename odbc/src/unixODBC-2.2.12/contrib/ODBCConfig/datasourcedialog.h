/*
This code was created by Fizz (fizz@titania.co.uk)

ODBCConfig - unixODBC configuration
Copyright (C) 2005  The unixODBC project

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/


#ifndef DATASOURCEDIALOG_H
#define DATASOURCEDIALOG_H

#include <odbcinst.h>
#include <odbcinstext.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qwhatsthis.h>
#include <qmessagebox.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qtabwidget.h>
#include <qstring.h>
#include <qfile.h>

#include "selectfilewidget.h"

#include "qt_datasourcedialog.h"

// Misc control settings...
#define ODBC_DRIVER_SIZE 10000
#define ODBC_FILENAME_MAX 1000
#define ODBC_BUFFER_SIZE 9600

#define DSNMODE_VIEW 0
#define DSNMODE_EDIT 1
#define DSNMODE_ADD 2

#define DSNTYPE_USER 0
#define DSNTYPE_SYSTEM 1
#define DSNTYPE_FILE 2
#define ODBC_DRIVER 3


class DataSourceDialog : public qt_dataSourceDialog
{
	Q_OBJECT

	public:
		DataSourceDialog(QWidget *parent = 0, const char *name = 0, int dialogMode = DSNMODE_VIEW, int dsnType = DSNTYPE_USER, QString fileDir = "");
		~DataSourceDialog();
		bool create(QString dataSource = "", QString driver = "");
											// Creates the dialog, must be called before viewing
                                            // as it creates the dialog. It will return >0 if there
                                            // was a problem creating the dialog.
		bool doneInDriver();				// Has the dialog work been handled in the driver itself

	private:
		int globalDialogMode;				// Dialog mode... Add, Configure or View
		int globalDSNType;					// DSN type... User, System or File
		bool configInDriver;				// Has the setup been handled in the driver itself
		QString dataSourceName;				// Data source name
		QString fileDSNDir;					// File data source directory
		HODBCINSTPROPERTY dataSourceProperties;			// Data source properties
		HODBCINSTPROPERTY dataSourcePointer;			// Pointer to data source properties
		int generalLoop;					// Loop variables used for tracking the number of
		int advancedLoop;					// widgets added to each tab page.
		int loggingLoop;

		// Dialog widgets...
		QLabel *label;
		QComboBox *comboBox;
		QLineEdit *lineEdit;
		QSpacerItem *spacer;
		QWidget *tabPointer;
		SelectFileWidget *fileWidget;

	private slots:
		void saveDataSource();				// Saves the data source
		void setExpertOptions();			// view / hide expert options
};

#endif
