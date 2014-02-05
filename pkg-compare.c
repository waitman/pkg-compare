/*
pkg-compare.c 1.0.0.0 2014-02-05

Copyright 2014 Waitman Gobble <ns@waitman.net>
see LICENSE for details

Usage: pkg-compare [options] new_path old_path

This program reads local.sqlite in new_path and compares list of installed 
packages in local.sqlite in old_path. If a package is found in old_path which 
is not in new_path then the name of the port is printed, one per line.

Command line switches:
	-p	prefix uninstalled packages with 'pkg install'

Upgrade Strategy:

# mv /usr/local /usr/old-local
# mv /var/db/pkg /usr/db/old-pkg
# cd /usr/ports/ports-mgmt/pkg
# make
# make install clean
# pkg-compare -p /var/db/pkg /var/db/old-pkg > upgrade.sh
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
		if (showprefix) printf("pkg install ");
		printf("%s\n",argv[0]);
	}
	return 0;

}

void usage(void) {
	printf("\npkg-compare.c 1.0.0.0 2014-02-05\n\n \
Copyright 2014 Waitman Gobble <ns@waitman.net>\n \
see LICENSE for details\n\n \
Usage: pkg-compare [options] new_path old_path\n\n \
This program reads local.sqlite in new_path and compares list of installed\n \
packages in local.sqlite in old_path. If a package is found in old_path which\n \
is not in new_path then the name of the port is printed, one per line.\n\n \
Command line switches:\n \
\t-p\tprefix uninstalled packages with 'pkg install'\n\n \
Upgrade Strategy:\n\n \
# mv /usr/local /usr/old-local\n \
# mv /var/db/pkg /usr/db/old-pkg\n \
# cd /usr/ports/ports-mgmt/pkg\n \
# make\n \
# make install clean\n \
# pkg-compare -p /var/db/pkg /var/db/old-pkg > upgrade.sh\n \
# sh upgrade.sh\n\n\n");
	return;
}

int main(int argc, char **argv){

	sqlite3		*db;	/* sqlite database object */
	int	 	rc;	/* db handle */
	char new_path[1024]={0};	/* new install path */
	char old_path[1024]={0};	/* old install path */

	showprefix = 0;

	/* create and init hash */
	matrix = map_create();
	map_set(matrix,"foo","bar"); 

	if (argc==4) {
		if (strcmp(argv[1],"-p")==0) {
			showprefix = 1;		/* turn on prefix */
			sprintf(new_path,"%s/local.sqlite",argv[2]);
			sprintf(old_path,"%s/local.sqlite",argv[3]);
		} else {
			usage();
			return(1);
		}
	} else {
		if (argc==3) {
			if (strcmp(argv[1],"-p")==0) {
				usage();
				return(1);
			} else {
				sprintf(new_path,"%s/local.sqlite",argv[1]);
				sprintf(old_path,"%s/local.sqlite",argv[2]);
			}
		} else {
			usage();
			return(1);
		}
	}
	
	/* first loop */
	
	rc = sqlite3_open(new_path, &db);
	if( rc )
	{
		printf("oops cannot open %s\n",new_path);
		return 1;
	}
	rc = sqlite3_exec(db, "SELECT origin FROM packages ORDER BY origin ASC", callback, 0, NULL);
	sqlite3_close(db);


	/* second loop */
	rc = sqlite3_open(old_path, &db);
	if( rc )
	{
		printf("oops cannot open %s\n",old_path);
		return 1;
	}
	rc = sqlite3_exec(db, "SELECT origin FROM packages ORDER BY origin ASC", callbackchk, 0, NULL);
	sqlite3_close(db);

	return 0;
}

/* EOF */
