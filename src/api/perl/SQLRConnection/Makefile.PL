# Generated automatically from Makefile.PL.in by configure.
# Generated automatically from Makefile.PL.in by configure.
use ExtUtils::MakeMaker;
require Config;
$Verbose = 1;

WriteMakefile(
	#'PREFIX'	=> "/usr/local/firstworks",
	'NAME'		=> "Firstworks::SQLRConnection",
	'VERSION_FROM'	=> "SQLRConnection.pm",
	'CC'		=> "c++",
	'CCFLAGS'	=> "-pedantic",
	'INC'		=> "-I../../../.. -I../../../common -I../../c++/include -I/usr/local/firstworks/include",
	#'LIBS'		=> ["-L../../c++/src/.libs -lsqlrclient -L/usr/local/firstworks/lib -lrudiments  -lstdc++-3-libc6.2-2-2.10.0 -lc "],
	'LIBS'		=> ["-L../../c++/src/.libs -lsqlrclient -L/usr/local/firstworks/lib -lrudiments "],
	'XSPROTOARG'	=> "-noprototypes",
	#'LD'		=> "ld",
	'LD'		=> "c++",
	#'LDDLFLAGS'	=> "-G",
	#'OBJECT'	=> '$(O_FILES)',
	'TYPEMAPS'	=> ["perlobject.map"],
	'realclean'	=> {'FILES' => 'perl'},
	'XSOPT'		=> '-C++',
);
