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


#include <qpushbutton.h>
#include <qstring.h>
#include <qcombobox.h>
#include <qmessagebox.h>
#include <odbcinstext.h>

#include "selectdriverdialog.h"

#define ODBC_BUFFER_SIZE 9600


SelectDriverDialog::SelectDriverDialog(QWidget *parent, const char *name) : qt_selectDriverDialog(parent,name)
{
	// Declare variables...
	char driverResults[ODBC_BUFFER_SIZE];
	QString driverName;
	int driverResultsPos = 0;

	// Connect SIGNAL / SLOTS...
	connect(selectDriverButton, SIGNAL(clicked()), this, SLOT(accept()));
	connect(cancelSelectButton, SIGNAL(clicked()), this, SLOT(reject()));

	// Read list of installed drivers...
	if (SQLGetInstalledDrivers(driverResults, ODBC_BUFFER_SIZE, 0) > 0)
	{
		driverName = QString(driverResults);

		if (driverName.isEmpty())
		{
			QMessageBox::warning(this, tr("Select Driver"), tr("There was a problem retrieving a list of drivers, are there any drivers installed?"), QMessageBox::Abort, QMessageBox::NoButton, QMessageBox::NoButton);
		}
		else
		{
			// Populate drivers combo...
			while (!driverName.isEmpty())
			{
				selectDriverCombo->insertItem(driverName);
				driverResultsPos = driverResultsPos + driverName.length() + 1;
				if (driverResultsPos < ODBC_BUFFER_SIZE)
				{
					driverName = QString(driverResults + driverResultsPos);
				}
				else
				{
					driverName = "";
				}
			}
		}
	}
	else
	{
		QMessageBox::warning(this, tr("Select Driver"), tr("Could not retrieve a list of drivers, are there any drivers installed?"), QMessageBox::Abort, QMessageBox::NoButton, QMessageBox::NoButton);
	}
}


QString SelectDriverDialog::selectedDriver()
{
	return selectDriverCombo->currentText();
}


