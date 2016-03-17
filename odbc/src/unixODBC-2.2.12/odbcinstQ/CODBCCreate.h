/**************************************************
 * 
 *
 **************************************************
 * This code was created by Nick Gorham nick.gorham@easysoft.com
 * Released under GPL 22.MAY.2006
 *
 * Contributions from...
 * -----------------------------------------------
 *
 **************************************************/
#ifndef CODBCCreate_included
#define CODBCCreate_included

#include <sql.h>
#include <sqlext.h>
#include <odbcinstext.h>

#include <qwidget.h>
#include <qmessagebox.h>
#include <qwizard.h>
#include <qvbox.h>
#include <qhbox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qvbuttongroup.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qtextedit.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#if QT_VERSION>=300
#include <qsettings.h>
#endif

class CODBCCreate : public QWizard
{
    Q_OBJECT

public:
    CODBCCreate( QWidget* parent = 0, const char* name = 0 );
    virtual ~CODBCCreate();
	void setDsn( const char *dsn );
	int getRetCode();

	void setValid( bool val );
	void setKeywords( QString kw );

public slots:
	void fds_click();
	void uds_click();
	void sds_click();
	void file_click();
	void ad_click();
	void dl_click(QListViewItem*);
	void page_change(const QString &title );
	void file_changed(const QString &text);

protected:
	void setupPage1();
	void setupPage2();
	void setupPage3();
	void setupPage4();

	void populate();

	bool createDsn();

	QLabel *lab;
	QVBox *box, *box1, *box2;
	QHBox *box3;
	QHBox *box1a;
	QGroupBox *gb;
	QVButtonGroup *bg;
	QPushButton *file_find, *advanced;
	QRadioButton *fds, *sds, *uds;
	QListView *driver_list;
	QString current_driver, dsn;
	QTextEdit *arg_list;
	QLineEdit *file_edit;
	QString extra_keywords;

	SQLRETURN ret_code;
	bool verify;

    virtual void LoadState();
    virtual void SaveState();

	bool appropriate ( QWidget *page ) const;
	void accept ();
	void reject ();
};

class CODBCAdvanced : public QDialog
{
    Q_OBJECT

public:
    CODBCAdvanced( QWidget* parent = 0, const char* name = 0, WFlags nFlags = 0 );
    virtual ~CODBCAdvanced();

	void setValid( bool val );
	void setKeywords( QString kw );

public slots:
	void ad_ok();

protected:

	QPushButton *ok, *cancel;
	CODBCCreate *parent;
	QLabel *lab;
	QTextEdit *text_list;
	QCheckBox *valid;
};

#endif 
