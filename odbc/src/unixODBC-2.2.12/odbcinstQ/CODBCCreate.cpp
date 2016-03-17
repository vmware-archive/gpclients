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
#include <unistd.h>

#include "CODBCCreate.h"

CODBCCreate::CODBCCreate( QWidget* parent, const char* name )
	: QWizard( parent, name, TRUE )
{
	setupPage1();
	setupPage2();
	setupPage3();
	setupPage4();
    LoadState();
	setNextEnabled( box1, FALSE );
	setNextEnabled( box3, FALSE );
    connect( this, SIGNAL(selected(const QString &)), SLOT(page_change(const QString &)) );

	setCaption( "Create New Data Source" );

	extra_keywords = "";
	verify = true;
	ret_code = false;
}

CODBCCreate::~CODBCCreate()
{
    SaveState();
}

void CODBCCreate::setDsn( const char *dsn )
{
	if ( dsn && strlen( dsn ) > 0 ) 
	{
		this->dsn = dsn;
	}
	else
	{
		this->dsn = "";
	}
}

int CODBCCreate::getRetCode()
{
	return ret_code;
}

void CODBCCreate::LoadState()
{
#if QT_VERSION>=300
    QSettings settings;

    // main window
    {
#ifdef Q_WS_X11
        int nX = settings.readNumEntry( "/unixODBC/ODBCCreate/x", geometry().x() );
        int nY = settings.readNumEntry( "/unixODBC/ODBCCreate/y", geometry().y() );
        int nW = settings.readNumEntry( "/unixODBC/ODBCCreate/w", geometry().width() );
        int nH = settings.readNumEntry( "/unixODBC/ODBCCreate/h", geometry().height() );
        setGeometry( nX, nY, nW, nH );
#else
        int nW = settings.readNumEntry( "/unixODBC/ODBCCreate/w", geometry().width() );
        int nH = settings.readNumEntry( "/unixODBC/ODBCCreate/h", geometry().height() );
        resize( nW, nH );
#endif
    }
#endif
}

void CODBCAdvanced::setValid( bool val )
{
	valid->setChecked( val );
}

void CODBCAdvanced::setKeywords( QString kw )
{
	text_list->setText( kw );	
}

void CODBCAdvanced::ad_ok()
{
	parent->setValid( valid->isChecked());
	parent->setKeywords( text_list->text());
}

CODBCAdvanced::CODBCAdvanced( QWidget* parent, const char* name, WFlags nFlags )
	: QDialog( parent, name, nFlags )
{
	setFixedHeight( 340 );
	setFixedWidth( 470 );

	setCaption( "Advanced File DSN Creation Settings" );

	this->parent = (CODBCCreate*)parent;

	ok = new QPushButton( "O&K", this );
	ok->setGeometry( 370,10, 80,25 );

	cancel = new QPushButton( "&Cancel", this );
	cancel->setGeometry( 370,45, 80,25 );

    lab = new QLabel( this );
	lab -> setAlignment( AlignTop | AlignLeft );
	lab -> setGeometry( 10,10, 330,150 );
	lab -> setText( "If you know the driver specific keywords for this data\nsource, you can type them and their values here. Put a\nnew keyword-value pair on each line. For example.\n\n      Server=MyServer\n      Database=MyDatabase\n\nFor more information on driver-specific keywords, please\nconsult your ODBC driver's documentation" );

    lab = new QLabel( this );
	lab -> setAlignment( AlignTop | AlignLeft );
	lab -> setGeometry( 10,175, 350,50 );
	lab -> setText( "Enter driver-specific keywords and values:" );

	text_list = new QTextEdit( this );
    text_list -> setGeometry( 10, 200, 350, 100);

    valid = new QCheckBox( "Verify this connection {recommended}", this );
	valid -> setGeometry( 10,295, 350,50 );

	connect( cancel, SIGNAL(clicked()), SLOT(reject()) );
	connect( ok, SIGNAL(clicked()), SLOT(ad_ok()) );
	connect( ok, SIGNAL(clicked()), SLOT(accept()) );
}

CODBCAdvanced::~CODBCAdvanced()
{
}

void CODBCCreate::ad_click()
{
	CODBCAdvanced odbcad( this, "ODBCAdvanced", Qt::WType_Modal );
	odbcad.setKeywords( extra_keywords );
	odbcad.setValid( verify );
	odbcad.exec();
}

