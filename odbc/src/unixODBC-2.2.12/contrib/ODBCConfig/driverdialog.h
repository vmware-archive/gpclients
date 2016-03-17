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


#ifndef DRIVERDIALOG_H
#define DRIVERDIALOG_H

#include <qpushbutton.h>
#include <qlineedit.h>
#include <qtoolbutton.h>
#include <odbcinstext.h>
#include <qlabel.h>
#include <qfiledialog.h>
#include <qmessagebox.h>

#include "qt_driverdialog.h"

#define DRIVERDIALOG_VIEW_MODE 0
#define DRIVERDIALOG_EDIT_MODE 1
#define ODBC_BUFFER_SIZE 9600


class DriverDialog : public qt_driverDialog
{
	Q_OBJECT

	public:
		DriverDialog(QWidget *parent = 0, const char *name = 0, int dialogMode = DRIVERDIALOG_VIEW_MODE, QString driver = "");

	private slots:
		void saveDriver();
		void selectDriverName();
		void selectSetupName();
};

#endif
