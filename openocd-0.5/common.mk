# common flags used in openocd build
AM_CPPFLAGS  =  -I$(top_srcdir) \
			          -I$(top_srcdir)/jimtcl \
                -I$(top_builddir)/jimtcl \
                -I$(top_srcdir)/helper \
                -I$(top_builddir)/helper \
			          -DPKGDATADIR=\"$(pkgdatadir)\" \
			          -DPKGLIBDIR=\"$(pkglibdir)\"