void CODBCCreate::setValid( bool val )
{
	verify = val;
}

void CODBCCreate::setKeywords( QString kw )
{
	extra_keywords = kw;
}

bool CODBCCreate::createDsn()
{
	BOOL ret;
	QString fname = file_edit->text();

	// Make sure it ends with .dsn
	
	if ( fname.right( 4 ).lower().compare( ".dsn" ))
	{
		fname.append( ".dsn" );
	}

	if ( access( fname, F_OK ) == 0 )
	{
		int replace = QMessageBox::information( NULL, "Save File DSN", "Data source file exists. Overwrite?", QMessageBox::Yes, QMessageBox::No );
		if ( replace == QMessageBox::No )
		{
			inst_logPushMsg( __FILE__, __FILE__, __LINE__, LOG_CRITICAL, ODBC_ERROR_CREATE_DSN_FAILED, "" );
			return false;
		}
	}

	if ( unlink( fname ))
	{
		inst_logPushMsg( __FILE__, __FILE__, __LINE__, LOG_CRITICAL, ODBC_ERROR_CREATE_DSN_FAILED, "" );
		return false;
	}

	ret = SQLWriteFileDSN((const char*) fname,
			"ODBC",
			"DRIVER",
			(const char *)current_driver );

	if ( !ret )
	{
		return false;
	}

	if ( extra_keywords.length() > 0 )
	{
		int start = 0;
		int end = 0;
		int eq;
		QString str;

		while( start < extra_keywords.length() )
		{
			end = extra_keywords.find( '\n', start );

			if ( end == -1 )
			{
				end = extra_keywords.length();
			}

			str = extra_keywords.mid( start, end-start ) + ";";
			eq = str.find( '=', 0 );
			if ( eq > 0 )
			{
				ret = SQLWriteFileDSN((const char*) fname,
							"ODBC",
							(const char *) str.left( eq ),
							(const char *) str.right( str.length() - eq - 1 ));
				if ( !ret )
				{
					return false;
				}
			}
			start = end + 1;
		}
	}

	return true;
}

void CODBCCreate::SaveState()
{
#if QT_VERSION>=300
    QSettings settings;

    // main window
    settings.writeEntry( "/unixODBC/ODBCCreate/x", x() );
    settings.writeEntry( "/unixODBC/ODBCCreate/y", y() );
    settings.writeEntry( "/unixODBC/ODBCCreate/w", width() );
    settings.writeEntry( "/unixODBC/ODBCCreate/h", height() );
#endif
}

void CODBCCreate::reject()
{
	inst_logPushMsg( __FILE__, __FILE__, __LINE__, LOG_CRITICAL, ODBC_ERROR_USER_CANCELED, "" );
	ret_code = false;

	QWizard::reject();
}

