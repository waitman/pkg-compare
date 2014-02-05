PREFIX?=	/usr/local
CC?=		clang
CFLAGS=		-I/usr/local/include
LDFLAGS=	-L/usr/local/lib -lsqlite3
man7dir?=	/usr/local/man/man7

all: pkg-compare

pkg-compare:
	${CC} -std=c99 -O2 -Wall -Werror ${CFLAGS} ${LDFLAGS} -o pkg-compare map_lib.c pkg-compare.c

clean:
	rm -f pkg-compare

install:
	install -m 0755 -g wheel -o root pkg-compare ${STAGEDIR}${PREFIX}/bin
	-install -m 0644 -g wheel -o root pkg-compare.7 ${STAGEDIR}${man7dir}

deinstall:
	rm -f ${STAGEDIR}${PREFIX}/bin/pkg-compare
	rm -f ${STAGEDIR}${man7dir}/pkg-compare.7

