ACFIRSTWORKS=""
for file in  \
	"../rudiments/autoconf/acfirstworks.m4" \
	"/usr/local/firstworks/share/rudiments/autoconf/acfirstworks.m4" \
	"/usr/share/rudiments/autoconf/acfirstworks.m4"
do
	if ( test -r "$file" )
	then
		ACFIRSTWORKS="$file"
		break;
	fi
done
if ( test -z "$ACFIRSTWORKS" )
then
	echo "acfirstworks.m4 not found"
	exit 0
fi

libtoolize --copy --force
cat /usr/share/aclocal/libtool.m4 > aclocal.m4
cat /usr/share/aclocal/ltoptions.m4 >> aclocal.m4
cat /usr/share/aclocal/ltsugar.m4 >> aclocal.m4
cat /usr/share/aclocal/ltversion.m4 >> aclocal.m4
cat /usr/share/aclocal/lt~obsolete.m4 >> aclocal.m4
cat "$ACFIRSTWORKS" >> aclocal.m4
autoconf
autoheader
sed -i -e "s/beos\*/beos\* | haiku\*/g" configure
