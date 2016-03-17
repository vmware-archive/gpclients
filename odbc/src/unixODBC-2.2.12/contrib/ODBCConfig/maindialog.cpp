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


#include "maindialog.h"


ODBCDialog::ODBCDialog(QWidget *parent, const char *name) : qt_mainDialog(parent,name)
{
	// If not "root" configure / disable options...
	if (getuid() > 0)
	{
		// set buttons...
		systemAddButton->setEnabled(FALSE);
		systemConfigureButton->setText(tr("View"));
		driverAddButton->setEnabled(FALSE);
		driverConfigureButton->setText(tr("View"));
		driverRemoveButton->setEnabled(FALSE);
		driverAutoButton->setEnabled(FALSE);
		fileDefaultButton->setEnabled(FALSE);
		fileRemoveButton->setEnabled(FALSE);
		fileAddButton->setEnabled(FALSE);
		fileConfigureButton->setText(tr("View"));
		tracingCheck->setEnabled(FALSE);
		forceTracingCheck->setEnabled(FALSE);
		traceFileEdit->setEnabled(FALSE);
		traceFileButton->setEnabled(FALSE);
		connectionPoolingCheck->setEnabled(FALSE);
		debugApplyButton->setEnabled(FALSE);
		debugOffButton->setEnabled(FALSE);

		// Configure user SIGNAL / SLOTS
		connect(driverConfigureButton, SIGNAL(clicked()), this, SLOT(viewDataSource()));
		connect(systemConfigureButton, SIGNAL(clicked()), this, SLOT(viewDataSource()));
		connect(systemList, SIGNAL(doubleClicked(QListViewItem*)), this, SLOT(doubleClickedDSN(QListViewItem*)));
		connect(driverList, SIGNAL(doubleClicked(QListViewItem*)), this, SLOT(doubleClickedDSN(QListViewItem*)));
	}
	else
	{
		// Configure root SIGNAL / SLOTS
		connect(systemRemoveButton, SIGNAL(clicked()), this, SLOT(removeDSN()));
		connect(systemAddButton, SIGNAL(clicked()), this, SLOT(addDataSource()));
		connect(systemConfigureButton, SIGNAL(clicked()), this, SLOT(configureDataSource()));
		connect(systemList, SIGNAL(doubleClicked(QListViewItem*)), this, SLOT(doubleClickedDSN(QListViewItem*)));
		connect(driverAddButton, SIGNAL(clicked()), this, SLOT(addDriver()));
		connect(driverRemoveButton, SIGNAL(clicked()), this, SLOT(removeDriver()));
		connect(driverList, SIGNAL(doubleClicked(QListViewItem*)), this, SLOT(doubleClickedDSN(QListViewItem*)));
		connect(driverConfigureButton, SIGNAL(clicked()), this, SLOT(configureDataSource()));
		connect(driverAutoButton, SIGNAL(clicked()), this, SLOT(autoSetupDrivers()));
		connect(fileAddButton, SIGNAL(clicked()), this, SLOT(addDataSource()));
		connect(fileConfigureButton, SIGNAL(clicked()), this, SLOT(configureDataSource()));
		connect(fileRemoveButton, SIGNAL(clicked()), this, SLOT(removeFileDSN()));
		connect(fileDefaultButton, SIGNAL(clicked()), this, SLOT(defaultFileDSN()));
		connect(traceFileButton, SIGNAL(clicked()), this, SLOT(selectTraceFile()));
		connect(tracingCheck, SIGNAL(toggled(bool)), this, SLOT(tracingEnable(bool)));
		connect(debugApplyButton, SIGNAL(clicked()), this, SLOT(saveDebugSettings()));
		connect(debugOffButton, SIGNAL(clicked()), this, SLOT(turnOffDebug()));
	}

	// Connect remaining SIGNALS and SLOTS...
	connect(aboutButton, SIGNAL(clicked()), this, SLOT(about()));
	connect(userRemoveButton, SIGNAL(clicked()), this, SLOT(removeDSN()));
	connect(userAddButton, SIGNAL(clicked()), this, SLOT(addDataSource()));
	connect(userConfigureButton, SIGNAL(clicked()), this, SLOT(configureDataSource()));
	connect(userList, SIGNAL(selectionChanged()), this, SLOT(enableUserButtons()));
	connect(userList, SIGNAL(doubleClicked(QListViewItem*)), this, SLOT(doubleClickedDSN(QListViewItem*)));
	connect(userTestButton, SIGNAL(clicked()), this, SLOT(testDSN()));
	connect(systemList, SIGNAL(selectionChanged()), this, SLOT(enableSystemButtons()));
	connect(systemTestButton, SIGNAL(clicked()), this, SLOT(testDSN()));
	connect(driverList, SIGNAL(selectionChanged()), this, SLOT(enableDriverButtons()));
	connect(fileList, SIGNAL(selectionChanged()), this, SLOT(enableFileButtons()));
	connect(fileList, SIGNAL(doubleClicked(QListViewItem*)), this, SLOT(doubleClickedDSN(QListViewItem*)));
	connect(directoryEdit, SIGNAL(textChanged(const QString&)), this, SLOT(updateFileList(const QString&)));
	connect(directoryButton, SIGNAL(clicked()), this, SLOT(setFileDSNDirectory()));

	// Populate the user, system and driver lists...
	updateDSNList(ODBC_USER_DSN);
	updateDSNList(ODBC_SYSTEM_DSN);
	updateDriverList();

	// Populate the file list...
	char fileDSNPath[ODBC_FILEDSN_SIZE];
	char fileDSNName[ODBC_FILEDSN_SIZE];
	if (QFile::exists("/etc/unixODBC/ODBCDataSources/"))
		sprintf(fileDSNName, "/etc/unixODBC/ODBCDataSources/");
	else
		sprintf(fileDSNName, "/");
	SQLGetPrivateProfileString("ODBC", "FileDSNPath", fileDSNName, fileDSNPath, ODBC_FILEDSN_SIZE, "odbcinst.ini");
	directoryEdit->setText(fileDSNPath);

	// Populate debug settings...
	setDebugOptions();
}


