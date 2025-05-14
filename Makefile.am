
EXTRA_DIST = \
	bootstrap \
	COPYING.LGPL \
	README.md


AM_CPPFLAGS = \
	$(CURL_CFLAGS) \
	-I$(srcdir)/include


LIBS = $(CURL_LIBS)


curlxx_HEADERS = \
	include/curlxx/basic_wrapper.hpp \
	include/curlxx/curl.hpp \
	include/curlxx/easy.hpp \
	include/curlxx/error.hpp \
	include/curlxx/escape.hpp \
	include/curlxx/header.hpp \
	include/curlxx/multi.hpp \
	include/curlxx/slist.hpp

curlxxdir = $(includedir)/curlxx


lib_LIBRARIES = libcurlxx.a


libcurlxx_a_SOURCES = \
	src/curl.cpp \
	src/easy.cpp \
	src/error.cpp \
	src/escape.cpp \
	src/header.cpp \
	src/multi.cpp \
	src/slist.cpp


.PHONY: company
company: compile_flags.txt

compile_flags.txt: Makefile
	printf "%s" "$(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS)" | xargs -n1 | sort -u > $(srcdir)/compile_flags.txt
	$(CPP) -xc++ /dev/null -E -Wp,-v 2>&1 | sed -n 's,^ ,-I,p' >> $(srcdir)/compile_flags.txt
