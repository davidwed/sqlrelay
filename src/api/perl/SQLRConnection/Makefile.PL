# Generated automatically from Makefile.PL.in by configure.
use ExtUtils::MakeMaker;
require Config;
$Verbose = 1;

WriteMakefile(
	#'PREFIX'	=> "/usr/local/firstworks",
	'NAME'		=> "Firstworks::SQLRConnection",
	'VERSION_FROM'	=> "SQLRConnection.pm",
	'CC'		=> "g++",
	'CCFLAGS'	=> "-pedantic",
	'INC'		=> "-I../../../.. -I../../../common -I../../c++/include -I/usr/local/firstworks/include",
	'LIBS'		=> ["-L../../c++/src/.libs -lsqlrclient -L/usr/local/firstworks/lib -lrudiments  "],
	'XSPROTOARG'	=> "-noprototypes",
	#'LD'		=> "g++",
	'LD'		=> "g++",
	#'LDDLFLAGS'	=> "-Wl,-G",
	#'OBJECT'	=> '$(O_FILES)',
	'TYPEMAPS'	=> ["perlobject.map"],
	'realclean'	=> {'FILES' => 'perl'},
	'XSOPT'		=> '-C++',
);