////////////////////////////////////////////////////////////////////////////
// ADD THINGS SECTION

void ODBCDialog::addDriver()					// Add driver
{
	AddDriverDialog dialog(this);
	if (dialog.exec())
	{
		updateDriverList();
	}
}

void ODBCDialog::addDataSource()				// Add user, system or file DSN
{
	// Select Driver...
	SelectDriverDialog selectDialog(this);
	if (selectDialog.exec())
	{
		if (QObject::sender() == userAddButton)  // Add User DSN
		{
			DataSourceDialog dialog(this, 0, DSNMODE_ADD, DSNTYPE_USER, directoryEdit->text());
			if (dialog.create("", selectDialog.selectedDriver()))
			{
				if (dialog.doneInDriver() || dialog.exec())
					updateDSNList(ODBC_USER_DSN);
			}
		}
		else if (QObject::sender() == systemAddButton)		// Add System DSN
		{
			DataSourceDialog dialog(this, 0, DSNMODE_ADD, DSNTYPE_SYSTEM, directoryEdit->text());
			if (dialog.create("", selectDialog.selectedDriver()))
			{
				if (dialog.doneInDriver() || dialog.exec())
					updateDSNList(ODBC_SYSTEM_DSN);
			}
		}
		else									// Add File DSN
		{
			DataSourceDialog dialog(this, 0, DSNMODE_ADD, DSNTYPE_FILE, directoryEdit->text());
			if (dialog.create("", selectDialog.selectedDriver()))
			{
				if (dialog.exec())
					updateFileList(NULL);
			}
		}
	}
}

void ODBCDialog::autoSetupDrivers()
{
	if (AutoSetupDrivers() == TRUE)
		updateDriverList();
}


////////////////////////////////////////////////////////////////////////////
// VIEW SECTION

void ODBCDialog::viewDataSource()					// View a data source or driver configuration
{
	if (QObject::sender() == systemConfigureButton)		// system DSN
	{
		DataSourceDialog dialog(this, 0, DSNMODE_VIEW, DSNTYPE_SYSTEM, directoryEdit->text());
		if (dialog.create(systemList->selectedItem()->text(0), ""))
			dialog.exec();
	}
	else if (QObject::sender() == fileConfigureButton)											// file DSN
	{
		DataSourceDialog dialog(this, 0, DSNMODE_VIEW, DSNTYPE_FILE, directoryEdit->text());
		if (dialog.create(fileList->selectedItem()->text(0), ""))
			dialog.exec();
	}
	else									// ODBC Driver
	{
		DataSourceDialog dialog(this, 0, DSNMODE_VIEW, ODBC_DRIVER, directoryEdit->text());
		if (dialog.create("", driverList->selectedItem()->text(0)))
			dialog.exec();
	}
}


