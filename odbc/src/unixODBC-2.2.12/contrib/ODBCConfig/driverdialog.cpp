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


#include "driverdialog.h"

QString driverName;


DriverDialog::DriverDialog(QWidget *parent, const char *name, int dialogMode, QString driver) : qt_driverDialog(parent,name)
{
	// Declare Variables...
	char driverResults[ODBC_BUFFER_SIZE];

	driverName = driver;

	// Configure dialog depending on the mode...
	if (dialogMode == DRIVERDIALOG_VIEW_MODE)			// View Mode...
	{
		driverDescriptionEdit->setReadOnly(TRUE);
		driverEdit->setReadOnly(TRUE);
		driverButton->setHidden(TRUE);
		driverSetupEdit->setReadOnly(TRUE);
		driverSetupButton->setHidden(TRUE);
		cancelButton->setText("Close");
		saveButton->setHidden(TRUE);
	}
	else												// Configure mode
	{
		this->setCaption("Configure ODBC Driver");
		connect(saveButton, SIGNAL(clicked()), this, SLOT(saveDriver()));
		connect(driverButton, SIGNAL(clicked()), this, SLOT(selectDriverName()));
		connect(driverSetupButton, SIGNAL(clicked()), this, SLOT(selectSetupName()));
	}
	label->setText(QString("%1 ODBC Driver") .arg(driver));

	// Populate the driver information
	if (SQLGetPrivateProfileString(driver.latin1(), "Description", "", driverResults, ODBC_BUFFER_SIZE, "odbcinst") > 0)
	{
		driverDescriptionEdit->setText(driverResults);
	}
	if (SQLGetPrivateProfileString(driver.latin1(), "Driver", "", driverResults, ODBC_BUFFER_SIZE, "odbcinst") > 0)
	{
		driverEdit->setText(driverResults);
	}
	if (SQLGetPrivateProfileString(driver.latin1(), "Setup", "", driverResults, ODBC_BUFFER_SIZE, "odbcinst") > 0)
	{
		driverSetupEdit->setText(driverResults);
	}
}


void DriverDialog::saveDriver()
{
	// Update the driver details...
	SQLWritePrivateProfileString(driverName.latin1(), "Description", driverDescriptionEdit->text().latin1(), "odbcinst");
	SQLWritePrivateProfileString(driverName.latin1(), "Driver", driverEdit->text().latin1(), "odbcinst");
	SQLWritePrivateProfileString(driverName.latin1(), "Setup", driverSetupEdit->text().latin1(), "odbcinst");
	this->accept();
}


void DriverDialog::selectDriverName()				// Select Driver File Dialog
{
	driverEdit->setText(QFileDialog::getOpenFileName(".", "Shared Libraries (*.so)\nAll Files (*)", this));
}


void DriverDialog::selectSetupName()				// Select Setup File Dialog
{
	driverSetupEdit->setText(QFileDialog::getOpenFileName(".", "Shared Libraries (*.so)\nAll Files (*)", this));
}
