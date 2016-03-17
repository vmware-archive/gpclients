## Please update this variable if any new macros are created

MACROS=						\
  aclocal-include.m4				\
  compiler-flags.m4				\
  curses.m4					\
  gnome-bonobo-check.m4				\
  gnome-fileutils.m4				\
  gnome-ghttp-check.m4				\
  gnome-gnorba-check.m4				\
  gnome-guile-checks.m4				\
  gnome-libgtop-check.m4			\
  gnome-objc-checks.m4				\
  gnome-orbit-check.m4				\
  gnome-print-check.m4				\
  gnome-pthread-check.m4			\
  gnome-support.m4				\
  gnome-undelfs.m4				\
  gnome-vfs.m4					\
  gnome-x-checks.m4				\
  gnome-xml-check.m4				\
  gnome.m4					\
  gperf-check.m4				\
  linger.m4					\
  need-declaration.m4

EXTRA_DIST= codeset.m4 gettext.m4 glibc21.m4 iconv.m4 intdiv0.m4 inttypes.m4 inttypes_h.m4 inttypes-pri.m4 isc-posix.m4 lcmessage.m4 lib-ld.m4 lib-link.m4 lib-prefix.m4 progtest.m4 stdint_h.m4 uintmax_t.m4 $(MACROS) gnome-common.m4 autogen.sh
MAINTAINERCLEANFILES=macros.dep

@MAINT@macros.dep: Makefile.am
@MAINT@	@echo '$$(top_srcdir)/aclocal.m4: $(MACROS:%=macros/%)' > $@

if INSIDE_GNOME_COMMON
gnome_aclocaldir = $(datadir)/aclocal/gnome

gnome-macros.dep: Makefile.am
	@echo '$$(top_srcdir)/aclocal.m4: $(MACROS:%=$(gnome_aclocaldir)/%)' > $@

gnome_aclocal_DATA = $(MACROS) gnome-macros.dep gnome-common.m4 autogen.sh

endif
