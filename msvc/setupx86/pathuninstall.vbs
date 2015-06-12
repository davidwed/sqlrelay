Set wshShell = CreateObject("WScript.Shell")

prefix = wshShell.RegRead("HKLM\SOFTWARE\SQLRelay\prefix")

' update the path
Set wshSystemEnv = wshShell.Environment("SYSTEM")
wshSystemEnv("PATH") = Replace(wshSystemEnv("PATH"),";" & prefix & "bin","",1,-1)

Set wshSystemEnv = Nothing
Set wshShell = Nothing
