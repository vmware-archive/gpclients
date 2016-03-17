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


#ifndef SELECTFILEWIDGET_H
#define SELECTFILEWIDGET_H

#include <qwidget.h>

class QLineEdit;
class QToolButton;
class QString;

class SelectFileWidget : public QWidget
{
	Q_OBJECT

	public:
		SelectFileWidget(QWidget *parent = 0, const char *name = 0);
		void setText(QString setFile);
		void setReadOnly(bool readOnly);
		QString text();

	private:
		QLineEdit *fileName;
		QToolButton *fileButton;

	private slots:
		void selectFile();
};

#endif