////////////////////////////////////////////////////////////////////////////
// CONFIGURE SECTION

void ODBCDialog::configureDataSource()					// Configure data surce
{
	if (QObject::sender() == userConfigureButton)		// User DSN
	{
		DataSourceDialog dialog(this, 0, DSNMODE_EDIT, DSNTYPE_USER, directoryEdit->text());
		if (dialog.create(userList->selectedItem()->text(0), ""))
		{
			if (dialog.doneInDriver() || dialog.exec())
				updateDSNList(ODBC_USER_DSN);
		}
	}
	else if (QObject::sender() == systemConfigureButton)	// System DSN
	{
		DataSourceDialog dialog(this, 0, DSNMODE_EDIT, DSNTYPE_SYSTEM, directoryEdit->text());
		if (dialog.create(systemList->selectedItem()->text(0), ""))
		{
			if (dialog.doneInDriver() || dialog.exec())
				updateDSNList(ODBC_SYSTEM_DSN);
		}
	}
	else if (QObject::sender() == fileConfigureButton)		// File DSN
	{
		DataSourceDialog dialog(this, 0, DSNMODE_EDIT, DSNTYPE_FILE, directoryEdit->text());
		if (dialog.create(fileList->selectedItem()->text(0), ""))
		{
			if (dialog.exec())
				updateFileList(NULL);
		}
	}
	else												// ODBC Driver
	{
		DataSourceDialog dialog(this, 0, DSNMODE_EDIT, ODBC_DRIVER, directoryEdit->text());
		if (dialog.create("", driverList->selectedItem()->text(0)))
		{
			if (dialog.exec())
				updateDriverList();
		}
	}
}

void ODBCDialog::doubleClickedDSN(QListViewItem*) 					// Configure/View DSN
{
	if (QObject::sender() == userList)			// User DSN
	{
		if (userList->selectedItem() != 0)
		{
			DataSourceDialog dialog(this, 0, DSNMODE_EDIT, DSNTYPE_USER, directoryEdit->text());
			if (dialog.create(userList->selectedItem()->text(0), ""))
			{
				if (dialog.doneInDriver() || dialog.exec())
					updateDSNList(ODBC_USER_DSN);
			}
		}
	}
	else if (QObject::sender() == systemList)	// System DSN
	{
		if (systemList->selectedItem() != 0)
		{
			if (getuid() > 0)
			{
				DataSourceDialog dialog(this, 0, DSNMODE_VIEW, DSNTYPE_SYSTEM, directoryEdit->text());
				if (dialog.create(systemList->selectedItem()->text(0), ""))
					dialog.exec();
			}
			else
			{
				DataSourceDialog dialog(this, 0, DSNMODE_EDIT, DSNTYPE_SYSTEM, directoryEdit->text());
				if (dialog.create(systemList->selectedItem()->text(0), ""))
				{
					if (dialog.doneInDriver() || dialog.exec())
						updateDSNList(ODBC_SYSTEM_DSN);
				}
			}
		}
	}
	else if (QObject::sender() == fileList)					// File DSN
	{
		if (fileList->selectedItem() != 0)
		{
			if (getuid() > 0)
			{
				DataSourceDialog dialog(this, 0, DSNMODE_VIEW, DSNTYPE_FILE, directoryEdit->text());
				if (dialog.create(fileList->selectedItem()->text(0), ""))
					dialog.exec();
			}
			else
			{
				DataSourceDialog dialog(this, 0, DSNMODE_EDIT, DSNTYPE_FILE, directoryEdit->text());
				if (dialog.create(fileList->selectedItem()->text(0), ""))
				{
					if (dialog.exec())
						updateFileList(NULL);
				}
			}
		}
	}
	else					// ODBC Driver
	{
		if (driverList->selectedItem() != 0)
		{
			if (getuid() > 0)
			{
				DataSourceDialog dialog(this, 0, DSNMODE_VIEW, ODBC_DRIVER, directoryEdit->text());
				if (dialog.create("", driverList->selectedItem()->text(0)))
					dialog.exec();
			}
			else
			{
				DataSourceDialog dialog(this, 0, DSNMODE_EDIT, ODBC_DRIVER, directoryEdit->text());
				if (dialog.create("", driverList->selectedItem()->text(0)))
				{
					if (dialog.exec())
						updateDriverList();
				}
			}
		}
	}
}


