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


#include "adddriverdialog.h"


// Driver Array: Name, Description, Driver, Setup (no directory paths!)
const QString ODBCDrivers[][4] = {
	{"MyODBC", "MySQL ODBC 2.50 Driver", "libmyodbc.so", "libodbcmyS.so"},
	{"MyODBC3", "MySQL ODBC 3.51 Driver", "libmyodbc3.so", "libodbcmyS.so"},
	{"FreeTDS", "FreeTDS Sybase and MS-SQL Driver", "libtdsodbc.so", "libtdsS.so"},
	{"Oracle", "Oracle Driver", "liboraodbc.so", "liboraodbcS.so"},
	{"DB2", "IBM DB2 Driver", "libdb2.so", ""},
	{"PostgreSQL", "PostgreSQL Driver", "libodbcpsql.so", "libodbcpsqlS.so"},
	{"mSQL", "Mini SQL Driver", "libodbcmini.so", "libodbcminiS.so"},
	{"MimerSQL", "Mimer SQL Driver", "libmimer.so", "libmimerS.so"},
	{"Text", "Text Driver", "libodbctxt.so", "libodbctxtS.so"},
	{"NN", "Internet News Driver", "libnn.so", "libodbcnnS.so"},
	{"Interbase", "Easysoft Interbase Driver", "libibodbc.so", "libibodbcS.so"},
	{"Firebird", "Easysoft Firebird Driver", "libfbodbc.so", "libfbodbcS.so"},
	{"Informix", "Informix Driver", "libifcli.so", ""},
	{"SQLite", "SQLite Driver", "libsqliteodbc.so", "libsqliteodbc.so"},
	{"", "", "", ""}};

// Driver / Setup Library Directory Paths...
const QString LibPaths[] = {
	"/usr/lib32/",
	"/usr/lib/",
	"/usr/local/lib32/",
	"/usr/local/lib/",
	"/usr/local/easysoft/fb/",
	"/usr/local/easysoft/ib/",
	"/usr/local/informix/lib/cli/",
	""};

const QString Lib64Paths[] = {
	"/usr/lib64/",
	"/usr/local/lib64/",
	""};


AddDriverDialog::AddDriverDialog(QWidget *parent, const char *name) : qt_addDriverDialog(parent,name)
{
	// Variables...
	QString driverTempString;

	// Initialise variables...
	int loop = 0;

	// Detect what drivers are available...
	driverGroup = new QButtonGroup(selectDriverPage, "driverGroup");
	selectDriverPageLayout->addWidget(driverGroup,1,0);
	driverGroup->setColumnLayout(0, Qt::Vertical);
    driverGroup->layout()->setSpacing(6);
    driverGroup->layout()->setMargin(11);
	driverGroup->setTitle(tr("ODBC Drivers"));
	QGridLayout *driverGroupLayout = new QGridLayout(driverGroup->layout());
	driverGroupLayout->setAlignment(Qt::AlignTop);

	// Detect which drivers are present and add radio widgets...
	for (int driverLoop = 0; ODBCDrivers[driverLoop][0].isEmpty() == FALSE; driverLoop++)
	{
		driverTempString = LibraryExists(ODBCDrivers[driverLoop][2]);
		if (driverTempString.isEmpty())
			driverTempString = Library64Exists(ODBCDrivers[driverLoop][2]);
		if (driverTempString.isEmpty() == FALSE)
		{
			if (DriverAlreadyConfigured(driverTempString) == FALSE)
			{
				// Add driver radio button...
				odbcRadio = new QRadioButton(driverGroup, ODBCDrivers[driverLoop][0]);
				driverGroupLayout->addWidget(odbcRadio, loop, 0);
				odbcRadio->setText(tr(ODBCDrivers[driverLoop][1]));
				if (loop == 0)
					odbcRadio->setChecked(TRUE);
				loop++;
			}
		}
	}

	// Add other widgets...
	otherRadio = new QRadioButton(driverGroup, "otherRadio" );		// add Other
	driverGroupLayout->addWidget(otherRadio, loop+1, 0);
	otherRadio->setText(tr("Other (Server Based)"));
	otherFileRadio = new QRadioButton(driverGroup, "otherFileRadio" );		// add Other (File Based)
	driverGroupLayout->addWidget(otherFileRadio, loop+2, 0);
	otherFileRadio->setText(tr("Other (File Based)"));
	QSpacerItem *driverSpacer = new QSpacerItem(1, 2, QSizePolicy::Minimum, QSizePolicy::Expanding);
	driverGroupLayout->addItem(driverSpacer, loop+3, 0);

	// Configure connections...
	connect(nextButton, SIGNAL(clicked()), this, SLOT(nextPage()));
	connect(previousButton, SIGNAL(clicked()), this, SLOT(previousPage()));
	connect(driverButton, SIGNAL(clicked()), this, SLOT(selectDriverName()));
	connect(setupButton, SIGNAL(clicked()), this, SLOT(selectSetupName()));
	connect(driver64Button, SIGNAL(clicked()), this, SLOT(selectDriver64Name()));
	connect(setup64Button, SIGNAL(clicked()), this, SLOT(selectSetup64Name()));

	// Set button defaults...
	previousButton->setEnabled(FALSE);
	expertCheck->setChecked(FALSE);
	nextButton->setMinimumSize(previousButton->sizeHint());

	resize(sizeHint());
}


