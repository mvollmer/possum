INCLUDES =  -I .. -I $(PYTHON_INCLUDES)

lib_LTLIBRARIES = libpossum.la
libpossum_la_SOURCES = sim.h \
		       comp.cc \
		       engine.cc

pkgpyexec_LTLIBRARIES = engine.la
engine_la_SOURCES = enginemodule.cc
engine_la_LIBADD = libpossum.la
engine_la_LDFLAGS = -avoid-version -module

pkgpython_DATA = __init__.py
