Set wshShell = CreateObject("WScript.Shell")
Set wshSystemEnv = wshShell.Environment("USER")
'WScript.Echo wshSystemEnv("PATH")
wshSystemEnv("PATH") = Replace(wshSystemEnv("PATH"),";C:\Program Files (x86)\Firstworks\bin","")
'WScript.Echo wshSystemEnv("PATH")
Set wshSystemEnv = Nothing
Set wshShell = Nothing
