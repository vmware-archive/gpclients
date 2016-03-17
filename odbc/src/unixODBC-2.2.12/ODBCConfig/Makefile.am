if QT

bin_PROGRAMS = ODBCConfig

INCLUDES = -I@top_srcdir@/include $(QT_CXXFLAGS) \
    -DSYSTEM_FILE_PATH=\"@sysconfdir@\" -DDEFLIB_PATH=\"@libdir@\" $(LTDLINCL)

ODBCConfig_LDADD =  \
	$(QT_LDFLAGS) \
	$(QT_LIBS) \
	../odbcinst/libodbcinst.la \
	../extras/libodbcextraslc.la \
	../DriverManager/libodbc.la 

ODBCConfig_DEPENDANCIES = ../odbcinst/libodbcinst.la ../extras/libodbcextraslc.la ../DriverManager/libodbc.la

ODBCConfig_SOURCES = \
	main.cpp

endif