////////////////////////////////////////////////////////////////////////////
// CONTROL THE WIZARD SECTION

void AddDriverDialog::nextPage()						// Clicked Next Button
{
	// Variables...
	QString tempString;
	bool selectionFound = FALSE;

	switch(wizard->id(wizard->visibleWidget()))
	{
		case 0:
			// Clear driver configuration first...
			driverNameEdit->clear();
			driverDescriptionEdit->clear();
			driverEdit->clear();
			driverSetupEdit->clear();
			driver64Edit->clear();
			driverSetup64Edit->clear();
			timeoutEdit->clear();
			reuseEdit->clear();

			// Detect which ODBC driver has been selected, and add text to widgets...
			for (int driverLoop = 0; ODBCDrivers[driverLoop][0].isEmpty() == FALSE; driverLoop++)
			{
				if (ODBCDrivers[driverLoop][0].compare(((QRadioButton *)driverGroup->selected())->name()) == 0)
				{
					driverNameEdit->setText(ODBCDrivers[driverLoop][0]);
					driverDescriptionEdit->setText(ODBCDrivers[driverLoop][1]);
					tempString = LibraryExists(ODBCDrivers[driverLoop][2]);
					driverEdit->setText(tempString);
					tempString = LibraryExists(ODBCDrivers[driverLoop][3]);
					driverSetupEdit->setText(tempString);
					tempString = Library64Exists(ODBCDrivers[driverLoop][2]);
					driver64Edit->setText(tempString);
					tempString = Library64Exists(ODBCDrivers[driverLoop][3]);
					driverSetup64Edit->setText(tempString);
					selectionFound = TRUE;
				}
			}
			
			// If not a detected ODBC driver, it must be an other...
			if (selectionFound == FALSE)
			{
				if (driverGroup->selected() == otherRadio)
				{
					tempString = LibraryExists("libodbcdrvcfg1S.so");
					driverSetupEdit->setText(tempString);
					tempString = Library64Exists("libodbcdrvcfg1S.so");
					driverSetup64Edit->setText(tempString);
				}
				else
				{
					tempString = LibraryExists("libodbcdrvcfg2S.so");
					driverSetupEdit->setText(tempString);
					tempString = Library64Exists("libodbcdrvcfg2S.so");
					driverSetup64Edit->setText(tempString);
				}
			}

			// Goto the next page...
			if ((otherRadio->isChecked() == TRUE) || (otherFileRadio->isChecked() == TRUE) || (expertCheck->isChecked() == TRUE))
													// Other driver is selected
			{
				wizardLabel->setText(tr(PAGE_1));
				previousButton->setEnabled(TRUE);
				wizard->raiseWidget(1);
			}
			else 									// Other driver selected (no expert)
			{
				cancelButton->setEnabled(FALSE);
				previousButton->setEnabled(FALSE);
				nextButton->setText(tr("Finished"));
				expertCheck->setHidden(TRUE);
				addODBCDriver();
				wizard->raiseWidget(3);
			}
			break;
		case 1:
			if (expertCheck->isChecked() == TRUE)
			{
				wizardLabel->setText(tr(PAGE_2));
				wizard->raiseWidget(2);
			}
			else
			{
				cancelButton->setEnabled(FALSE);
				previousButton->setEnabled(FALSE);
				nextButton->setText(tr("Finished"));
				expertCheck->setHidden(TRUE);
				addODBCDriver();
				wizard->raiseWidget(3);
			}
			break;
		case 2:
			cancelButton->setEnabled(FALSE);
			previousButton->setEnabled(FALSE);
			nextButton->setText(tr("Finished"));
			expertCheck->setHidden(TRUE);
			addODBCDriver();
			wizard->raiseWidget(3);
			break;
	}

	resize(sizeHint());
}

