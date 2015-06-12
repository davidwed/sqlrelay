if WScript.Arguments.Count<1 then
	WScript.quit
end if

prefix = WScript.Arguments.Item(WScript.Arguments.Count-1)

Set wshShell = CreateObject("WScript.Shell")
Set wshSystemEnv = wshShell.Environment("SYSTEM")
'WScript.Echo wshSystemEnv("PATH")
wshSystemEnv("PATH") = Replace(wshSystemEnv("PATH"),";" & prefix & "bin","")
'WScript.Echo wshSystemEnv("PATH")
Set wshSystemEnv = Nothing
Set wshShell = Nothing
