AUTOMAKE_OPTIONS = foreign
CXXFLAGS = -O2 --std=c++11

CPPFLAGS += \
    -I../include \
    -I../../eienexpr/include


INCLUDE_DIR = ../include
INCLUDE_FILES = \
    $(INCLUDE_DIR)/encoding.hpp \
    $(INCLUDE_DIR)/filesys.hpp \
    $(INCLUDE_DIR)/json.hpp \
    $(INCLUDE_DIR)/md5.hpp \
    $(INCLUDE_DIR)/smartptr.hpp \
    $(INCLUDE_DIR)/strings.hpp \
    $(INCLUDE_DIR)/system.hpp \
    $(INCLUDE_DIR)/time.hpp \
    $(INCLUDE_DIR)/system_detection.inl \
    $(INCLUDE_DIR)/func_traits.inl \
    $(INCLUDE_DIR)/func_runable.inl \
    $(INCLUDE_DIR)/func_invoker.inl \
    $(INCLUDE_DIR)/mixed_ref_specified_type.inl \
    $(INCLUDE_DIR)/is_x_funcs.inl \
    $(INCLUDE_DIR)/utilities.hpp \
    $(INCLUDE_DIR)/console.hpp \
    $(INCLUDE_DIR)/archives.hpp \
    $(INCLUDE_DIR)/compress.hpp \
    $(INCLUDE_DIR)/threads.hpp \
    $(INCLUDE_DIR)/threadtask.hpp \
    $(INCLUDE_DIR)/winux.hpp

include_HEADERS = $(INCLUDE_FILES)

lib_LTLIBRARIES = libwinux.la

libwinux_la_SOURCES = \
    encoding.cpp \
    filesys.cpp \
    json.cpp \
    md5.cpp \
    mystdint.h \
    sha1.h \
    sha1.cpp \
    smartptr.cpp \
    strings.cpp \
    system.cpp \
    time.cpp \
    utilities.cpp \
    console.cpp \
    zip.h \
    zip.cpp \
    unzip.h \
    unzip.cpp \
    ../../eienexpr/src/eienexpr.cpp \
    archives.cpp \
    compress.cpp \
    threads.cpp

libwinux_la_LIBADD = 
libwinux_la_LDFLAGS = -version-info $(SO_VERSION_CURRENT):$(SO_VERSION_REVISION):$(SO_VERSION_AGE) -no-undefined
