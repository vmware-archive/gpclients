/*
This code was created by Fizz (fizz@titania.co.uk)

ODBCConfig - unixODBC statistics
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
	// Initialise stuff...
    statsHandle = 0;
	progressMaxValue = 0;

	// Disable user and program columns if /proc does not exist...
	if (QFile::exists("/proc") == FALSE)
	{
		detailsTable->hideColumn(0);
		detailsTable->hideColumn(1);
	}

	// Get initial statistics...
	updateStatistics();

	// Configure and start the statistics refresh timer...
	statsTimer = new QTimer(this);
	statsTimer->start(timerSpin->value() * 100, FALSE);

	// Connect SIGNAL / SLOTS...
	connect(timerSpin, SIGNAL(valueChanged(int)), this, SLOT(updateTimer(int)));
	connect(statsTimer, SIGNAL(timeout()), this, SLOT(updateStatistics()));
	connect(aboutButton, SIGNAL(clicked()), this, SLOT(about()));
}

ODBCDialog::~ODBCDialog()
{
	uodbc_close_stats(statsHandle);
}


void ODBCDialog::updateTimer(int newValue)
{
	statsTimer->stop();
	statsTimer->start(newValue * 100, FALSE);
}


void ODBCDialog::updateStatistics()
{
	// Variables...
	int pidTotal = 0;
	bool statsFail = FALSE;
	int handleNum = 0;
	QString tempString;
	QString processUser = "";
	QString processName = "";
	QFile processFile;
	uid_t userID;
	struct passwd* passwdItem;

	// Set file name...
	processFile.setName("status");

	// Get statistics...
	if (!statsHandle)
	{
		if (uodbc_open_stats(&statsHandle, UODBC_STATS_READ) != 0)
		{
			statsFail = TRUE;
			statsHandle = 0;
		}
	}

	if (statsFail == FALSE)
	{
		// Get stats for summary tab...
		if (uodbc_get_stats(statsHandle, -1, statsResult, 4) == 4)
		{
			// Get progress bars max value...
			if (statsResult[0].value.l_value > progressMaxValue)
				progressMaxValue = statsResult[0].value.l_value;
			if (statsResult[1].value.l_value > progressMaxValue)
				progressMaxValue = statsResult[1].value.l_value;
			if (statsResult[2].value.l_value > progressMaxValue)
				progressMaxValue = statsResult[2].value.l_value;
			if (statsResult[3].value.l_value > progressMaxValue)
				progressMaxValue = statsResult[3].value.l_value;

			// Set progress bars and label values...
			envProgress->setProgress(statsResult[0].value.l_value, progressMaxValue);
			envLabel->setText(QString::number(statsResult[0].value.l_value));
			conProgress->setProgress(statsResult[1].value.l_value, progressMaxValue);
			conLabel->setText(QString::number(statsResult[1].value.l_value));
			staProgress->setProgress(statsResult[2].value.l_value, progressMaxValue);
			staLabel->setText(QString::number(statsResult[2].value.l_value));
			desProgress->setProgress(statsResult[3].value.l_value, progressMaxValue);
			desLabel->setText(QString::number(statsResult[3].value.l_value));

			// Get stats for details tab...
			pidTotal = uodbc_get_stats(statsHandle, 0, pidResults, MAXPROCESSES);
			detailsTable->setNumRows(pidTotal);

			// Update stat details table...
			for (int pidNumber = 0; pidNumber < pidTotal; pidNumber++)
        	{
				// Reset variables...
				processUser = "";
				processName = "";


				handleNum = uodbc_get_stats(statsHandle, pidResults[pidNumber].value.l_value, handleResult, MAXHANDLES);
				if (handleNum > 0)
				{
					// Get Process information (program name, username)...
					tempString = "/proc/";
					tempString.append(QString::number(pidResults[pidNumber].value.l_value));
					QDir::setCurrent(tempString);
					if (processFile.open(IO_ReadOnly))
					{
						while ((processFile.atEnd() == FALSE) && ((processName.isEmpty() == TRUE) || (processUser.isEmpty() == TRUE)))
						{
							processFile.readLine(tempString, 500);
							if (tempString.contains("Name:") > 0)
							{
								processName = tempString.right(tempString.length() - 6);
							}
							if (tempString.contains("Uid:") > 0)
							{
								processUser = tempString.mid(5, (tempString.find((char)9, 6) - 5));
								userID = processUser.toInt();
								passwdItem = getpwuid(userID);
								processUser = passwdItem->pw_name;
							}
						}
						processFile.close();
					}

					// Update the table...
					detailsTable->setText(pidNumber, 0, processUser);
					detailsTable->setText(pidNumber, 1, processName);
					detailsTable->setText(pidNumber, 2, QString::number(pidResults[pidNumber].value.l_value));
					for (int handle = 0; handle < MAXHANDLES; handle++)
					{
						detailsTable->setText(pidNumber, handle + 3, QString::number(handleResult[handle].value.l_value));
					}
				}
				else
				{
					clearDetailRow(pidNumber);
				}
			}
		}
		else
		{
			statsFail = TRUE;
		}
	}
}


void ODBCDialog::clearDetailRow(int row)
{
	int column;

	for ( column = 0; column < detailsTable->numCols(); column++ )
	{
		detailsTable->setText(row, column, "");
	}
}


void ODBCDialog::about()
{
	qt_aboutDialog dialog;
	dialog.exec();
}

