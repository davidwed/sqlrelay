if WScript.Arguments.Count<1 then
	WScript.quit
end if

prefix = WScript.Arguments.Item(WScript.Arguments.Count-1)

Set wshShell = CreateObject("WScript.Shell")
Set wshSystemEnv = wshShell.Environment("SYSTEM")

' update the PATH environment variable
WScript.Echo wshSystemEnv("PATH")
wshSystemEnv("PATH") = wshSystemEnv("PATH") & ";" & prefix & "bin"
WScript.Echo wshSystemEnv("PATH")

' update permissions on some folders
wshShell.run "cacls.exe """ & prefix & "var\sqlrelay\cache"" /E /G Everyone:C"
wshShell.run "cacls.exe """ & prefix & "var\sqlrelay\debug"" /E /G Everyone:C"
wshShell.run "cacls.exe """ & prefix & "var\sqlrelay\log"" /E /G Everyone:C"
wshShell.run "cacls.exe """ & prefix & "var\sqlrelay\tmp"" /E /G Everyone:C"
wshShell.run "cacls.exe """ & prefix & "var\sqlrelay\tmp\ipc"" /E /G Everyone:C"
wshShell.run "cacls.exe """ & prefix & "var\sqlrelay\tmp\pids"" /E /G Everyone:C"
wshShell.run "cacls.exe """ & prefix & "var\sqlrelay\tmp\sockets"" /E /G Everyone:C"

Set wshSystemEnv = Nothing
Set wshShell = Nothing
