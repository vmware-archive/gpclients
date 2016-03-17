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


#include <qlineedit.h>
#include <qtoolbutton.h>
#include <qfiledialog.h>
#include <qlayout.h>
#include <qstring.h>
#include <qwidget.h>

#include "selectfilewidget.h"

SelectFileWidget::SelectFileWidget(QWidget *parent, const char *name) : QWidget(parent,name)
{
	QGridLayout *mainLayout = new QGridLayout(this, 1, 1, 0, 6, 0);
	fileName = new QLineEdit(this);
	mainLayout->addWidget(fileName, 0, 0);
	fileButton = new QToolButton(this); 
	fileButton->setText("...");
	mainLayout->addWidget(fileButton, 0, 1);
	QObject::connect(fileButton, SIGNAL(clicked()), this, SLOT(selectFile()));
}

void SelectFileWidget::setText(QString setFile)
{
	fileName->setText(setFile);
}

void SelectFileWidget::setReadOnly(bool readOnly)
{
	fileName->setReadOnly(readOnly);
	fileButton->setEnabled(!readOnly);
}

void SelectFileWidget::selectFile()
{
	fileName->setText(QFileDialog::getOpenFileName(".", "All Files (*)", this));	
}

QString SelectFileWidget::text()
{
	return fileName->text();
}