void CODBCCreate::accept()
{
	if ( fds->isOn())
	{
		QString conn_str;
		char out_str[ 4095 ];
		const char *in_str;
		SQLHENV henv;
		SQLHDBC hdbc;
		SQLSMALLINT len;
		SQLRETURN ret;
		QString fname = file_edit->text();

		// Make sure it ends with .dsn
	
		if ( fname.right( 4 ).lower().compare( ".dsn" ))
		{
			fname.append( ".dsn" );
			file_edit->setText( fname );
		}

		conn_str = "DRIVER={" + current_driver + "};SAVEFILE=" + fname + ";";

		if ( extra_keywords.length() > 0 )
		{
			int start = 0;
			int end = 0;

			while( start < extra_keywords.length() )
			{
				end = extra_keywords.find( '\n', start );

				if ( end == -1 )
				{
					end = extra_keywords.length();
				}

				conn_str += extra_keywords.mid( start, end-start ) + ";";
				start = end + 1;
			}
		}
		in_str = (const char*)conn_str;

		if ( verify )
		{
			SQLAllocEnv( &henv );
			SQLAllocConnect( henv, &hdbc );

			ret = SQLDriverConnect( hdbc, (SQLHWND)1, 
					(SQLCHAR*)in_str, strlen( in_str ), (SQLCHAR*)out_str, 
					sizeof( out_str ), &len, SQL_DRIVER_COMPLETE );
	
			SQLFreeConnect( hdbc );
			SQLFreeEnv( henv );

			if ( ret != SQL_SUCCESS ) 
			{
				int create = QMessageBox::No;

				create = QMessageBox::information( NULL, "Create Data Source", "A connection could not be made using the file data source parameters entered. Save non-verified file DSN?", QMessageBox::Yes, QMessageBox::No );
				if ( create == QMessageBox::No )
				{
					inst_logPushMsg( __FILE__, __FILE__, __LINE__, LOG_CRITICAL, ODBC_ERROR_CREATE_DSN_FAILED, "" );
					ret_code = false;
					QWizard::reject();
					return;
				}
				else 
				{
					strcpy( out_str, in_str );
					if ( !createDsn())
					{
						ret_code = false;
						QWizard::reject();
						return;
					}
				}
			}
		}
		else
		{
			strcpy( out_str, in_str );
			if ( !createDsn())
			{
				ret_code = false;
				QWizard::reject();
				return;
			}
		}
	
		ret_code = true;
	}
	else
	{
		int mode;

		if ( sds -> isOn())
		{
			mode = ODBC_ADD_SYS_DSN;
		}
		else
		{
			mode = ODBC_ADD_DSN;
		}

		if ( dsn.length() > 0 )
		{
			ret_code = SQLConfigDataSource((HWND) 1, mode, current_driver, dsn.prepend( "DSN=" ));
		}
		else
		{
			ret_code = SQLConfigDataSource((HWND) 1, mode, current_driver, "" );
		}
	}

	QWizard::accept();
}

bool CODBCCreate::appropriate ( QWidget *page ) const
{
	if ( page == box3 )
	{
		return fds->isOn();
	}
	else
	{
		return true;
	}
}

void CODBCCreate::page_change( const QString &title )
{
	/*
	 * Load the text list
	 */
	arg_list->clear();

	if ( currentPage() == box2 )
	{
		QString prefix;

		if ( fds -> isOn())
		{
			prefix = "File Data Source ";
		}
		else if ( sds -> isOn())
		{
			prefix = "System Data Source " + dsn;
		}
		else if ( uds -> isOn())
		{
			prefix = "User Data Source " + dsn;
		}

		arg_list->append( prefix );
		if ( fds -> isOn())
		{
			QString fname = file_edit->text();

			// Make sure it ends with .dsn
	
			if ( fname.right( 4 ).lower().compare( ".dsn" ))
			{
				fname.append( ".dsn" );
				file_edit->setText( fname );
			}
			arg_list->append( "File name: " + fname );
		}
		arg_list->append( "Driver: " + current_driver );

		if ( fds -> isOn() && extra_keywords.length() > 0 )
		{
			arg_list->append( "Driver-specific Keywords:");
			arg_list->append( extra_keywords );
		}
	}
	else if ( currentPage() == box1 )
	{
		advanced->setEnabled( fds->isOn());
	}
}

void CODBCCreate::file_click()
{
	char path[ 1024 ];
	char def[ 1024 ];

	sprintf( def, "%s/ODBCDataSources", odbcinst_system_file_path());

	SQLGetPrivateProfileString( "ODBC", "FileDSNPath", 
			def, path, sizeof( path ), "odbcinst.ini" );

	QString s = QFileDialog::getSaveFileName(
				path,
				"ODBC File Data Sources's (*.dsn)",
				this,
				"Select file name",
				"Choose a filename to save under" );

	if ( s )
	{
		file_edit->setText( s );
	}
}

void CODBCCreate::file_changed(const QString &text)
{
	if ( text.length() > 0 ) 
	{
		setNextEnabled( box3, TRUE );
	}
	else
	{
		setNextEnabled( box3, FALSE );
	}
}

void CODBCCreate::dl_click( QListViewItem *item )
{
	current_driver = item->text( 0 );
	setNextEnabled( box1, TRUE );
}

void CODBCCreate::fds_click()
{
	lab->setText( "Selecting File Data Source creates a file-based\ndata sourcewhich is shareable between all\nusers with access to the database" );
}

void CODBCCreate::uds_click()
{
	lab->setText( "Selecting User Data Source creates a data source\nwhich is specific to this machine, and visable\nonly to you" );
}

