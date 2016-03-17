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


#ifndef ODBCDIALOG_H
#define ODBCDIALOG_H

#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qlistview.h>
#include <qlineedit.h>
#include <qstring.h>
#include <qcombobox.h>
#include <odbcinstext.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <unistd.h>
#include <qmessagebox.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qfiledialog.h>

#include "adddriverdialog.h"
#include "selectdriverdialog.h"
#include "selectdriverdialog.h"
#include "datasourcedialog.h"

#include "qt_maindialog.h"
#include "qt_about.h"
#include "qt_logindialog.h"

#define ODBC_BUFFER_SIZE 9600
#define ODBC_FILEDSN_SIZE 256


class ODBCDialog : public qt_mainDialog
{
	Q_OBJECT

	public:
		ODBCDialog(QWidget *parent = 0, const char *name = 0);
		
	private slots:
		void addDriver();
		void addDataSource();
		void about();
		void enableDriverButtons();
		void enableSystemButtons();
		void enableUserButtons();
		void enableFileButtons();
		void updateDriverList();
		void updateDSNList(int dsnType);
		void updateFileList(const QString&);
		void removeDriver();
		void removeDSN();
		void removeFileDSN();
		void viewDataSource();
		void configureDataSource();
		void doubleClickedDSN(QListViewItem*);
		void setFileDSNDirectory();
		void defaultFileDSN();
		void autoSetupDrivers();
		void setDebugOptions();
		void selectTraceFile();
		void tracingEnable(bool);
		void saveDebugSettings();
		void turnOffDebug();
		void testDSN();
};

#endif
