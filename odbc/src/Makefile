# Set the shell
SHELL=/bin/bash

# Protect the default target from utility targets in included makefiles
all: psqlodbc

# Load in global macros and support targets, functions
BLD_TOP=../../..
include $(BLD_TOP)/Makefile.global
include $(BLD_TOP)/Makefile.thirdparty

BUILDROOT:=$(shell pwd)
INSTALL_DIR_LOCAL=greenplum-connectivity
INSTALL_DIR=$(BUILDROOT)/$(INSTALL_DIR_LOCAL)
OS=$(shell uname -s)
ifeq "$(findstring win,$(BLD_ARCH))" "win"
OS=Windows
endif

PYTHON_VER:=$(shell python -V 2>&1| perl -p -i -e 's,^Python (\d+\.\d+).*$$,\1,')

# psqlodbc / unixODBC compatibility matrix

PSQLODBC-08.02.0400_UNIXODBC_VERSIONS=
PSQLODBC-08.02.0500_UNIXODBC_VERSIONS=2.2.12
PSQLODBC-08.03.0400_UNIXODBC_VERSIONS=2.2.12
PSQLODBC-08.04.0200_UNIXODBC_VERSIONS=2.2.12
PSQLODBC-09.00.0200_UNIXODBC_VERSIONS=2.2.12
PSQLODBC-09.02.0100_UNIXODBC_VERSIONS=2.2.12

.PHONY: psqlodbc $(PSQLODBC_TARGETS) unixodbc $(UNIXODBC_TARGETS)
.SECONDARY: $(foreach ver,$(UNIXODBC_VERSIONS),timestamp.unixODBC-$(ver))

# psqlodbc settings and iterator target
ifneq "$(findstring $(BLD_ARCH),rhel4_x86_64 rhel4_x86_32)" ""
PSQLODBC_VERSIONS=08.01.0200 08.02.0400 08.02.0500 08.03.0100 08.03.0200 08.03.0300 08.03.0400 08.04.0200 09.00.0200
else
PSQLODBC_VERSIONS=08.01.0200 08.02.0400 08.02.0500 08.03.0100 08.03.0200 08.03.0300 08.03.0400 08.04.0200 09.00.0200 09.02.0100
endif
PSQLODBC_DIR=${BUILDROOT}/psqlodbc-${PSQLODBC_VER}

PSQLODBC_TARGETS=$(foreach ver,$(PSQLODBC_VERSIONS),psqlodbc/$(ver)/$(BLD_BITS))

ifeq "$(findstring win,$(BLD_ARCH))" "win"
psqlodbc:
	rm -rf $(INSTALL_DIR_LOCAL)
	mkdir -p $(INSTALL_DIR_LOCAL)/odbc/psqlodbc-08.04.0200/unixodbc-2.2.12
	cp psqlodbc-08.04.0200/installer/psqlodbc.msm $(INSTALL_DIR_LOCAL)/odbc/psqlodbc-08.04.0200/unixodbc-2.2.12/
else
psqlodbc:
	rm -rf $(INSTALL_DIR_LOCAL)
	$(MAKE) $(PSQLODBC_TARGETS)
endif

psqlODBCVersion=$(*D)
Bits=$(*F)

psqlodbc/%:
	# ---- Build psqlODBC-$(psqlODBCVersion) ($(Bits)-bit) ----
	$(MAKE) PSQLODBC_VER=$(psqlODBCVersion) unixodbc

# unixODBC settings
AIX_UNIXODBC_VERSIONS=2.2.12
SunOS_UNIXODBC_VERSIONS=2.2.12
Linux_UNIXODBC_VERSIONS=2.2.12
Darwin_UNIXODBC_VERSIONS=2.2.12
UNIXODBC_VERSIONS=$(filter $($(OS)_UNIXODBC_VERSIONS),$(PSQLODBC-$(PSQLODBC_VER)_UNIXODBC_VERSIONS))

UNIXODBC_INC_DIR=${BUILDROOT}/unixODBC-$(UnixODBCVersion)/.install/include
UNIXODBC_LIB_DIR=${BUILDROOT}/unixODBC-${UnixODBCVersion}/.install/lib

UNIXODBC_DIR=$(BUILDROOT)/unixODBC-$(UnixODBCVersion)

52_32_DDTESTLIB=ivtestlib
52_64_DDTESTLIB=ddtestlib
53sp2_32_DDTESTLIB=ivtestlib
53sp2_64_DDTESTLIB=ddtestlib

#ifneq "$(BLD_BITS)" ""
#export NO_M64=1
#CFLAGS_BITS=-m$(BLD_BITS) -mcpu=v9
#endif

ifneq "$(findstring $(BLD_ARCH),aix5_ppc_32 aix5_ppc_64)" ""
CFLAGS_AUTHLIBS=-I$(BLD_THIRDPARTY_INCLUDE_DIR) -I/opt/freeware/include
LDFLAGS_AUTHLIBS=-L$(BLD_THIRDPARTY_LIB_DIR) -L/opt/freeware/lib
export LIBPATH=/usr/lib/threads:$(CURDIR)/$(BLD_THIRDPARTY_LIB_DIR):/opt/freeware/lib
endif

