Set wshShell = CreateObject("WScript.Shell")
Set wshSystemEnv = wshShell.Environment("SYSTEM")
'WScript.Echo wshSystemEnv("PATH")
wshSystemEnv("PATH") = Replace(wshSystemEnv("PATH"),";C:\Program Files\Firstworks\bin","") & ";C:\Program Files\Firstworks\bin"
'WScript.Echo wshSystemEnv("PATH")
Set wshSystemEnv = Nothing
Set wshShell = Nothing
