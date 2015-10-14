@echo off

setlocal enableextensions enabledelayedexpansion

set DB=%1

if [%DB%]==[] (
	echo usage: test database
	goto:end
)

for %%D in (c c++ cs java nodejs perl perldbi php phppdo python pythondb ruby tcl) do (
	call:runtest %%D
	if !ERRORLEVEL!==1 (
		goto:end
	)
)

goto:end

:runtest
	set DIR=%1

	echo testing in %DIR%
	echo.
	cd %DIR%

	set TEST=[]
	set TESTFILE=[]
	goto case_%DIR%
		:case_c
			set TEST=%DB%.exe
			set TESTFILE=%DB%.exe
			goto:endswitch
		:case_c++
			set TEST=%DB%.exe
			set TESTFILE=%DB%.exe
			goto:endswitch
		:case_cs
			set TEST=%DB%.exe
			set TESTFILE=%DB%.exe
			goto:endswitch
		:case_java
			set TEST=call run %DB%
			set TESTFILE=%DB%.class
			goto:endswitch
		:case_nodejs
			set TEST=node %DB%.js
			set TESTFILE=%DB%.js
			goto:endswitch
		:case_perl
			set TEST=perl %DB%.pl
			set TESTFILE=%DB%.pl
			goto:endswitch
		:case_perldbi
			set TEST=perl %DB%.pl
			set TESTFILE=%DB%.pl
			goto:endswitch
		:case_php
			set TEST=php %DB%.php
			set TESTFILE=%DB%.php
 			goto:endswitch
		:case_phppdo
			set TEST=php %DB%.php
			set TESTFILE=%DB%.php
			goto:endswitch
		:case_python
			set TEST=python %DB%.py
			set TESTFILE=%DB%.py
			goto:endswitch
		:case_pythondb
			set TEST=python %DB%.py
			set TESTFILE=%DB%.py
			goto:endswitch
		:case_ruby
			set TEST=ruby %DB%.rb
			set TESTFILE=%DB%.rb
			goto:endswitch
		:case_tcl
			set TEST=tclsh %DB%.tcl
			set TESTFILE=%DB%.tcl
			goto:endswitch
	:endswitch

	if exist %TESTFILE% (
		%TEST%
		if !ERRORLEVEL!==1 (
			echo.
			echo.
			echo %DB% failed in %DIR%
			goto:eof
		) else (
			echo.
			echo "test complete"
		)
	) else (
		echo no test found for %DB% in %DIR%
	)

	echo.
	echo ================================================================================
	echo.

	cd ..
goto:eof

:end

endlocal
