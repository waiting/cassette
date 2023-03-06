AUTOMAKE_OPTIONS = foreign
CFLAGS = \
    -O2 \
    -std=c99 \
    -DTHREADSAFE=1 \
    -DSQLITE_THREADSAFE=1 \
    -DSQLITE_MAX_ATTACHED=10 \
    -DSQLITE_SOUNDEX \
    -DSQLITE_ENABLE_COLUMN_METADATA \
    -DSQLITE_HAS_CODEC \
    -DSQLITE_SECURE_DELETE \
    -DSQLITE_ENABLE_FTS3 \
    -DSQLITE_ENABLE_FTS3_PARENTHESIS \
    -DSQLITE_ENABLE_FTS4 \
    -DSQLITE_ENABLE_FTS5 \
    -DSQLITE_ENABLE_JSON1 \
    -DSQLITE_ENABLE_RTREE \
    -DSQLITE_ENABLE_GEOPOLY \
    -DSQLITE_CORE \
    -DSQLITE_ENABLE_EXTFUNC \
    -DSQLITE_ENABLE_CSV \
    -DSQLITE_ENABLE_SHA3 \
    -DSQLITE_ENABLE_CARRAY \
    -DSQLITE_ENABLE_FILEIO \
    -DSQLITE_ENABLE_SERIES \
    -DSQLITE_ENABLE_UUID \
    -DSQLITE_TEMP_STORE=2 \
    -DSQLITE_USE_URI \
    -DSQLITE_USER_AUTHENTICATION 

CPPFLAGS += -I../include

INCLUDE_DIR = ../include

INCLUDE_FILES = \
    $(INCLUDE_DIR)/sqlite3.h \
    $(INCLUDE_DIR)/sqlite3secure.h


lib_LTLIBRARIES = libsqlite3secure.la

libsqlite3secure_la_SOURCES = \
    sqlite3secure.c

libsqlite3secure_la_LIBADD = -lpthread -ldl -lm
libsqlite3secure_la_LDFLAGS = -no-undefined

#includedir = @includedir@/sqlite3secure
include_HEADERS = $(INCLUDE_FILES)

noinst_HEADERS = \
    carray.c \
    chacha20poly1305.c \
    codec.c \
    codecext.c \
    csv.c \
    extensionfunctions.c \
    fastpbkdf2.c \
    fileio.c \
    md5.c \
    regexp.c \
    rekeyvacuum.c \
    rijndael.c \
    series.c \
    sha1.c \
    sha2.c \
    shathree.c \
    shell.c \
    sqlite3.c \
    test_windirent.c \
    userauth.c \
    uuid.c \
    codec.h \
    fastpbkdf2.h \
    mystdint.h \
    rijndael.h \
    sha1.h \
    sha2.h \
    sqlite3.h \
    sqlite3ext.h \
    sqlite3secure.h \
    sqlite3userauth.h \
    test_windirent.h

EXTRA_DIST = \
    sqlite3.def \
    sqlite370.ico \
    filelist.md \
    sqlite3.rc \
    sqlite3shell.rc