////////////////////////////////////////////////////////////////////////////
// UPDATE LISTS SECTION

void ODBCDialog::updateDriverList()				// Get driver list
{
	// Declare variables...
	char odbcResults[ODBC_BUFFER_SIZE];
	char odbcDescResults[ODBC_BUFFER_SIZE];
	QString odbcName;
	QListViewItem *currentItem = 0;
	int odbcResultsPos = 0;

	// Clear the list...
	driverList->clear();

	// Populate the list...
	if (SQLGetInstalledDrivers(odbcResults, ODBC_BUFFER_SIZE, 0) > 0)
	{
		odbcName = QString(odbcResults);
		while (!odbcName.isEmpty())
		{
			// Set the driver name and description in the QListView...
			if (SQLGetPrivateProfileString(odbcName.latin1(), "Description", "", odbcDescResults, ODBC_BUFFER_SIZE, "odbcinst") > 0)
			{
				currentItem = new QListViewItem(driverList, odbcName, QString(odbcDescResults));
			}
			else
			{
				currentItem = new QListViewItem(driverList, odbcName);
			}
			
			// Get the next driver name...
			odbcResultsPos = odbcResultsPos + odbcName.length() + 1;
			if (odbcResultsPos < ODBC_BUFFER_SIZE)
			{
				odbcName = QString(odbcResults + odbcResultsPos);
			}
			else
			{
				odbcName = "";
			}
		}
	}

	// Disable add data source buttons if there are no drivers...
	if (driverList->childCount() == 0)
	{
		userAddButton->setEnabled(FALSE);
		systemAddButton->setEnabled(FALSE);
	}
	else
	{
		userAddButton->setEnabled(TRUE);
		if (getuid() == 0)
			systemAddButton->setEnabled(TRUE);
	}

	// Enable/disable configure and remove buttons...
	enableDriverButtons();
}

void ODBCDialog::updateDSNList(int dsnType)		// Get DSN Lists...
{
	// Declare variables...
	char odbcResults[ODBC_BUFFER_SIZE];
	char odbcDescResults[ODBC_BUFFER_SIZE];
	char odbcDriverResults[ODBC_BUFFER_SIZE];
	QString odbcName;
	QListViewItem *currentItem = 0;
	int odbcResultsPos = 0;

	// Clear the list...
	if (dsnType == ODBC_USER_DSN)
		userList->clear();
	else
		systemList->clear();

	// Populate the list...
	SQLSetConfigMode(dsnType);
	if (SQLGetPrivateProfileString(0, 0, 0, odbcResults, ODBC_BUFFER_SIZE, 0) > 0)
	{
		odbcName = QString(odbcResults);
		while (!odbcName.isEmpty())
		{
			// Set the driver name and description in the QListView...
			SQLGetPrivateProfileString(odbcName.latin1(), "Description", "", odbcDescResults, ODBC_BUFFER_SIZE, 0);
			SQLGetPrivateProfileString(odbcName.latin1(), "Driver", "", odbcDriverResults, ODBC_BUFFER_SIZE, 0);
			if (dsnType == ODBC_USER_DSN)
			{
				currentItem = new QListViewItem(userList, odbcName, QString(odbcDescResults), QString(odbcDriverResults));
			}
			else
			{
				currentItem = new QListViewItem(systemList, odbcName, QString(odbcDescResults), QString(odbcDriverResults));
			}

			// Get the next driver name...
			odbcResultsPos = odbcResultsPos + odbcName.length() + 1;
			if (odbcResultsPos < ODBC_BUFFER_SIZE)
			{
				odbcName = QString(odbcResults + odbcResultsPos);
			}
			else
			{
				odbcName = "";
			}
		}
	}
	SQLSetConfigMode(ODBC_BOTH_DSN);

	// Enable/disable configure and remove buttons
	if (dsnType == ODBC_USER_DSN)
	{
		enableUserButtons();
	}
	else
	{
		enableSystemButtons();
	}
}

