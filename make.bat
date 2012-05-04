rmdir /s /q "C:\Program Files\Firstworks\include\sqlrelay"
del "C:\Program Files\Firstworks\lib\libsqlr*.lib"
del "C:\Program Files\Firstworks\bin\libsqlr*.dll"
del "C:\Program Files\Firstworks\bin\SQLR*.dll"
del "C:\Program Files\Firstworks\bin\sqlrsh.exe"
cd src\api\c++\msvc
msbuild.exe /target:Clean
msbuild.exe
cd ..\..\c\msvc
msbuild.exe /target:Clean
msbuild.exe
cd ..\..\c#\SQLRClient
msbuild.exe /target:Clean
msbuild.exe
cd ..\..\..\util\msvc
msbuild.exe /target:Clean
msbuild.exe
cd ..\..\cmdline\msvc
msbuild.exe /target:Clean
msbuild.exe
cd ..\..\..
