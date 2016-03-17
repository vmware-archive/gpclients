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


#ifndef ODBCDIALOG_H
#define ODBCDIALOG_H

#include "qt_about.h"
#include "qt_maindialog.h"

#include <qtimer.h>
#include <qtable.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qprogressbar.h>
#include <qlabel.h>
#include <qstring.h>
#include <qfile.h>
#include <qdir.h>
#include <pwd.h>
#include <uodbc_stats.h>

#define MAXPROCESSES 64
#define MAXHANDLES 4


class ODBCDialog : public qt_mainDialog
{
	Q_OBJECT

	public:
		ODBCDialog(QWidget *parent = 0, const char *name = 0);
		~ODBCDialog();
		
	private slots:
		void updateStatistics();
		void updateTimer(int newValue);
		void about();

	private:
		QTimer *statsTimer;
		void *statsHandle;
		uodbc_stats_retentry statsResult[4];
		uodbc_stats_retentry pidResults[MAXPROCESSES];
		uodbc_stats_retentry handleResult[MAXHANDLES];
		int progressMaxValue;
		void clearDetailRow(int row);
};

#endif