void ODBCDialog::updateFileList(const QString&)				// Update File DSN list
{
	// Declare Variables...
	QListViewItem *currentItem = 0;
	QDir directory;
	char descriptionString[ODBC_BUFFER_SIZE + 1];
	char driverString[ODBC_BUFFER_SIZE + 1];
	//QString filePermissions;
	//QString fileSize;

	// Clear file list
	fileList->clear();

	// Configure settings...
	directory.setFilter(QDir::Files | QDir::Hidden);
	directory.setNameFilter("*.dsn");
	directory.cd(directoryEdit->text());
	const QFileInfoList *directoryFileList = directory.entryInfoList();
	QFileInfoListIterator it(*directoryFileList);
	QFileInfo *directoryFile;

	while ((directoryFile=it.current()))			// Populate the file list
	{
		if (SQLReadFileDSN(directoryFile->fileName().latin1(), "ODBC", "Description", descriptionString, ODBC_BUFFER_SIZE, NULL) == 0 || strlen(descriptionString) == 0)
			descriptionString[0] = 0;
		if (SQLReadFileDSN(directoryFile->fileName().latin1(), "ODBC", "Driver", driverString, ODBC_BUFFER_SIZE, NULL) == 0 || strlen(driverString) == 0)
			driverString[0] = 0;

		currentItem = new QListViewItem(fileList, directoryFile->baseName(), QString(descriptionString), QString(driverString));
		++it; 
	}

	// Enable/disable buttons
	enableFileButtons();
}


////////////////////////////////////////////////////////////////////////////
// REMOVE THINGS SECTION