void CODBCCreate::sds_click()
{
	lab->setText( "Selecting System Data Source creates a data source\nwhich is specific to this machine, and usable\nby any user who logs onto the machine" );
}

void CODBCCreate::populate()
{
  	char      	szDriverName[INI_MAX_OBJECT_NAME+1];
  	char      	szPropertyName[INI_MAX_PROPERTY_NAME+1];
  	char      	szDescription[INI_MAX_PROPERTY_VALUE+1];
  	char      	szDriver[INI_MAX_PROPERTY_VALUE+1];
  	char      	szDriver64[INI_MAX_PROPERTY_VALUE+1];
  	char      	szSetup[INI_MAX_PROPERTY_VALUE+1];
  	char      	szSetup64[INI_MAX_PROPERTY_VALUE+1];
  	QListViewItem *pListViewItem;
  	QString     qsError;
  	char        szINI[FILENAME_MAX+1];
	HINI		hIni;

  	sprintf( szINI, "%s/odbcinst.ini", odbcinst_system_file_path());

  	if ( iniOpen( &hIni, szINI, "#;", '[', ']', '=', TRUE ) != INI_ERROR )
  	{
    	iniObjectFirst( hIni );
    	while ( iniObjectEOL( hIni ) == FALSE )
    	{
      		szDriverName[0] = '\0';
      		szDescription[0]  = '\0';
      		szDriver[0]   = '\0';
      		szDriver64[0]   = '\0';
      		szSetup64[0]   = '\0';
      		szSetup[0]    = '\0';
      		iniObject( hIni, szDriverName );
      		iniPropertyFirst( hIni );
      		if ( strcmp( szDriverName, "ODBC" ) == 0 )
      		{
        		iniObjectNext( hIni );
        		continue;
      		}

      		while ( iniPropertyEOL( hIni ) == FALSE )
      		{
        		iniProperty( hIni, szPropertyName );
        		iniToUpper( szPropertyName );

        		if ( strncmp( szPropertyName, "DESCRIPTION", INI_MAX_PROPERTY_NAME ) == 0 )
          			iniValue( hIni, szDescription );
#ifdef PLATFORM64
				if ( strncmp( szPropertyName, "DRIVER64", INI_MAX_PROPERTY_NAME ) == 0 )
          			iniValue( hIni, szDriver64 );
				if ( strncmp( szPropertyName, "DRIVER", INI_MAX_PROPERTY_NAME ) == 0 )
          			iniValue( hIni, szDriver );

        		if ( strncmp( szPropertyName, "SETUP64", INI_MAX_PROPERTY_NAME ) == 0 )
          			iniValue( hIni, szSetup64 );
        		if ( strncmp( szPropertyName, "SETUP", INI_MAX_PROPERTY_NAME ) == 0 )
          			iniValue( hIni, szSetup );
#else
        		if ( strncmp( szPropertyName, "DRIVER", INI_MAX_PROPERTY_NAME ) == 0 )
          			iniValue( hIni, szDriver );

        		if ( strncmp( szPropertyName, "SETUP", INI_MAX_PROPERTY_NAME ) == 0 )
          			iniValue( hIni, szSetup );
#endif

        		iniPropertyNext( hIni );
      		}

#ifdef PLATFORM64
      		if ( szDriver64[ 0 ] != '\0' )
      		{
	      		strcpy( szDriver, szDriver64 );
      		}
      		if ( szSetup64[ 0 ] != '\0' )
      		{
	      		strcpy( szSetup, szSetup64 );
      		}
#endif

      		pListViewItem = new QListViewItem( driver_list, szDriverName, szDescription, szDriver, szSetup );
      		iniObjectNext( hIni );
    	}
    	iniClose( hIni );
  	}
  	else
  	{
    	qsError.sprintf( "Could not open system file at %s", szINI );
    	QMessageBox::information( this, "Create New Data Source",  qsError );
  	}
}

void CODBCCreate::setupPage3()
{
	box3 = new QHBox( this );
	box3->setSpacing( 5 );

	file_edit = new QLineEdit( box3 );
	file_find = new QPushButton( "Browse", box3, "Browse" );

    connect( file_find, SIGNAL(clicked()), SLOT(file_click()) );
    connect( file_edit, SIGNAL(textChanged(const QString &)), SLOT(file_changed(const QString &)) );

	addPage( box3, "Type the name of the file data source you want to save\nthis connection to. Or, find the location to save to\nby clicking Browse" );

	setHelpEnabled( box3, FALSE );
	setFinishEnabled( box3, FALSE );
}

