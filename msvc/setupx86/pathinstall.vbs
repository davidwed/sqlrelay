Set wshShell = CreateObject("WScript.Shell")
Set wshSystemEnv = wshShell.Environment("SYSTEM")

' update the PATH environment variable
'WScript.Echo wshSystemEnv("PATH")
wshSystemEnv("PATH") = Replace(wshSystemEnv("PATH"),";C:\Program Files\Firstworks\bin","") & ";C:\Program Files\Firstworks\bin"
'WScript.Echo wshSystemEnv("PATH")

' update permissions on some folders
wshShell.run "cacls.exe ""C:\Program Files\Firstworks\var\sqlrelay\cache"" /E /G Everyone:C"
wshShell.run "cacls.exe ""C:\Program Files\Firstworks\var\sqlrelay\debug"" /E /G Everyone:C"
wshShell.run "cacls.exe ""C:\Program Files\Firstworks\var\sqlrelay\log"" /E /G Everyone:C"
wshShell.run "cacls.exe ""C:\Program Files\Firstworks\var\sqlrelay\tmp"" /E /G Everyone:C"
wshShell.run "cacls.exe ""C:\Program Files\Firstworks\var\sqlrelay\tmp\ipc"" /E /G Everyone:C"
wshShell.run "cacls.exe ""C:\Program Files\Firstworks\var\sqlrelay\tmp\pids"" /E /G Everyone:C"
wshShell.run "cacls.exe ""C:\Program Files\Firstworks\var\sqlrelay\tmp\sockets"" /E /G Everyone:C"

Set wshSystemEnv = Nothing
Set wshShell = Nothing
