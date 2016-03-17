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


// This routine has been designed to add/modify/view user, system and file data sources. 
// Plus view/modify ODBC drivers.

#include "datasourcedialog.h"


////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR / DESTRUCTOR SECTION

DataSourceDialog::DataSourceDialog(QWidget *parent, const char *name, int dialogMode, int dsnType, QString fileDir) : qt_dataSourceDialog(parent,name)
{
	// Variables...
	QString windowCaption;

	// Initialise some globals...
	globalDialogMode = dialogMode;
	globalDSNType = dsnType;
	fileDSNDir = fileDir;
	dataSourceProperties = NULL;
	dataSourcePointer = NULL;
	label = NULL;
	comboBox = NULL;
	lineEdit = NULL;
	spacer = NULL;
	tabPointer = NULL;
	fileWidget = NULL;
	generalLoop = 1;
	advancedLoop = 1;
	loggingLoop = 1;
	configInDriver = false;

	// Configure dialog caption...
	switch (dialogMode)
	{
		case DSNMODE_ADD:
			windowCaption = "Add ";
			connect(saveButton, SIGNAL(clicked()), this, SLOT(saveDataSource()));
			break;
		case DSNMODE_EDIT:
			windowCaption = "Configure ";
			connect(saveButton, SIGNAL(clicked()), this, SLOT(saveDataSource()));
			break;
		default:
			windowCaption = "View ";
			cancelButton->setText("Close");
			saveButton->setHidden(TRUE);
			break;
	}
	switch (dsnType)
	{
		case DSNTYPE_USER:
			windowCaption.append("User Data Source");
			break;
		case DSNTYPE_SYSTEM:
			windowCaption.append("System Data Source");
			break;
		case DSNTYPE_FILE:
			windowCaption.append("File Data Source");
			break;
		default:
			windowCaption.append("ODBC Driver");
			break;
	}
	setCaption(tr(windowCaption));

	// Connect SIGNAL / SLOTS...
	connect(expertCheck, SIGNAL(clicked()), this, SLOT(setExpertOptions()));
}

DataSourceDialog::~DataSourceDialog()
{
	ODBCINSTDestructProperties(&dataSourceProperties);
}

bool DataSourceDialog::doneInDriver()
{
	return configInDriver;
}


////////////////////////////////////////////////////////////////////////////
// BUILD DIALOG SECTION