void ODBCDialog::removeDriver()			// Remove a driver
{
	// Variables...
	QListViewItem *driverItemToRemove;
	QString driverToRemove;
	QString tempString;
	DWORD usageCount;
	bool removeDataSource = FALSE;

	// Get driver name...
	driverItemToRemove = driverList->selectedItem();
	if (driverItemToRemove != 0)
	{
		driverToRemove = driverItemToRemove->text(0);
		tempString = QString(tr("Are you sure you want to remove the '%1' ODBC driver?")).arg(driverToRemove);
		if (QMessageBox::question(this, tr("Remove ODBC Driver"), tempString, QMessageBox::Yes, QMessageBox::No, QMessageBox::NoButton) == QMessageBox::Yes)
		{
			// Delete data sources too?
			if (QMessageBox::question(this, tr("Remove ODBC Driver"), tr("Remove data sources that use this driver?"), QMessageBox::Yes, QMessageBox::No, QMessageBox::NoButton) == QMessageBox::Yes)
				removeDataSource = TRUE;

			// Delete driver...
			if (SQLRemoveDriver(driverToRemove.latin1(), removeDataSource, &usageCount) == FALSE)
			{
				QMessageBox::warning(this, tr("Remove ODBC Driver"), tr("Could not remove the driver."), QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
			}
			else
			{
				updateDriverList();
				if (removeDataSource == TRUE)
				{
					updateDSNList(ODBC_USER_DSN);
					updateDSNList(ODBC_SYSTEM_DSN);
				}
			}
		}
	}
}

void ODBCDialog::removeDSN()				// Remove a DSN
{
	// Variables...
	QListViewItem *datasourceItemToRemove;
	QString datasourceToRemove;
	QString tempString;
	int dsnType;

	// Was it for user or system DSN lists?
	if (QObject::sender() == userRemoveButton)
	{
		dsnType = ODBC_USER_DSN;
		datasourceItemToRemove = userList->selectedItem();
	}
	else
	{
		dsnType = ODBC_SYSTEM_DSN;
		datasourceItemToRemove = systemList->selectedItem();
	}
	SQLSetConfigMode(dsnType);

	// Remove DSN...
	if (datasourceItemToRemove != 0)
	{
		datasourceToRemove = datasourceItemToRemove->text(0);
		tempString = QString(tr("Are you sure you want to remove the\n'%1' data source?")).arg(datasourceToRemove);
		if (QMessageBox::question(this, tr("Remove Data Source"), tempString, QMessageBox::Yes, QMessageBox::No, QMessageBox::NoButton) == QMessageBox::Yes)
		{
			// Delete data Source...
			if (SQLRemoveDSNFromIni(datasourceToRemove.latin1()) == FALSE)
			{
				QMessageBox::warning(this, tr("Remove Data Source"), tr("Could not remove the data source."), QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
			}
			else
			{
				updateDSNList(dsnType);
			}
		}
	}
	SQLSetConfigMode(ODBC_BOTH_DSN);
}

void ODBCDialog::removeFileDSN()				// Remove file DSN
{
	// Variables...
	QListViewItem *fileDSNToRemove;
	QString fileDSNName;
	QString tempString;

	// Get selected fileDSN and confirm...
	fileDSNToRemove = fileList->selectedItem();
	fileDSNName = fileDSNToRemove->text(0);
	tempString = QString(tr("Are you sure you want to remove the\n'%1' file data source? It will be deleted.")).arg(fileDSNName);
	if (QMessageBox::question(this, tr("Remove File Data Source"), tempString, QMessageBox::Yes, QMessageBox::No, QMessageBox::NoButton) == QMessageBox::Yes)
	{
		fileDSNName.prepend(directoryEdit->text());
		if (unlink(fileDSNName.append(".dsn")))
		{
			QMessageBox::warning(this, tr("Remove File Data Source"), tr("Could not remove the file data source."), QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
		}
		else
		{
			updateFileList(NULL);
		}
	}
}


////////////////////////////////////////////////////////////////////////////
// DEBUG OPTIONS SECTION

void ODBCDialog::selectTraceFile()				// Select a Debug Tracing File
{
	QString tempDir;
	tempDir = QFileDialog::getSaveFileName(traceFileEdit->text(), QString::null, this, NULL , "Save tracing file as", 0, TRUE);
	if (tempDir.isNull() == FALSE)
		traceFileEdit->setText(tempDir);
}

void ODBCDialog::tracingEnable(bool checkStatus)			// Tracing Check State Changed
{
	forceTracingCheck->setEnabled(checkStatus);
	traceFileEdit->setEnabled(checkStatus);
	traceFileButton->setEnabled(checkStatus);
}

void ODBCDialog::saveDebugSettings()			// Save Debug Options
{
	// Variables...
	char debugSetting[ODBC_FILEDSN_SIZE + 1];
	bool saveDebugSettings = TRUE;
	bool saveFailed = FALSE;

	// If debug is going to be switched on, warn the user...
	if (tracingCheck->isChecked())
	{
		if (QMessageBox::warning(this, tr("Turn on tracing"), tr("You are about to turn of tracing. Tracing\ncan use a lot of disk space. Are you sure\nyou want to turn on tracing?"), QMessageBox::Yes, QMessageBox::No, QMessageBox::NoButton) == QMessageBox::No)
			saveDebugSettings = FALSE;
	}

	// Save the debug settings...
	if (saveDebugSettings == TRUE)
	{
		if (tracingCheck->isChecked())
			strcpy(debugSetting, "Yes");
		else
			strcpy(debugSetting, "No ");

		if (SQLWritePrivateProfileString( "ODBC", "Trace", debugSetting, "odbcinst.ini" ))
		{
			if (forceTracingCheck->isChecked())
				strcpy(debugSetting, "Yes");
			else
				strcpy(debugSetting, "No ");

			if (SQLWritePrivateProfileString( "ODBC", "ForceTrace", debugSetting, "odbcinst.ini" ))
			{
				if (connectionPoolingCheck->isChecked())
					strcpy(debugSetting, "Yes");
				else
					strcpy(debugSetting, "No ");

				if (SQLWritePrivateProfileString( "ODBC", "Pooling", debugSetting, "odbcinst.ini" ))
				{
					if (!SQLWritePrivateProfileString( "ODBC", "Pooling", directoryEdit->text().ascii(), "odbcinst.ini" ))
						saveFailed = TRUE;
				}
				else
					saveFailed = TRUE;
				}
			else
				saveFailed = TRUE;
			}
		else
			saveFailed = TRUE;
	}

	// If save has failed, give a warning
	if (saveFailed == TRUE)
	{
		QMessageBox::critical(this, tr("Turn on tracing"), tr("Unable to save debug options."), QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
		setDebugOptions();
	}
}

void ODBCDialog::turnOffDebug()					// Switch all the debug stuff off
{
	tracingCheck->setChecked(FALSE);
	traceFileEdit->setText("/tmp/sql.log");
	connectionPoolingCheck->setChecked(FALSE);
	saveDebugSettings();
}

void ODBCDialog::setDebugOptions()					// Populate the debg options
{
	char debugSetting[ODBC_FILEDSN_SIZE + 1];
	SQLGetPrivateProfileString( "ODBC", "Trace", "No", debugSetting, ODBC_FILEDSN_SIZE, "odbcinst.ini" );
	if ((debugSetting[0] == 0) || (toupper(debugSetting[0]) == 'Y'))
		tracingCheck->setChecked(TRUE);
	else
		tracingCheck->setChecked(FALSE);
	SQLGetPrivateProfileString( "ODBC", "ForceTrace", "No", debugSetting, ODBC_FILEDSN_SIZE, "odbcinst.ini" );
	if ((debugSetting[0] == 0) || (toupper(debugSetting[0]) == 'Y'))
		forceTracingCheck->setChecked(TRUE);
	else
		forceTracingCheck->setChecked(FALSE);
	SQLGetPrivateProfileString( "ODBC", "TraceFile", "/tmp/sql.log", debugSetting, ODBC_FILEDSN_SIZE, "odbcinst.ini" );
	traceFileEdit->setText(debugSetting);
	SQLGetPrivateProfileString( "ODBC", "Pooling", "No", debugSetting, ODBC_FILEDSN_SIZE, "odbcinst.ini" );
	if ((debugSetting[0] == 0) || (toupper(debugSetting[0]) == 'Y'))
		connectionPoolingCheck->setChecked(TRUE);
	else
		connectionPoolingCheck->setChecked(FALSE);
}


////////////////////////////////////////////////////////////////////////////
// TEST DSN SECTION

void ODBCDialog::testDSN()
{
	// Variables...
	SQLHENV odbcEnvironmentHandle;
	long odbcFunctionResult = 0;
	SQLHDBC odbcConnectionHandle;
	QString odbcDataSource = "";
	QString odbcUser = "";
	QString odbcPassword = "";
	QListViewItem *tempDSN;
	char tempString[ODBC_BUFFER_SIZE + 1];
	bool requestUserPass = FALSE;

	// Get ODBC connection details...
	if (QObject::sender() == userTestButton)
	{
		tempDSN = userList->selectedItem();
		odbcDataSource = tempDSN->text(0);
		SQLSetConfigMode(ODBC_USER_DSN);
	}
	else if (QObject::sender() == systemTestButton)
	{
		tempDSN = systemList->selectedItem();
		odbcDataSource = tempDSN->text(0);
		SQLSetConfigMode(ODBC_SYSTEM_DSN);
	}

	// Check to see if username has been configured...
	if (SQLGetPrivateProfileString(odbcDataSource.latin1(), "Username", "", tempString, ODBC_BUFFER_SIZE, 0) == 0 || strlen(tempString) == 0)
	{
		if (SQLGetPrivateProfileString(odbcDataSource.latin1(), "UID", "", tempString, ODBC_BUFFER_SIZE, 0) == 0 || strlen(tempString) == 0)
		{
			if (SQLGetPrivateProfileString(odbcDataSource.latin1(), "User", "", tempString, ODBC_BUFFER_SIZE, 0) == 0 || strlen(tempString) == 0)
			{
				requestUserPass = TRUE;
			}
		}
	}
	odbcUser = tempString;

	// Check to see if password has been configured...
	if (SQLGetPrivateProfileString(odbcDataSource.latin1(), "Password", "", tempString, ODBC_BUFFER_SIZE, 0) == 0 || strlen(tempString) == 0)
	{
		if (SQLGetPrivateProfileString(odbcDataSource.latin1(), "PWD", "", tempString, ODBC_BUFFER_SIZE, 0) == 0 || strlen(tempString) == 0)
		{
			if (SQLGetPrivateProfileString(odbcDataSource.latin1(), "Pass", "", tempString, ODBC_BUFFER_SIZE, 0) == 0 || strlen(tempString) == 0)
			{
				requestUserPass = TRUE;
			}
		}
	}
	odbcPassword = tempString;

	SQLSetConfigMode(ODBC_BOTH_DSN);

	// Request Username / Password if required...
	if (requestUserPass == TRUE)
	{
		qt_loginDialog dialog(this);
		if (odbcUser.isEmpty())
			dialog.usernameEdit->clear();
		else
			dialog.usernameEdit->setText(odbcUser);
		if (odbcPassword.isEmpty())
			dialog.passwordEdit->clear();
		else
			dialog.passwordEdit->setText(odbcPassword);
		if (dialog.exec())
		{
			odbcUser = dialog.usernameEdit->text();
			odbcPassword = dialog.passwordEdit->text();
		}
	}

	// Test ODBC connection...
	odbcFunctionResult = SQLAllocEnv(&odbcEnvironmentHandle);
	if ((odbcFunctionResult == SQL_SUCCESS) || (odbcFunctionResult == SQL_SUCCESS_WITH_INFO))
	{
		odbcFunctionResult = SQLAllocConnect(odbcEnvironmentHandle, &odbcConnectionHandle);
		if ((odbcFunctionResult == SQL_SUCCESS) || (odbcFunctionResult == SQL_SUCCESS_WITH_INFO))
		{
			odbcFunctionResult = SQLConnect(odbcConnectionHandle, (SQLCHAR*)odbcDataSource.ascii(), SQL_NTS, (SQLCHAR*)odbcUser.ascii(), SQL_NTS, (SQLCHAR*)odbcPassword.ascii(), SQL_NTS);
			if ((odbcFunctionResult == SQL_SUCCESS) || (odbcFunctionResult == SQL_SUCCESS_WITH_INFO))
			{
				QMessageBox::information(this, tr("Data Source Test"), tr("Successfully connected!"), QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
				SQLDisconnect(odbcConnectionHandle);
			}
			else
			{
				QMessageBox::warning(this, tr("Data Source Test"), tr("Failed to connect to data source."), QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
			}
			SQLFreeHandle(SQL_HANDLE_DBC, odbcConnectionHandle);
		}
		else
		{
			QMessageBox::warning(this, tr("Data Source Test"), tr("Failed to allocate connection."), QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
		}
		SQLFreeHandle(SQL_HANDLE_ENV, odbcEnvironmentHandle);
	}
	else
	{
		QMessageBox::warning(this, tr("Data Source Test"), tr("Failed to allocate environment."), QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
	}
}


////////////////////////////////////////////////////////////////////////////
// MISC THINGS SECTION

void ODBCDialog::about()						// About the progem
{
	qt_aboutDialog dialog(this);
	dialog.exec();
}

void ODBCDialog::enableDriverButtons()			// Enable / disable driver buttons
{
	if (driverList->selectedItem() == 0)
	{
		driverConfigureButton->setEnabled(FALSE);
		driverRemoveButton->setEnabled(FALSE);
	}
	else
	{
		driverConfigureButton->setEnabled(TRUE);
		if (getuid() == 0)
			driverRemoveButton->setEnabled(TRUE);
	}
}

void ODBCDialog::enableSystemButtons()			// Enable / disable system DSN buttons
{
	if (systemList->selectedItem() == 0)
	{
		systemConfigureButton->setEnabled(FALSE);
		systemRemoveButton->setEnabled(FALSE);
		systemTestButton->setEnabled(FALSE);
	}
	else
	{
		systemConfigureButton->setEnabled(TRUE);
		systemTestButton->setEnabled(TRUE);
		if (getuid() == 0)
			systemRemoveButton->setEnabled(TRUE);
	}
}

void ODBCDialog::enableUserButtons()			// Enable / disable user DSN buttons
{
	if (userList->selectedItem() == 0)
	{
		userConfigureButton->setEnabled(FALSE);
		userRemoveButton->setEnabled(FALSE);
		userTestButton->setEnabled(FALSE);
	}
	else
	{
		userConfigureButton->setEnabled(TRUE);
		userRemoveButton->setEnabled(TRUE);
		userTestButton->setEnabled(TRUE);
	}
}

void ODBCDialog::enableFileButtons()			// Enable / disable file DSN buttons
{
	// Enable / disable file dsn buttons...
	if (getuid() == 0)
	{
		if (fileList->selectedItem() == 0)
		{
			fileConfigureButton->setEnabled(FALSE);
			fileRemoveButton->setEnabled(FALSE);
		}
		else
		{
			fileConfigureButton->setEnabled(TRUE);
			fileRemoveButton->setEnabled(TRUE);
		}
	}
	else
	{
		if (fileList->selectedItem() == 0)
		{
			fileConfigureButton->setEnabled(FALSE);
		}
		else
		{
			fileConfigureButton->setEnabled(TRUE);
		}
	}
}

void ODBCDialog::setFileDSNDirectory()			// Set a new file DSN directory
{
	QString tempDir;
	tempDir = QFileDialog::getExistingDirectory(directoryEdit->text(), this, NULL , "Select File DSN Directory", TRUE);
	if (tempDir.isNull() == FALSE)
		directoryEdit->setText(tempDir);
}

void ODBCDialog::defaultFileDSN()				// Set default file DSN
{
	SQLWritePrivateProfileString("ODBC", "FileDSNPath", (char*)directoryEdit->text().ascii(), "odbcinst.ini");
	QMessageBox::information(this, tr("Set Default File Data Source Directory"), QString(tr("Default file data source directory has been set to\n'%1'.")).arg(directoryEdit->text()), QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
}

