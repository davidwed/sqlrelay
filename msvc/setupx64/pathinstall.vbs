prefix = Session.Property("CustomActionData")

Set wshShell = CreateObject("WScript.Shell")

Set wshSystemEnv = wshShell.Environment("SYSTEM")

' update the PATH environment variable
wshSystemEnv("PATH") = wshSystemEnv("PATH") & ";" & prefix & "bin"

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