bool DataSourceDialog::create(QString dataSource, QString driver)
{
	// Variables...
	char driverName[ODBC_BUFFER_SIZE + 1];
	char dataSourceResults[ODBC_BUFFER_SIZE + 1];
	bool everythingOk = TRUE;
	bool viewMode = TRUE;
	int loop = 0;
	QString settingName;
	int setupMode = 0;

	configInDriver = false;

	// Set variables...
	if (globalDSNType == ODBC_DRIVER)
		dataSourceName = driver;
	else
		dataSourceName = dataSource;
	strncpy(driverName, driver.latin1(), ODBC_BUFFER_SIZE);
	if (globalDialogMode == DSNMODE_VIEW)
		viewMode = TRUE;
	else
		viewMode = FALSE;

	// Set config mode...
	if (globalDSNType == DSNTYPE_USER)
	{
		SQLSetConfigMode(ODBC_USER_DSN);
		switch ( globalDialogMode )
		{
			case DSNMODE_ADD:
				setupMode = ODBC_ADD_DSN;
				break;

			case DSNMODE_EDIT:
				setupMode = ODBC_CONFIG_DSN;
				break;
		}
	}
	else if (globalDSNType == DSNTYPE_SYSTEM)
	{
		SQLSetConfigMode(ODBC_SYSTEM_DSN);
		switch ( globalDialogMode )
		{
			case DSNMODE_ADD:
				setupMode = ODBC_ADD_SYS_DSN;
				break;

			case DSNMODE_EDIT:
				setupMode = ODBC_CONFIG_SYS_DSN;
				break;
		}
	}

	// If edit / view mode, get the driver name...
	if (globalDialogMode != DSNMODE_ADD)
	{
		if ((globalDSNType == DSNTYPE_USER) || (globalDSNType == DSNTYPE_SYSTEM))  // if it is a user or system dsn...
		{
			if (SQLGetPrivateProfileString(dataSourceName.latin1(), "Driver", "", driverName, ODBC_BUFFER_SIZE, 0) == 0 || strlen(driverName) == 0)
			{
				everythingOk = FALSE;
				QMessageBox::critical(this, tr("Data Source Configuration"), tr("Could not read the data source name. Confirm\nthat the data source exists."), QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
			}
		}
		else if (globalDSNType == DSNTYPE_FILE)		// it is a file DSN...
		{
			if (SQLReadFileDSN(dataSourceName.latin1(), "ODBC", "DRIVER", driverName, ODBC_BUFFER_SIZE, NULL) == 0 || strlen(driverName) == 0)
			{
				everythingOk = FALSE;
				QMessageBox::critical(this, tr("Data Source Configuration"), tr("Could not read the data source name. Confirm\nthat the data source exists."), QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
			}
		}
	}

	// can we call SQLConfigDataSource ?
	char attr[ 1024 ];
	memset( attr, 0, sizeof( attr ));
	sprintf( attr, "DSN=%s", dataSourceName.latin1());
	if ( SQLConfigDataSource(( HWND ) 1, setupMode, driverName, attr )) 
	{
		configInDriver = true;
		return true;
	}

	// Build the driver properties...
	if (everythingOk == TRUE)
	{
		// Get list of driver properties...
		if (globalDSNType != ODBC_DRIVER)
		{
			if (ODBCINSTConstructProperties(driverName, &dataSourceProperties) != ODBCINST_SUCCESS)
			{
				if (globalDialogMode != DSNMODE_ADD)
				{
					dataSourceProperties  = (HODBCINSTPROPERTY)malloc(sizeof(ODBCINSTPROPERTY));
					memset(dataSourceProperties, 0, sizeof(ODBCINSTPROPERTY));
					dataSourceProperties->nPromptType = ODBCINST_PROMPTTYPE_TEXTEDIT;
					dataSourceProperties->pNext = NULL;
					dataSourceProperties->bRefresh = 0;
					dataSourceProperties->hDLL = NULL;
					dataSourceProperties->pWidget = NULL;
					dataSourceProperties->pszHelp = strdup(tr("<b>Data Source Name</b><br><br>The name of the data source (DSN)."));
					dataSourceProperties->aPromptData = NULL;
					strncpy(dataSourceProperties->szName, "Name", INI_MAX_PROPERTY_NAME);
					strcpy(dataSourceProperties->szValue, dataSource.ascii());
				}
				else
				{
					QMessageBox::critical(this, tr("Data Source Configuration"), tr("Could not construct driver properties. Has a\ndriver setup library been configured?\n\nIf no setup library exists, you can configure\nthe driver to use one of the standard setup\nlibraries. The setup library 'libodbcdrvcfg1S.so'\nis for server based databases and\n'libodbcdrvcfg2S.so' is for file based\ndatabases."), QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
					everythingOk = FALSE;
				}
			}
		}
		else		// driver edit / view, construct properties manually...
		{
			dataSourceProperties  = (HODBCINSTPROPERTY)malloc(sizeof(ODBCINSTPROPERTY));
			memset(dataSourceProperties, 0, sizeof(ODBCINSTPROPERTY));
			dataSourceProperties->nPromptType = ODBCINST_PROMPTTYPE_TEXTEDIT;
			dataSourceProperties->pNext = NULL;
			dataSourceProperties->bRefresh = 0;
			dataSourceProperties->hDLL = NULL;
			dataSourceProperties->pWidget = NULL;
			dataSourceProperties->pszHelp = strdup(tr("<b>ODBC Driver Name</b><br><br>The name of the ODBC driver."));
			dataSourceProperties->aPromptData = NULL;
			strncpy(dataSourceProperties->szName, "Name", INI_MAX_PROPERTY_NAME);
			strcpy(dataSourceProperties->szValue, driver.ascii());

			dataSourceProperties->pNext = (HODBCINSTPROPERTY)malloc(sizeof(ODBCINSTPROPERTY));
			dataSourcePointer = dataSourceProperties->pNext;
			memset(dataSourcePointer, 0, sizeof(ODBCINSTPROPERTY));
			dataSourcePointer->nPromptType = ODBCINST_PROMPTTYPE_TEXTEDIT;
			dataSourcePointer->pNext = NULL;
			dataSourcePointer->bRefresh = 0;
			dataSourcePointer->hDLL = NULL;
			dataSourcePointer->pWidget = NULL;
			dataSourcePointer->pszHelp = strdup(tr("<b>ODBC Driver Description</b><br><br>A description of the ODBC driver."));
			dataSourcePointer->aPromptData = NULL;
			strncpy(dataSourcePointer->szName, "Description", INI_MAX_PROPERTY_NAME);
			strcpy(dataSourcePointer->szValue, "");

			dataSourcePointer->pNext = (HODBCINSTPROPERTY)malloc(sizeof(ODBCINSTPROPERTY));
			dataSourcePointer = dataSourcePointer->pNext;
			memset( dataSourcePointer, 0, sizeof(ODBCINSTPROPERTY));
			dataSourcePointer->nPromptType = ODBCINST_PROMPTTYPE_FILENAME;
			dataSourcePointer->pszHelp = strdup(tr("<b>ODBC Driver Library</b><br><br>This is the full path to the ODBC Driver Library. Libraries are named with a .so file extension and are typically stored in /usr/lib or /usr/local/lib. You must specify a ODBC driver library."));
			strncpy( dataSourcePointer->szName, "Driver", INI_MAX_PROPERTY_NAME);
			strncpy( dataSourcePointer->szValue, "", INI_MAX_PROPERTY_VALUE);

			dataSourcePointer->pNext = (HODBCINSTPROPERTY)malloc(sizeof(ODBCINSTPROPERTY));
			dataSourcePointer = dataSourcePointer->pNext;
			memset( dataSourcePointer, 0, sizeof(ODBCINSTPROPERTY));
			dataSourcePointer->nPromptType = ODBCINST_PROMPTTYPE_FILENAME;
			dataSourcePointer->pszHelp = strdup(tr("<b>ODBC Driver Library (64 bit)</b><br><br>This is the full path to the ODBC Driver Library. Libraries are named with a .so file extension and are typically stored in /usr/lib or /usr/local/lib. You must specify a ODBC driver library."));
			strncpy( dataSourcePointer->szName, "Driver64", INI_MAX_PROPERTY_NAME);
			strncpy( dataSourcePointer->szValue, "", INI_MAX_PROPERTY_VALUE);

			dataSourcePointer->pNext = (HODBCINSTPROPERTY)malloc(sizeof(ODBCINSTPROPERTY));
			dataSourcePointer = dataSourcePointer->pNext;
			memset( dataSourcePointer, 0, sizeof(ODBCINSTPROPERTY));
			dataSourcePointer->nPromptType = ODBCINST_PROMPTTYPE_FILENAME;
			dataSourcePointer->pszHelp = strdup(tr("<b>ODBC Driver Setup Library</b><br><br>This is the full path to an ODBC driver setup library for the ODBC driver. ODBC driver setup libraries are used to help configure ODBC data sources.<br><br>If the ODBC driver does not have a setup library or one is not provided with unixODBC specifically for the library, you can use a standard one. There are two standard driver setup libraries; a file based setup library (libodbcdrvcfg2S.so) and a server based setup library (libodbcdrvcfg1S.so)."));
			strncpy( dataSourcePointer->szName, "Setup", INI_MAX_PROPERTY_NAME);
			strncpy( dataSourcePointer->szValue, "", INI_MAX_PROPERTY_VALUE);

			dataSourcePointer->pNext = (HODBCINSTPROPERTY)malloc(sizeof(ODBCINSTPROPERTY));
			dataSourcePointer = dataSourcePointer->pNext;
			memset( dataSourcePointer, 0, sizeof(ODBCINSTPROPERTY));
			dataSourcePointer->nPromptType = ODBCINST_PROMPTTYPE_FILENAME;
			dataSourcePointer->pszHelp = strdup(tr("<b>ODBC Driver Setup Library (64 bit)</b><br><br>This is the full path to an ODBC driver setup library for the ODBC driver. ODBC driver setup libraries are used to help configure ODBC data sources.<br><br>If the ODBC driver does not have a setup library or one is not provided with unixODBC specifically for the library, you can use a standard one. There are two standard driver setup libraries; a file based setup library (libodbcdrvcfg2S.so) and a server based setup library (libodbcdrvcfg1S.so)."));
			strncpy( dataSourcePointer->szName, "Setup64", INI_MAX_PROPERTY_NAME);
			strncpy( dataSourcePointer->szValue, "", INI_MAX_PROPERTY_VALUE);

			dataSourcePointer->pNext = (HODBCINSTPROPERTY)malloc(sizeof(ODBCINSTPROPERTY));
			dataSourcePointer = dataSourcePointer->pNext;
			memset(dataSourcePointer, 0, sizeof(ODBCINSTPROPERTY));
			dataSourcePointer->nPromptType = ODBCINST_PROMPTTYPE_TEXTEDIT;
			dataSourcePointer->pNext = NULL;
			dataSourcePointer->bRefresh = 0;
			dataSourcePointer->hDLL = NULL;
			dataSourcePointer->pWidget = NULL;
			dataSourcePointer->pszHelp = strdup(tr("<b>Connection Pool Timeout</b><br><br>Number of seconds before a connection timesout when in a Connection Pool. Leave this value blank to disable Connection Pooling."));
			dataSourcePointer->aPromptData = NULL;
			strncpy( dataSourcePointer->szName, "CPTimeout", INI_MAX_PROPERTY_NAME);

			dataSourcePointer->pNext = (HODBCINSTPROPERTY)malloc(sizeof(ODBCINSTPROPERTY));
			dataSourcePointer = dataSourcePointer->pNext;
			memset(dataSourcePointer, 0, sizeof(ODBCINSTPROPERTY));
			dataSourcePointer->nPromptType = ODBCINST_PROMPTTYPE_TEXTEDIT;
			dataSourcePointer->pNext = NULL;
			dataSourcePointer->bRefresh = 0;
			dataSourcePointer->hDLL = NULL;
			dataSourcePointer->pWidget = NULL;
			dataSourcePointer->pszHelp = strdup(tr("<b>Connection Pool Reuse</b><br><br>The maximum number of times a connection can be reused in a Connection Pool. Set to a lower number when dealing with drivers which have stability issues or memory leaks."));
			dataSourcePointer->aPromptData = NULL;
			strncpy(dataSourcePointer->szName, "CPReuse", INI_MAX_PROPERTY_NAME);
		}
	}

	// Build dialog based on driver setup / driver properties
	if (everythingOk == TRUE)
	{

		// Add any aditional properties if this is view / edit...
		if ((globalDialogMode != DSNMODE_ADD) && (globalDSNType != DSNTYPE_FILE))
		{
			// Variables...
			char odbcResults[ODBC_BUFFER_SIZE];
			HODBCINSTPROPERTY lastDataSourcePointer = NULL;
			bool propertyExists = FALSE;
			QString odbcName = "";
			QString odbcTemp = "";
			QString odbcTemp2 = "";
			int odbcResultsPos = 0;
			int getResult = 0;
			memset(odbcResults, 0, ODBC_BUFFER_SIZE);

			// Get a full list of configured properties...
			if (globalDSNType != ODBC_DRIVER)
				getResult = SQLGetPrivateProfileString(dataSourceName.latin1(), 0, 0, odbcResults, ODBC_BUFFER_SIZE, 0);
			else
				getResult = SQLGetPrivateProfileString(dataSourceName.latin1(), 0, 0, odbcResults, ODBC_BUFFER_SIZE, "odbcinst");

			if (getResult > 0)
			{
				// Get a pointer to the last set of properties...
				for (dataSourcePointer = dataSourceProperties; dataSourcePointer != NULL; dataSourcePointer = dataSourcePointer->pNext)
				{
					lastDataSourcePointer = dataSourcePointer;
				}
	
				// Check to see it property exists...
				odbcName = QString(odbcResults);
				odbcResultsPos = 0;
				while (!odbcName.isEmpty())
				{
					propertyExists = FALSE;
					odbcTemp = odbcName.upper();
					for (dataSourcePointer = dataSourceProperties; ((dataSourcePointer != NULL) && (propertyExists == FALSE)); dataSourcePointer = dataSourcePointer->pNext)
					{
						odbcTemp2 = dataSourcePointer->szName;
						if (odbcTemp.compare(odbcTemp2.upper()) == 0)
							propertyExists = TRUE;
					}

					// Add property...
					if ((propertyExists == FALSE) && (odbcName.compare("UsageCount") != 0))
					{
						lastDataSourcePointer->pNext = (HODBCINSTPROPERTY)malloc(sizeof(ODBCINSTPROPERTY));
						lastDataSourcePointer = lastDataSourcePointer->pNext;
						memset(lastDataSourcePointer, 0, sizeof(ODBCINSTPROPERTY));
						lastDataSourcePointer->nPromptType = ODBCINST_PROMPTTYPE_TEXTEDIT;
						lastDataSourcePointer->pNext = NULL;
						lastDataSourcePointer->bRefresh = 0;
						lastDataSourcePointer->hDLL = NULL;
						lastDataSourcePointer->pWidget = NULL;
						lastDataSourcePointer->pszHelp = strdup(tr("No help for this option, read the driver documentation"));
						lastDataSourcePointer->aPromptData = NULL;
						strncpy(lastDataSourcePointer->szName, odbcName.latin1(), INI_MAX_PROPERTY_NAME);
						strcpy(lastDataSourcePointer->szValue, "");
					}

					// Get the next property name...
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
		}

		for (dataSourcePointer = dataSourceProperties; dataSourcePointer != NULL; dataSourcePointer = dataSourcePointer->pNext)
		{
			// Place property on correct tab...
			settingName = dataSourcePointer->szName;
			if (settingName.upper() == "TRACE")			// Logging options
			{
				settingName.append(":");
				tabPointer = loggingTab;
				loop = loggingLoop;
			}
			else if (settingName.upper() == "TRACEFILE")
			{
				settingName = "Trace File:";
				tabPointer = loggingTab;
				loop = loggingLoop;
			}
														// Settings
			else if (settingName.upper() == "NAME" || settingName.upper() == "DESCRIPTION" || settingName.upper() == "DRIVER" || settingName.upper() == "SETUP" || settingName.upper() == "DIRECTORY")
			{
				settingName.append(":");
				tabPointer = generalTab;
				loop = generalLoop;
			}
			else if (settingName.upper() == "SERVER" || settingName.upper() == "SERVERNAME" || settingName.upper() == "HOST")
			{
				settingName = "Server:";
				tabPointer = generalTab;
				loop = generalLoop;
			}
			else if (settingName.upper() == "DB" || settingName.upper() == "DATABASE")
			{
				settingName = "Database:";
				tabPointer = generalTab;
				loop = generalLoop;
			}
			else if (settingName.upper() == "USER" || settingName.upper() == "UID" || settingName.upper() == "USERNAME")
			{
				settingName = "Username:";
				tabPointer = generalTab;
				loop = generalLoop;
			}
			else if (settingName.upper() == "PASS" || settingName.upper() == "PWD" || settingName.upper() == "PASSWORD")
			{
				settingName = "Password:";
				tabPointer = generalTab;
				loop = generalLoop;
			}
													// Advanced settings (everything else)
			else
			{
				if (settingName.upper() == "CPTIMEOUT")
					settingName = "CP Timeout:";
				else if (settingName.upper() == "CPREUSE")
					settingName = "CP Reuse:";
				else if (settingName.upper() == "DRIVER64")
					settingName = "Driver 64:";
				else if (settingName.upper() == "SETUP64")
					settingName = "Setup 64:";
				else
					settingName.append(":");
				tabPointer = advancedTab;
				loop = advancedLoop;
			}

			// If in view / edit mode, get configured settings. Else use driver default...
			if (globalDialogMode != DSNMODE_ADD)
			{
				if (generalLoop != 1)
				{
					if ((globalDSNType == DSNTYPE_USER) || (globalDSNType == DSNTYPE_SYSTEM))  // if it is a user or system dsn...
						SQLGetPrivateProfileString(dataSourceName.latin1(), dataSourcePointer->szName, "", dataSourceResults, ODBC_BUFFER_SIZE, 0);
					else if (globalDSNType == DSNTYPE_FILE)			// if it is a file dsn
						SQLReadFileDSN(dataSourceName.latin1(), "ODBC", dataSourcePointer->szName, dataSourceResults, ODBC_BUFFER_SIZE, 0);
					else
						SQLGetPrivateProfileString(dataSourceName.latin1(), dataSourcePointer->szName, "", dataSourceResults, ODBC_BUFFER_SIZE, "odbcinst");
				}
				else
				{
					strncpy(dataSourceResults, dataSourceName.latin1(), ODBC_BUFFER_SIZE);
				}
			}
			else
			{
				strncpy(dataSourceResults, dataSourcePointer->szValue, ODBC_BUFFER_SIZE);
			}

			// Grid layout column 1, the label...
			if (dataSourcePointer->nPromptType != ODBCINST_PROMPTTYPE_HIDDEN)
			{
				label = new QLabel(tabPointer);
				label->setText(tr(settingName));
				((QGridLayout *)tabPointer->layout())->addWidget(label, loop, 0);
				QWhatsThis::add(label, tr(dataSourcePointer->pszHelp));
			}

			// Grid layout column 2, data entry...
			switch (dataSourcePointer->nPromptType)
			{
				case ODBCINST_PROMPTTYPE_TEXTEDIT:
					lineEdit = new QLineEdit(tabPointer);
					lineEdit->setText(dataSourceResults);
					if (generalLoop != 1 || globalDialogMode == DSNMODE_ADD)
						lineEdit->setReadOnly(viewMode);
					else
						lineEdit->setReadOnly(TRUE);
					((QGridLayout *)tabPointer->layout())->addMultiCellWidget(lineEdit, loop, loop, 1, 2);
					QWhatsThis::add(lineEdit, tr(dataSourcePointer->pszHelp));
					label->setBuddy(lineEdit);
					dataSourcePointer->pWidget = lineEdit;
					break;

				case ODBCINST_PROMPTTYPE_LISTBOX:
				case ODBCINST_PROMPTTYPE_COMBOBOX:
					if (viewMode == TRUE)
					{
						lineEdit = new QLineEdit(tabPointer);
						lineEdit->setText(dataSourceResults);
						lineEdit->setReadOnly(viewMode);
						((QGridLayout *)tabPointer->layout())->addMultiCellWidget(lineEdit, loop, loop, 1, 2);
						QWhatsThis::add(lineEdit, tr(dataSourcePointer->pszHelp));
						label->setBuddy(lineEdit);
						dataSourcePointer->pWidget = lineEdit;
					}
					else
					{
						comboBox = new QComboBox(TRUE, tabPointer);
						comboBox->insertStrList((const char **)dataSourcePointer->aPromptData);
						comboBox->setEditText(dataSourceResults);
						((QGridLayout *)tabPointer->layout())->addMultiCellWidget(comboBox, loop, loop, 1, 2);
						QWhatsThis::add(comboBox, tr(dataSourcePointer->pszHelp));
						label->setBuddy(comboBox);
						dataSourcePointer->pWidget = comboBox;
					}
					break;

				case ODBCINST_PROMPTTYPE_FILENAME:
					fileWidget = new SelectFileWidget(tabPointer);
					fileWidget->setText(dataSourceResults);
					fileWidget->setReadOnly(viewMode);
					((QGridLayout *)tabPointer->layout())->addMultiCellWidget(fileWidget, loop, loop, 1, 2);
					QWhatsThis::add(fileWidget, tr(dataSourcePointer->pszHelp));
					label->setBuddy(fileWidget);
					dataSourcePointer->pWidget = fileWidget;
					break;

				case ODBCINST_PROMPTTYPE_TEXTEDIT_PASSWORD:
					lineEdit = new QLineEdit(tabPointer);
					lineEdit->setText(dataSourceResults);
					lineEdit->setReadOnly(viewMode);
					lineEdit->setEchoMode(QLineEdit::Password);
					((QGridLayout *)tabPointer->layout())->addMultiCellWidget(lineEdit, loop, loop, 1, 2);
					QWhatsThis::add(lineEdit, tr(dataSourcePointer->pszHelp));
					label->setBuddy(lineEdit);
					dataSourcePointer->pWidget = lineEdit;
					break;

				default: //ODBCINST_PROMPTTYPE_LABEL:
					lineEdit = new QLineEdit(tabPointer);
					lineEdit->setReadOnly(TRUE);
					lineEdit->setText(dataSourceResults);
					((QGridLayout *)tabPointer->layout())->addMultiCellWidget(lineEdit, loop, loop, 1, 2);
					QWhatsThis::add(lineEdit, tr(dataSourcePointer->pszHelp));
					label->setBuddy(lineEdit);
					dataSourcePointer->pWidget = lineEdit;
					break;
			}

			// Increment loop counter...
			if (tabPointer == generalTab)
				generalLoop++;
			else if (tabPointer == loggingTab)
				loggingLoop++;
			else
				advancedLoop++;
		}

		// Hide unused labels and add spacers...
		settingsLabel->setHidden(TRUE);
		advancedLabel->setHidden(TRUE);
		loggingLabel->setHidden(TRUE);
		if ((advancedLoop < 2) && (loggingLoop < 2))
			expertCheck->setHidden(TRUE);
		spacer = new QSpacerItem(1, 2, QSizePolicy::Minimum, QSizePolicy::Expanding);
    	generalTabLayout->addItem(spacer, generalLoop+1, 0);
		spacer = new QSpacerItem(1, 2, QSizePolicy::Minimum, QSizePolicy::Expanding);
    	advancedTabLayout->addItem(spacer, advancedLoop+1, 0);
		spacer = new QSpacerItem(1, 2, QSizePolicy::Minimum, QSizePolicy::Expanding);
    	loggingTabLayout->addItem(spacer, loggingLoop+1, 0);

		// Default to hiding extra tabs...
		dataSourceTabs->removePage(loggingTab);
		dataSourceTabs->removePage(advancedTab);
	}

	// Set config mode...
	SQLSetConfigMode(ODBC_BOTH_DSN);

	resize(sizeHint());

	// Create success or fail...
	return everythingOk;
}


////////////////////////////////////////////////////////////////////////////
// SAVE SECTION

void DataSourceDialog::saveDataSource()				// Save the edited dsn configuration
{
	// Declare vaviables...
	bool error = FALSE;
	QString fileDSN;

	// is there nothing for us to do 
	if ( configInDriver ) 
	{
		return;
	}

	// Set config mode...
	if (globalDSNType == DSNTYPE_USER)
		SQLSetConfigMode(ODBC_USER_DSN);
	else if (globalDSNType == DSNTYPE_SYSTEM)
		SQLSetConfigMode(ODBC_SYSTEM_DSN);

	// Initialise variables...
	fileDSN = QString("%1%2").arg(fileDSNDir, ((QLineEdit *)dataSourceProperties->pWidget)->text());

	if ((globalDSNType == DSNTYPE_USER) || (globalDSNType == DSNTYPE_SYSTEM))			// Add User / System DSN...
	{
		dataSourceName = ((QLineEdit *)dataSourceProperties->pWidget)->text();
		if (SQLWritePrivateProfileString(dataSourceName.latin1(), NULL, NULL, "odbc.ini") == FALSE)
		{
			QMessageBox::critical(this, tr("Add Data Source"), "Unable to create data source", QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
			error = TRUE;
		}
	}
	else if (globalDSNType == DSNTYPE_FILE)										// Add File DSN...
	{
		if (SQLWriteFileDSN(fileDSN.latin1(), "ODBC", NULL, NULL) == FALSE)
		{
			QMessageBox::critical(this, tr("Add Data Source"), "Unable to create data source", QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton);
			error = TRUE;
		}
	}

	// Save all DSN settings...
	if (error == FALSE)
	{
		for (dataSourcePointer = dataSourceProperties->pNext; dataSourcePointer != NULL; dataSourcePointer = dataSourcePointer->pNext)
		{
			switch (dataSourcePointer->nPromptType)
			{
				case ODBCINST_PROMPTTYPE_LABEL:
				case ODBCINST_PROMPTTYPE_TEXTEDIT:
				case ODBCINST_PROMPTTYPE_TEXTEDIT_PASSWORD:
					if ((globalDSNType == DSNTYPE_USER) || (globalDSNType == DSNTYPE_SYSTEM))
						SQLWritePrivateProfileString(dataSourceName.latin1(), dataSourcePointer->szName, ((QLineEdit *)dataSourcePointer->pWidget)->text().latin1(), "odbc.ini");
					else if (globalDSNType == DSNTYPE_FILE)
						SQLWriteFileDSN(fileDSN.latin1(), "ODBC", dataSourcePointer->szName, ((QLineEdit *)dataSourcePointer->pWidget)->text().latin1());
					else
						SQLWritePrivateProfileString(dataSourceName.latin1(), dataSourcePointer->szName, ((QLineEdit *)dataSourcePointer->pWidget)->text().latin1(), "odbcinst");
					break;

				case ODBCINST_PROMPTTYPE_FILENAME:
					if ((globalDSNType == DSNTYPE_USER) || (globalDSNType == DSNTYPE_SYSTEM))
						SQLWritePrivateProfileString(dataSourceName.latin1(), dataSourcePointer->szName, ((SelectFileWidget *)dataSourcePointer->pWidget)->text().latin1(), "odbc.ini");
					else if (globalDSNType == DSNTYPE_FILE)
						SQLWriteFileDSN(fileDSN.latin1(), "ODBC", dataSourcePointer->szName, ((SelectFileWidget *)dataSourcePointer->pWidget)->text().latin1());
					else
						SQLWritePrivateProfileString(dataSourceName.latin1(), dataSourcePointer->szName, ((SelectFileWidget *)dataSourcePointer->pWidget)->text().latin1(), "odbcinst");
					break;

				case ODBCINST_PROMPTTYPE_LISTBOX:
				case ODBCINST_PROMPTTYPE_COMBOBOX:
					if ((globalDSNType == DSNTYPE_USER) || (globalDSNType == DSNTYPE_SYSTEM))
						SQLWritePrivateProfileString(dataSourceName.latin1(), dataSourcePointer->szName, ((QComboBox *)dataSourcePointer->pWidget)->currentText().latin1(), "odbc.ini");
					else if (globalDSNType == DSNTYPE_FILE)
						SQLWriteFileDSN(fileDSN.latin1(), "ODBC", dataSourcePointer->szName, ((QComboBox *)dataSourcePointer->pWidget)->currentText().latin1());
					else
						SQLWritePrivateProfileString(dataSourceName.latin1(), dataSourcePointer->szName, ((QComboBox *)dataSourcePointer->pWidget)->currentText().latin1(), "odbcinst");;
					break;
			}
		}
		// Set config mode...
		SQLSetConfigMode(ODBC_BOTH_DSN);
		
		accept();
	}
	else
	{
		// Set config mode...
		SQLSetConfigMode(ODBC_BOTH_DSN);

		reject();
	}
}


////////////////////////////////////////////////////////////////////////////
// MISC SECTION

void DataSourceDialog::setExpertOptions()
{
	if (expertCheck->isChecked() == TRUE)
	{
		if (advancedLoop > 1)
		{
			dataSourceTabs->addTab(advancedTab, tr("Advanced"));
		}
		if (loggingLoop > 1)
		{
			dataSourceTabs->addTab(loggingTab, tr("Logging"));
		}
	}
	else
	{
		if (advancedLoop > 1)
			dataSourceTabs->removePage(advancedTab);
		if (loggingLoop > 1)
			dataSourceTabs->removePage(loggingTab);
	}
}