void CODBCCreate::setupPage4()
{
	box2 = new QVBox( this );

	arg_list = new QTextEdit( box2 );
    arg_list -> setGeometry( 10, 10, 300, 100);
  	arg_list -> setMinimumSize( 50, 50 );
  	arg_list -> setMaximumSize( 32767, 32767 );
	arg_list -> setReadOnly( true );

	addPage( box2, "When you click finish, you will create the data source\nwhich you have just configured. The driver may prompt\nyou for more information" );

	setHelpEnabled( box2, FALSE );
	setFinishEnabled( box2, TRUE );
}

void CODBCCreate::setupPage2()
{
	box1 = new QVBox( this );
	box1->setSpacing( 5 );

	driver_list = new QListView( box1 );
    driver_list -> setGeometry( 10, 10, 300, 100);
  	driver_list -> setMinimumSize( 50, 50 );
  	driver_list -> setMaximumSize( 32767, 32767 );
  	driver_list -> setFocusPolicy( QWidget::TabFocus );
  	driver_list -> setBackgroundMode( QWidget::PaletteBackground );
  	driver_list -> setFrameStyle( QFrame::Box | QFrame::Raised );
  	driver_list -> setResizePolicy( QScrollView::Manual );
  	driver_list -> setVScrollBarMode( QScrollView::Auto );
  	driver_list -> setHScrollBarMode( QScrollView::Auto );
  	driver_list -> setTreeStepSize( 20 );
  	driver_list -> setMultiSelection( FALSE );
  	driver_list -> setAllColumnsShowFocus( TRUE );
  	driver_list -> setItemMargin( 1 );
  	driver_list -> setRootIsDecorated( FALSE );
  	driver_list -> addColumn( "Name", -1 );
  	driver_list -> setColumnWidthMode( 0, QListView::Maximum );
  	driver_list -> setColumnAlignment( 0, 1 );
  	driver_list -> addColumn( "Description", -1 );
  	driver_list -> setColumnWidthMode( 1, QListView::Maximum );
  	driver_list -> setColumnAlignment( 1, 1 );
  	driver_list -> addColumn( "Driver Lib", -1 );
  	driver_list -> setColumnWidthMode( 2, QListView::Maximum );
  	driver_list -> setColumnAlignment( 2, 1 );
  	driver_list -> addColumn( "Setup Lib", -1 );
  	driver_list -> setColumnWidthMode( 3, QListView::Maximum );
  	driver_list -> setColumnAlignment( 3, 1 );

	box1a = new QHBox( box1 );
	advanced = new QPushButton( "Advanced...", box1a, "Advanced" );
	advanced -> setEnabled( false );
	advanced->setFixedWidth( 90 );

	populate();

    connect( driver_list, SIGNAL(clicked(QListViewItem *)), SLOT(dl_click(QListViewItem *)) );
    connect( advanced, SIGNAL(clicked()), SLOT(ad_click()) );

	addPage( box1, "Select a driver for which you want to set up a data source" );
	setHelpEnabled( box1, FALSE );
	setFinishEnabled( box1, FALSE );
}

void CODBCCreate::setupPage1()
{
	box = new QVBox( this );
    box->setGeometry( 10,10, 410,75 );

	bg = new QVButtonGroup( "", box, "Bgroup" );
    bg -> setGeometry( 10, 10, 300, 100);

	fds = new QRadioButton( "File Data Source", bg );
	uds = new QRadioButton( "User Data Source", bg );
	sds = new QRadioButton( "System Data Source", bg );

    connect( fds, SIGNAL(clicked()), SLOT(fds_click()) );
    connect( uds, SIGNAL(clicked()), SLOT(uds_click()) );
    connect( sds, SIGNAL(clicked()), SLOT(sds_click()) );

    lab = new QLabel( box );
	lab -> setAlignment( AlignTop | AlignLeft );

	fds -> setChecked( true );
	fds_click();

	addPage( box, "Select type of data source" );
	setHelpEnabled( box, FALSE );
	setFinishEnabled( box, FALSE );
}
