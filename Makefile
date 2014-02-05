PREFIX?=	/usr/local
CC?=		clang
CFLAGS=		-I/usr/local/include
LDFLAGS=	-L/usr/local/lib -lsqlite3
man8dir?=	/usr/local/man/man7

all: pkg-compare

pkg-compare:
	${CC} -std=c99 -O2 -Wall -Werror ${CFLAGS} ${LDFLAGS} -o pkg-compare map_lib.c pkg-compare.c

clean:
	rm -f pkg-compare

install:
	install -m 0755 -g wheel -o root pkg-compare ${PREFIX}/bin
	-install -m 0644 -g wheel -o root pkg-compare.8 ${man8dir}

deinstall:
	rm -f ${PREFIX}/bin/pkg-compare
	rm -f ${man8dir}/pkg-compare.8

