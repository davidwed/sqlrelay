libtoolize --copy --force
cat /usr/share/aclocal/libtool.m4 > aclocal.m4
cat /usr/share/aclocal/ltoptions.m4 >> aclocal.m4
cat /usr/share/aclocal/ltsugar.m4 >> aclocal.m4
cat /usr/share/aclocal/ltversion.m4 >> aclocal.m4
cat /usr/share/aclocal/lt~obsolete.m4 >> aclocal.m4
autoconf
autoheader
sed -i -e "s/beos\*/beos\* | haiku\*/g" configure