void AddDriverDialog::previousPage()					// Clicked Previous Button
{
	switch(wizard->id(wizard->visibleWidget()))
	{
		case 1:
			previousButton->setEnabled(FALSE);
			wizardLabel->setText(tr(PAGE_0));
			wizard->raiseWidget(0);
			break;
		case 2:
			wizardLabel->setText(tr(PAGE_1));
			wizard->raiseWidget(1);
			break;
	}

	resize(sizeHint());
}


////////////////////////////////////////////////////////////////////////////
// ADD THE DRIVER SECTION

void AddDriverDialog::addODBCDriver()				// Add ODBC Driver
{
	QString tempText;

	// Reconnect SIGNAL / SLOTS
	disconnect(nextButton, SIGNAL(clicked()), this, SLOT(nextPage()));

	// Add driver
	if (AddDriver(driverNameEdit->text(), driverDescriptionEdit->text(), driverEdit->text(), driverSetupEdit->text(), driver64Edit->text(), driverSetup64Edit->text(), timeoutEdit->text(), reuseEdit->text()))
	{
		wizardLabel->setText(tr(PAGE_3_OK));
		tempText = "The following ODBC driver has been successfully added:\n\nDriver Name: ";
		tempText.append(driverNameEdit->text());
		if (driverDescriptionEdit->text().isEmpty() == FALSE)
		{
			tempText.append("\nDescription: ");
			tempText.append(driverDescriptionEdit->text());
		}
		if (driverEdit->text().isEmpty() == FALSE)
		{
			tempText.append("\nODBC Driver: ");
			tempText.append(driverEdit->text());
		}
		if (driverSetupEdit->text().isEmpty() == FALSE)
		{
			tempText.append("\nDriver Setup Library: ");
			tempText.append(driverSetupEdit->text());
		}
		tempText.append("\n\nThe driver is now available for data sources (DSN).");
		finishedLabel->setText(tr(tempText));
		connect(nextButton, SIGNAL(clicked()), this, SLOT(accept()));
	}
	else
	{
		wizardLabel->setText(tr(PAGE_3_FAIL));
		finishedLabel->setText(tr("Failed to install the ODBC driver, check that\nthe driver actually exists."));
		connect(nextButton, SIGNAL(clicked()), this, SLOT(reject()));
	}
}

