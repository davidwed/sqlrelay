libtoolize --copy --force
cat /usr/share/aclocal/libtool.m4 > aclocal.m4
cat /usr/share/aclocal/ltoptions.m4 >> aclocal.m4
cat /usr/share/aclocal/ltsugar.m4 >> aclocal.m4
cat /usr/share/aclocal/ltversion.m4 >> aclocal.m4
cat /usr/share/aclocal/lt~obsolete.m4 >> aclocal.m4

for file in acfirstworks.m4 acrudiments.m4
do
	foundfile=""
	for fullfile in  \
		"../rudiments/autoconf/$file" \
		"/usr/local/firstworks/share/rudiments/autoconf/$file" \
		"/usr/share/rudiments/autoconf/$file"
	do
		if ( test -r "$fullfile" )
		then
			foundfile="$fullfile"
			break;
		fi
	done
	if ( test -z "$foundfile" )
	then
		echo "acfirstworks.m4 not found"
		exit 0
	fi
	cat $foundfile >> aclocal.m4
done

autoconf
autoheader
sed -i -e "s/beos\*/beos\* | haiku\*/g" configure
