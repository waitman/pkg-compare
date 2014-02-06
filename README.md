```
pkg-compare.c 1.1 2014-02-05

Copyright 2014 Waitman Gobble <ns@waitman.net>
see LICENSE for details

Usage: pkg-compare [options] new_path old_path

This program reads local.sqlite in new_path and compares list of installed
packages in local.sqlite in old_path. If a package is found in old_path which
is not in new_path then the name of the port is printed, one per line.

Command line switches:
        -p      prefix uninstalled packages with 'pkg install -Rf'
	-m	show manually installed packages only, no deps

Upgrade Strategy:

# mv /usr/local /usr/old-local
# mv /var/db/pkg /var/db/old-pkg
# cd /usr/ports/ports-mgmt/pkg
# make
# make install clean
# pkg-compare -p -m /var/db/pkg /var/db/old-pkg > upgrade.sh
# sh upgrade.sh

```
