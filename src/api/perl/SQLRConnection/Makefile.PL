# Generated automatically from Makefile.PL.in by configure.
# Generated automatically from Makefile.PL.in by configure.
use ExtUtils::MakeMaker;
require Config;
$Verbose = 1;

WriteMakefile(
	'NAME'		=> "Firstworks::SQLRConnection",
	'VERSION_FROM'	=> "SQLRConnection.pm",
	'CC'		=> "c++",
	'CCFLAGS'	=> "-pedantic -w -O2 -fPIC",
	'INC'		=> "-I../../../.. -I../../../common -I../../c++/include -I/usr/local/firstworks/include",
	'LIBS'		=> ["-L../../c++/src/.libs -lsqlrclient -L/usr/local/firstworks/lib -lrudiments  -lstdc++-3-libc6.2-2-2.10.0 -lc "],
	'XSPROTOARG'	=> "-noprototypes",
	'LD'		=> "ld",
	'LDDLFLAGS'	=> "-G",
	'OBJECT'	=> '$(O_FILES)',
	'TYPEMAPS'	=> ["perlobject.map"],
	'realclean'	=> {'FILES' => 'perl'},
);
