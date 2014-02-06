/*

pkg-compare.c 1.1 2014-02-05

Copyright 2014 Waitman Gobble <ns@waitman.net>
see LICENSE for details

Usage: pkg-compare [options] new_path old_path

This program reads local.sqlite in new_path and compares list of installed 
packages in local.sqlite in old_path. If a package is found in old_path which 
is not in new_path then the name of the port is printed, one per line.

Command line switches:
	-p	prefix uninstalled packages with 'pkg install -Rf'
	-m	show manually installed packages only, no deps

Upgrade Strategy:

# mv /usr/local /usr/old-local
# mv /var/db/pkg /var/db/old-pkg
# cd /usr/ports/ports-mgmt/pkg
# make
# make install clean
# pkg-compare -p -m /var/db/pkg /var/db/old-pkg > upgrade.sh
# sh upgrade.sh

*/

#include <stdio.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "map_lib.h"

int showprefix; /* print 'pkg install' before package name */
int showmanual; /* only show manually installed packages */

struct map_t *matrix;

static int callback(void *NotUsed, int argc, char **argv, char **coln) {

	/* first loop set matrix */
	map_set(matrix,argv[0],"+");
	return 0;

}

static int callbackchk(void *NotUsed, int argc, char **argv, char **coln) {

	/* second loop check to see if we already have it */
	char *a=NULL;
	a=map_get(matrix,argv[0]);
	if (a==NULL) {
		if (showprefix) printf("pkg install -Rf ");
		printf("%s\n",argv[0]);
	}
	return 0;

}

void usage(void) {
	printf("\n\npkg-compare.c 1.1 2014-02-05\n\n \
Copyright 2014 Waitman Gobble <ns@waitman.net>\n \
see LICENSE for details\n\n \
Usage: pkg-compare [options] new_path old_path\n\n \
This program reads local.sqlite in new_path and compares list of installed\n \
packages in local.sqlite in old_path. If a package is found in old_path which\n \
is not in new_path then the name of the port is printed, one per line.\n\n \
Command line switches:\n \
\t-p\tprefix uninstalled packages with 'pkg install -Rf'\n \
\t-m\tshow manually installed packages only, no deps\n\n \
Upgrade Strategy:\n\n \
# mv /usr/local /usr/old-local\n \
# mv /var/db/pkg /var/db/old-pkg\n \
# cd /usr/ports/ports-mgmt/pkg\n \
# make\n \
# make install clean\n \
# pkg-compare -p -m /var/db/pkg /var/db/old-pkg > upgrade.sh\n \
# sh upgrade.sh\n\n\n");
	return;
}

int main(int argc, char **argv){

	sqlite3		*db;		/* sqlite database object */
	int	 	rc;		/* db handle */
	int		c;		/* command line params */
	int		more=1;		/* decrement argc if switches */
	char new_path[1024]={0};	/* new install path */
	char old_path[1024]={0};	/* old install path */

	showprefix = 0;
	showmanual = 0;

	/* create and init hash */
	matrix = map_create();
	map_set(matrix,"foo","bar"); 

	/* get command line switches */
	while ((c = getopt (argc, argv, "pm")) != -1) {
		switch (c) {
			case 'p': /* use prefix */
				showprefix = 1;
				++more; /* discount this param */
				printf("showprefix\n");
				break;
			case 'm': /* only manually installed pkgs */
				showmanual = 1;
				printf("showmanual\n");
				++more; /* discount this param */
				break;
			default:
				break;
		}
	}

	if (argc<(2+more)) {
		usage();
		return 1; /* bye */
	}
		
	printf("%i\n",argc);
	sprintf(old_path,"%s/local.sqlite",argv[--argc]);
	sprintf(new_path,"%s/local.sqlite",argv[--argc]);

	/* first loop */
	
	rc = sqlite3_open(new_path, &db);
	if( rc )
	{
		printf("ERROR: cannot open %s\n \
Check that the path is correct, then \
try again.\n\n",new_path);
		return 1; /* bye */
	}
	
	if (showmanual) {
		/* this appears to return sames results as 
			pkg query -e '%a == 0' %o */
		rc = sqlite3_exec(db, "SELECT origin FROM packages a \
			WHERE NOT EXISTS (SELECT 1 FROM deps b WHERE \
			a.origin = b.origin) ORDER BY origin ASC", 
			callback, 0, NULL);
	} else {
		/* show all installed packages */
		rc = sqlite3_exec(db, "SELECT origin FROM packages \
			ORDER BY origin ASC",
			callback, 0, NULL);
	}
	sqlite3_close(db);


	/* second loop */
	rc = sqlite3_open(old_path, &db);
	if( rc )
	{
		printf("ERROR: cannot open %s\n \
Check that the path is correct \
and try again.\n\n",old_path);
		return 1; /* bye */
	}
	if (showmanual) {
		/* this appears to return sames results as 
			pkg query -e '%a == 0' %o */
		rc = sqlite3_exec(db, "SELECT origin FROM packages a \
			WHERE NOT EXISTS (SELECT 1 FROM deps b WHERE \
			a.origin = b.origin) ORDER BY origin ASC", 
			callbackchk, 0, NULL);
	} else { 
		/* show all installed packages */
		rc = sqlite3_exec(db, "SELECT origin FROM packages \
			ORDER BY origin ASC",
			callbackchk, 0, NULL);
	} 

	sqlite3_close(db);

	return 0;
}

/* EOF */
