AUTOMAKE_OPTIONS = foreign
CXXFLAGS = -O2 --std=c++11
CPPFLAGS += \
    -I../include \
    -I../../winux/include

INCLUDE_DIR = ../include
INCLUDE_FILES = \
    $(INCLUDE_DIR)/eienexpr.hpp \
    $(INCLUDE_DIR)/eienexpr_errstrs.inl

include_HEADERS = $(INCLUDE_FILES)

lib_LTLIBRARIES = libeienexpr.la

libeienexpr_la_SOURCES = eienexpr.cpp

libeienexpr_la_LIBADD = -lwinux
libeienexpr_la_LDFLAGS = -L../../winux/src/.libs -no-undefined
