AUTOMAKE_OPTIONS = foreign
CXXFLAGS = -O2 --std=c++11
DEFS += -DSQLITE_HAS_CODEC -DHAVE_SQLITE_DRIVER -DHAVE_MYSQL_DRIVER

CPPFLAGS += \
    -I../include \
    -I../../winux/include \
    -I../../../thirdparties/sqlite3secure/include \
    -I@mysql_header_dir@

INCLUDE_DIR = ../include
INCLUDE_FILES = \
    $(INCLUDE_DIR)/eiendb.hpp \
    $(INCLUDE_DIR)/eiendb_base.hpp \
    $(INCLUDE_DIR)/eiendb_common.hpp \
    $(INCLUDE_DIR)/eiendb_mysql.hpp \
    $(INCLUDE_DIR)/eiendb_sqlite.hpp

include_HEADERS = $(INCLUDE_FILES)

lib_LTLIBRARIES = libeiendb.la

libeiendb_la_SOURCES = \
    eiendb.cpp \
    eiendb_base.cpp \
    eiendb_common.cpp \
    eiendb_mysql.cpp \
    eiendb_sqlite.cpp

libeiendb_la_LIBADD = -lwinux -lsqlite3secure -lmysqlclient
libeiendb_la_LDFLAGS = \
    -L../../winux/src/.libs \
    -L../../../thirdparties/sqlite3secure/src/.libs \
    -L@mysql_lib_dir@ \
    -no-undefined
