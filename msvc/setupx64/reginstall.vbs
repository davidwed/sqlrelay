' sanity check
if WScript.Arguments.Count < 1 then
	WScript.quit
end if

' get prefix
prefix=WScript.Arguments.Item(WScript.Arguments.Count-1)

' install registry key
set WshShell=WScript.CreateObject("WScript.Shell")
WshShell.RegWrite "HKLM\SOFTWARE\SQLRelay\","","REG_SZ"
WshShell.RegWrite "HKLM\SOFTWARE\SQLRelay\prefix",prefix,"REG_SZ"