bool AddDriver(QString driverName, QString driverDescription, QString driver, QString driverSetup, QString driver64, QString driverSetup64, QString timeout, QString reuse)		// Add Driver
{
	// Variables...
	char driverToInstall[ODBC_DRIVER_SIZE];
	char *driverPos;
	char pathOut[ODBC_FILENAME_MAX+1];
	DWORD usageCount = 0;
	QString driverTempString;

	// Variable initialisation...
	memset(driverToInstall, '\0', ODBC_DRIVER_SIZE);
	driverPos = driverToInstall;

	if (driverName.isEmpty() || (driver.isEmpty() && driver64.isEmpty()) || ((QFile::exists(driver) == FALSE) && (QFile::exists(driver64) == FALSE)))
	{
		return FALSE;
	}
	else
	{
		sprintf(driverPos, "%s", driverName.ascii());
		driverPos += driverName.length() + 1;
		if (driverDescription.isEmpty() == FALSE)
		{
			sprintf(driverPos, "Description=%s", driverDescription.ascii());
			driverPos += driverDescription.length() + 13;
		}
		if (driver.isEmpty() == FALSE)
		{
			if (QFile::exists(driver))
			{
				sprintf(driverPos, "Driver=%s", driver.ascii());
				driverPos += driver.length() + 8;
			}
		}
		if (driver64.isEmpty() == FALSE)
		{
			if (QFile::exists(driver64))
			{
				sprintf(driverPos, "Driver64=%s", driver64.ascii());
				driverPos += driver64.length() + 10;
			}
		}
		if (driverSetup.isEmpty() == FALSE)
		{
			if (QFile::exists(driverSetup))
			{
				sprintf(driverPos, "Setup=%s", driverSetup.ascii());
				driverPos += driverSetup.length() + 7;
			}
		}
		if (driverSetup64.isEmpty() == FALSE)
		{
			if (QFile::exists(driverSetup64))
			{
				sprintf(driverPos, "Setup64=%s", driverSetup64.ascii());
				driverPos += driverSetup64.length() + 9;
			}
		}
		if (timeout.isEmpty() == FALSE)
		{
			sprintf(driverPos, "CPTimeout=%s", timeout.ascii());
			driverPos += timeout.length() + 11;
		}
		if (reuse.isEmpty() == FALSE)
		{
			sprintf(driverPos, "CPReuse=%s", reuse.ascii());
			driverPos += reuse.length() + 9;
		}
		
		// Install driver...
		if (SQLInstallDriverEx(driverToInstall, NULL, pathOut, ODBC_FILENAME_MAX, NULL, ODBC_INSTALL_COMPLETE, &usageCount) == TRUE)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
}

bool AutoSetupDrivers()					// Automatically setup drivers
{
	// Variables...
	int driverLoop;
	QString driversSetupText = "";
	QString driverTempString;
	QString driver64TempString;
	QString driverSetupTempString;
	QString driverSetup64TempString;
	bool driversAdded = FALSE;

	// Detect which drivers are present
	for (driverLoop = 0; ODBCDrivers[driverLoop][0].isEmpty() == FALSE; driverLoop++)
	{
		driverTempString = LibraryExists(ODBCDrivers[driverLoop][2]);
		driver64TempString = Library64Exists(ODBCDrivers[driverLoop][2]);
		driverSetupTempString = LibraryExists(ODBCDrivers[driverLoop][3]);
		driverSetup64TempString = Library64Exists(ODBCDrivers[driverLoop][3]);
		if ((driverTempString.isEmpty() == FALSE) || (driverTempString.isEmpty() == FALSE))
		{
			if (DriverAlreadyConfigured(driverTempString) == FALSE)
			{
				// Add driver...
				if (AddDriver(ODBCDrivers[driverLoop][0], ODBCDrivers[driverLoop][1], driverTempString, driverSetupTempString, driver64TempString, driverSetup64TempString, "", ""))
				{
					driversSetupText.append(ODBCDrivers[driverLoop][1]);
					driversSetupText.append("\n");
					driversAdded = TRUE;
				}
			}
		}
	}

	// Display Status Message Box
	if (driversAdded == TRUE)
	{
		driversSetupText.prepend("ODBC driver auto setup detected and\ninstalled the following driver(s):\n\n");
		QMessageBox::information(NULL, QObject::tr("Auto Setup ODBC Drivers"), QObject::tr(driversSetupText), QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
	}
	else
	{
		QMessageBox::information(NULL, QObject::tr("Auto Setup ODBC Drivers"), QObject::tr("There were no missing ODBC\ndrivers detected."), QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
	}
	
	// Return if drivers were added
	return driversAdded;
}


////////////////////////////////////////////////////////////////////////////
// SUPPORTING FUNCTIONS SECTION

QString LibraryExists(QString libraryName)					// Check all the library paths for the library
{
	// Variables...
	QString libraryFound = "";
	QString tempString;

	for (int driverLoop = 0; ((LibPaths[driverLoop].isEmpty() == FALSE) && (libraryFound.isEmpty())); driverLoop++)
	{
		tempString = LibPaths[driverLoop];
		tempString.append(libraryName);
		if (QFile::exists(tempString))
			libraryFound = tempString;
	}

	return libraryFound;
}

QString Library64Exists(QString libraryName)					// Check all the library 64 paths for the library
{
	// Variables...
	QString libraryFound = "";
	QString tempString;

	for (int driverLoop = 0; ((Lib64Paths[driverLoop].isEmpty() == FALSE) && (libraryFound.isEmpty())); driverLoop++)
	{
		tempString = Lib64Paths[driverLoop];
		tempString.append(libraryName);
		if (QFile::exists(tempString))
			libraryFound = tempString;
	}

	return libraryFound;
}

bool DriverAlreadyConfigured(QString driverLibraryName)		// Find already configured drivers
{
	// Declare variables...
	char odbcResults[ODBC_BUFFER_SIZE];
	char odbcDescResults[ODBC_BUFFER_SIZE];
	bool driverFound = FALSE;
	QString odbcName;
	int odbcResultsPos = 0;

	if (SQLGetInstalledDrivers(odbcResults, ODBC_BUFFER_SIZE, 0) > 0)
	{
		odbcName = QString(odbcResults);
		while (!odbcName.isEmpty())
		{
			// Compare driver to that specified...
			if (SQLGetPrivateProfileString(odbcName.latin1(), "Driver", "", odbcDescResults, ODBC_BUFFER_SIZE, "odbcinst") > 0)
			{
				if (driverLibraryName.compare(QString(odbcDescResults)) == 0)
					driverFound = TRUE;
			}
			if (SQLGetPrivateProfileString(odbcName.latin1(), "Driver64", "", odbcDescResults, ODBC_BUFFER_SIZE, "odbcinst") > 0)
			{
				if (driverLibraryName.compare(QString(odbcDescResults)) == 0)
					driverFound = TRUE;
			}

			// Get the next driver name...
			odbcResultsPos = odbcResultsPos + odbcName.length() + 1;
			if ((odbcResultsPos < ODBC_BUFFER_SIZE) && (driverFound == FALSE))
			{
				odbcName = QString(odbcResults + odbcResultsPos);
			}
			else
			{
				odbcName = "";
			}
		}
	}

	return driverFound;
}


////////////////////////////////////////////////////////////////////////////
// MISC SECTION

void AddDriverDialog::selectDriverName()			// Select Driver File Dialog
{
	driverEdit->setText(QFileDialog::getOpenFileName(".", "Shared Libraries (*.so)\nAll Files (*)", this));
}

void AddDriverDialog::selectSetupName()				// Select Setup File Dialog
{
	driverSetupEdit->setText(QFileDialog::getOpenFileName(".", "Shared Libraries (*.so)\nAll Files (*)", this));
}

void AddDriverDialog::selectDriver64Name()			// Select Driver File Dialog
{
	driver64Edit->setText(QFileDialog::getOpenFileName(".", "Shared Libraries (*.so)\nAll Files (*)", this));
}

void AddDriverDialog::selectSetup64Name()				// Select Setup File Dialog
{
	driverSetup64Edit->setText(QFileDialog::getOpenFileName(".", "Shared Libraries (*.so)\nAll Files (*)", this));
}
