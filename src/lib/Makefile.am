lib_LTLIBRARIES = libmirror.la
libmirror_la_SOURCES = mirror.c interp.c memory.c  \
        primitive.c dynload.c linklst.c tree.c \
        compiler.c builtins.c \
        mirror_parser.y mirror_lexer.l \
        dict.c compiler.h  dict.h     interp.h  \
	memory.h  mirror_parser.h  tree.h \
	config.h    dynload.h  linklst.h \
        mirror.h  primitive.h \
        builtins/os.c builtins/object.c builtins/fileconn.c builtins/bytearray.c \
	builtins/float.c builtins/integer.c builtins/string.c builtins/char.c \
	builtins/context.c builtins/file.c builtins/process.c builtins/time.c \
	builtins/regexp.c builtins/signal.c builtins/socket.c builtins/system.c \
	builtins/weakref.c
        
libmirror_la_LDFLAGS=-module -no-undefined
AM_YFLAGS = -d
AM_CFLAGS = -g

CLEANFILES = mirror_parser.c mirror_parser.tab.h mirror_lexer.c
