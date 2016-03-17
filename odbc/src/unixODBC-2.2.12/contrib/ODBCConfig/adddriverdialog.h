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


#ifndef ADDDRIVERDIALOG_H
#define ADDDRIVERDIALOG_H

#include <qfile.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qlayout.h>
#include <qbuttongroup.h>
#include <qtoolbutton.h>
#include <qwidgetstack.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qfiledialog.h>
#include <qstring.h>
#include <qmessagebox.h>
#include <odbcinstext.h>

#include "qt_adddriverdialog.h"

// Wizard Page Titles...
#define PAGE_0 "Add ODBC Driver"
#define PAGE_1 "Configure ODBC Driver"
#define PAGE_2 "Configure ODBC Driver Expert Options"
#define PAGE_3_OK "Added ODBC Driver Successfully"
#define PAGE_3_FAIL "Add ODBC Driver Failed"

// Misc control settings...
#define ODBC_DRIVER_SIZE 10000
#define ODBC_FILENAME_MAX 1000
#define ODBC_BUFFER_SIZE 9600


QString LibraryExists(QString libraryName);
QString Library64Exists(QString libraryName);
bool AddDriver(QString driverName, QString driverDescription, QString driver, QString driverSetup = "", QString driver64 = "", QString driverSetup64 = "", QString timeout = "", QString reuse = "");
bool AutoSetupDrivers();
bool DriverAlreadyConfigured(QString driverLibraryName);


class AddDriverDialog : public qt_addDriverDialog
{
	Q_OBJECT

	public:
		AddDriverDialog(QWidget *parent = 0, const char *name = 0);

	private:
		QButtonGroup *driverGroup;
		QRadioButton *odbcRadio;
		QRadioButton *otherRadio;
		QRadioButton *otherFileRadio;
		
	private slots:
		void nextPage();
		void previousPage();
		void addODBCDriver();
		void selectDriverName();
		void selectSetupName();
		void selectDriver64Name();
		void selectSetup64Name();
};

#endif
