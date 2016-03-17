bin_PROGRAMS = isql dltest odbcinst iusql odbc_config

INCLUDES = -I@top_srcdir@/include $(LTDLINCL) \
	-DPREFIX=\"@prefix@\" \
	-DEXEC_PREFIX=\"@exec_prefix@\" \
	-DLIB_PREFIX=\"@libdir@\" \
	-DBIN_PREFIX=\"@bindir@\" \
	-DINCLUDE_PREFIX=\"@includedir@\" \
	-DSYSTEM_FILE_PATH=\"@sysconfdir@\"

EXTRA_DIST = \
	isql.h \
	COPYING

if QNX
odbcinst_LDADD = \
	-L$(prefix)/lib -lodbcinst \
	$(LIBLTDL) $(LIBADD_DL) 

odbcinst_DEPENDENCIES = \
    $(LIBLTDL) 
else
odbcinst_LDADD = \
	../odbcinst/libodbcinst.la \
	../ini/libinilc.la \
	$(LIBLTDL) $(LIBADD_DL) 

if WITHLT
odbcinst_DEPENDENCIES = \
	$(LIBLTDL) \
	../odbcinst/libodbcinst.la \
	../ini/libinilc.la 
else
odbcinst_DEPENDENCIES = \
	../odbcinst/libodbcinst.la \
	../ini/libinilc.la 
endif
endif

odbcinst_SOURCES = odbcinst.c 

if QNX
isql_LDADD = \
	-L$(prefix)/lib -lodbc \
 	$(READLINE)

isql_DEPENDENCIES = \
	$(LIBLTDL)
else
isql_LDADD = \
	../DriverManager/libodbc.la \
	../extras/libodbcextraslc.la \
 	$(READLINE)

if WITHLT
isql_DEPENDENCIES = \
	../DriverManager/libodbc.la \
	$(LIBLTDL)
else
isql_DEPENDENCIES = \
	../DriverManager/libodbc.la 
endif
endif

isql_SOURCES = isql.c

if QNX
iusql_LDADD = \
	-L$(prefix)/lib -lodbc \
	-lodbcinst \
 	$(READLINE)

iusql_DEPENDENCIES = \
	$(LIBLTDL)
else
iusql_LDADD = \
	../DriverManager/libodbc.la \
	../odbcinst/libodbcinst.la \
	../ini/libinilc.la \
 	$(READLINE)

if WITHLT
iusql_DEPENDENCIES = \
	../odbcinst/libodbcinst.la \
	../DriverManager/libodbc.la \
	../ini/libinilc.la \
	../extras/libodbcextraslc.la \
	$(LIBLTDL)
else
iusql_DEPENDENCIES = \
	../odbcinst/libodbcinst.la \
	../DriverManager/libodbc.la \
	../ini/libinilc.la \
	../extras/libodbcextraslc.la 
endif
endif

iusql_SOURCES = iusql.c

dltest_SOURCES = dltest.c

odbc_config_SOURCES = odbc-config.c

if WITHLT
dltest_DEPENDENCIES = $(LIBLTDL)
dltest_LDADD = $(LIBLTDL) $(LIBADD_DL)
else
endif