# CFLAGS by OS
SunOS_CFLAGS=-DINC_JQ_FIXES
AIX_CFLAGS=-pthread -DINC_JQ_FIXES

# CFLAGS by platform

# CFLAGS computation
CFLAGS=$(strip $(BLD_CFLAGS) $(CFLAGS_AUTHLIBS) $($(OS)_CFLAGS) $($(BLD_ARCH)_CFLAGS))

# LDFLAGS by OS
AIX_LDFLAGS=-L/usr/lib/threads -lssl -lcrypto

# LDFLAGS by platform

# LDFLAGS computation
LDFLAGS=$(strip $(BLD_LDFLAGS) $(LDFLAGS_AUTHLIBS) $($(OS)_LDFLAGS) $($(BLD_ARCH)_LDFLAGS))


rhel5_x86_32_CONFIG_FLAGS_UNIXODBC=--host=i686-pc-linux-gnu
BLD_CONFIG_FLAGS_UNIXODBC=$($(BLD_ARCH)_CONFIG_FLAGS_UNIXODBC)


UNIXODBC_TARGETS=$(foreach ver,$(UNIXODBC_VERSIONS),unixodbc/$(ver))

unixodbc: $(UNIXODBC_TARGETS)

UnixODBCVersion=$(*F)
Bits=$(BLD_BITS)

timestamp.unixODBC-%: unixODBC-%
	#
	# ---- Build $(<) ----
	#
	@echo ""
	@echo ""
	@echo ""
	env | sort
	@echo ""
	@echo ""
	@echo ""
	if [ -d $(<)-fixes ]; then \
	  (cd $(<)-fixes/ && $(TAR) cf - .) | (cd $(<)/ && $(TAR) xvf -); \
	fi
	touch $(<)/aclocal.m4
	touch $(<)/Makefile.in
	touch $(<)/configure
	touch $(<)/Makefile
	cd $(<)/ && \
	  CC="$(BLD_CC) $(CFLAGS)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" ./configure $(BLD_CONFIG_FLAGS_UNIXODBC) --enable-gui=no --enable-drivers=no --prefix=`pwd`/.install
ifeq "$(findstring osx,$(BLD_ARCH))" "osx"
	@echo "`date` -- INFO: Fixing libtool for safe running on OS X ($(BLD_ARCH))..."
	sed -e 's,whole_archive_flag_spec="-all_load \\$$convenience",whole_archive_flag_spec=,' $(<)/libtool > $(<)/libtool.modified
	cp -p $(<)/libtool $(<)/libtool.orig
	cp $(<)/libtool.modified $(<)/libtool
	chmod 755 $(<)/libtool
	-diff $(<)/libtool.orig $(<)/libtool
	@echo "`date` -- INFO: ...done."
endif
	cd $(<)/ && \
	  make
	cd $(<)/ && \
	  make install
	touch $@

08.01.0200_IFLAGS=-I$(UNIXODBC_DIR)/.install/include/
BLD_IFLAGS=$($(PSQLODBC_VER)_IFLAGS)

08.01.0200_LDFLAGS=-L$(UNIXODBC_DIR)/.install/lib/
LDFLAGS += $($(PSQLODBC_VER)_LDFLAGS)

UNIXODBC_ARG=--with-unixodbc=$(UNIXODBC_DIR)/.install/
08.01.0200_UNIXODBC_ARG=--with-unixodbc
ifneq "$($(PSQLODBC_VER)_UNIXODBC_ARG)" ""
UNIXODBC_ARG=$($(PSQLODBC_VER)_UNIXODBC_ARG)
endif
UNIXODBC_ARG+=--enable-openssl

unixodbc/%: timestamp.unixODBC-%
	#
	# ---- Build psqlodbc-$(PSQLODBC_VER) ($(Bits)-bit) for unixODBC-$(UnixODBCVersion) ----
	#
	mkdir -p $(PSQLODBC_DIR)/.unixodbc$(UnixODBCVersion)
	cd $(PSQLODBC_DIR)/.unixodbc$(UnixODBCVersion) && \
	  CC="$(BLD_CC)" CFLAGS="$(strip $(CFLAGS) -DODBC$(Bits) $(BLD_IFLAGS))" LDFLAGS="$(LDFLAGS)" $(BLD_WHERE_THE_LIBRARY_THINGS_ARE)=$(UNIXODBC_DIR)/.install/lib:$($(BLD_WHERE_THE_LIBRARY_THINGS_ARE)) ../configure $(UNIXODBC_ARG) --prefix=$(INSTALL_DIR)/psqlodbc-$(PSQLODBC_VER)
ifeq "$(findstring osx,$(BLD_ARCH))" "osx"
	@echo "`date` -- INFO: Modifying the psqlodbc_la_LIBADD line in Makefile due to OS X bug..."
	sed 's,^psqlodbcw_la_LIBADD.*,psqlodbcw_la_LIBADD = -lpq -lssl -lodbcinst,' $(PSQLODBC_DIR)/.unixodbc$(UnixODBCVersion)/Makefile > $(PSQLODBC_DIR)/.unixodbc$(UnixODBCVersion)/Makefile.tmp
	mv $(PSQLODBC_DIR)/.unixodbc$(UnixODBCVersion)/Makefile $(PSQLODBC_DIR)/.unixodbc$(UnixODBCVersion)/Makefile.orig
	mv $(PSQLODBC_DIR)/.unixodbc$(UnixODBCVersion)/Makefile.tmp $(PSQLODBC_DIR)/.unixodbc$(UnixODBCVersion)/Makefile
	-diff $(PSQLODBC_DIR)/.unixodbc$(UnixODBCVersion)/Makefile.orig $(PSQLODBC_DIR)/.unixodbc$(UnixODBCVersion)/Makefile
	rm $(PSQLODBC_DIR)/.unixodbc$(UnixODBCVersion)/Makefile.orig
	@echo "`date` -- INFO: ...done."
endif
	cd $(PSQLODBC_DIR)/.unixodbc$(UnixODBCVersion) && \
	  make
	#
	# ---- Populate install directory ----
	#
	mkdir -p $(INSTALL_DIR_LOCAL)/odbc/psqlodbc-$(PSQLODBC_VER)/unixodbc-$(UnixODBCVersion)
	cp -f $(PSQLODBC_DIR)/readme.txt $(INSTALL_DIR_LOCAL)/odbc/psqlodbc-$(PSQLODBC_VER)/
	cp -f $(PSQLODBC_DIR)/license.txt $(INSTALL_DIR_LOCAL)/odbc/psqlodbc-$(PSQLODBC_VER)/
	cp -f $(PSQLODBC_DIR)/.unixodbc$(UnixODBCVersion)/.libs/psqlodbcw.so $(INSTALL_DIR_LOCAL)/odbc/psqlodbc-$(PSQLODBC_VER)/unixodbc-$(UnixODBCVersion)/
	# copy the include files for unixODBC
	cp -rf $(UNIXODBC_INC_DIR) $(INSTALL_DIR_LOCAL)/odbc/psqlodbc-$(PSQLODBC_VER)/unixodbc-$(UnixODBCVersion)/
	$(BLD_WHERE_THE_LIBRARY_THINGS_ARE)=$(UNIXODBC_DIR)/.install/lib:$($(BLD_WHERE_THE_LIBRARY_THINGS_ARE)) $(LDD) $(INSTALL_DIR_LOCAL)/odbc/psqlodbc-$(PSQLODBC_VER)/unixodbc-$(UnixODBCVersion)/psqlodbcw.so > LibList.full
	cat LibList.full
	cat LibList.full | $(LDD_FILTER) | $(GREP) -E '(libgcc_s|libodbc|libodbcinst)'| $(GREP) -E -v '^(|/usr)/lib(|64)/' > LibList.filtered
	cat LibList.filtered
	LibList=`cat LibList.filtered`; \
	if [ ! -z "$${LibList}" ]; then \
	  for i in $${LibList}; do \
	    ls -1 $$i*$(LDD_LIB_SUFFIX); \
	    (cd `dirname $$i`/ && $(TAR) cf - `basename $$i`*) | (cd $(INSTALL_DIR_LOCAL)/odbc/psqlodbc-$(PSQLODBC_VER)/unixodbc-$(UnixODBCVersion)/ && $(TAR) xpf -); \
	  done ; \
	fi

DriverManagerVersion=$(*F)

CLEAN_TARGETS_UNIXODBC=$(foreach pver,$(PSQLODBC_VERSIONS),$(foreach suffix,$(PSQLODBC-$(pver)_UNIXODBC_VERSIONS),$(PSQLODBC_DIR)$(pver)/.unixodbc$(suffix)))

CLEAN_TARGETS=$(CLEAN_TARGETS_UNIXODBC)

clean:
	@for i in $(CLEAN_TARGETS) $(INSTALL_DIR_LOCAL); do \
	  if [ -e $$i ]; then \
	    echo rm -rf $$i; \
	    rm -rf $$i; \
	  fi; \
	done
	for i in $($(OS)_UNIXODBC_VERSIONS); do \
	  (cd unixODBC-$$i && if [ -f Makefile ]; then $(MAKE) clean; fi); \
	  rm -f timestamp.unixODBC-$$i; \
	done
	# this needs to be run on the win32(cygwin) build server
	#cd psqlodbc-08.04.0200/ && \
	#  . vs2005 && \
	#  nmake ANSI_VERSION=yes USE_SSPI=yes -f win32.mak CFG=Release CLEAN
	#cd psqlodbc-08.04.0200/ && \
	#  . vs2005 && \
	#  nmake ANSI_VERSION=no USE_SSPI=yes -f win32.mak CFG=Release CLEAN
