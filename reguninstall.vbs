' uninstall registry key
set WshShell=WScript.CreateObject("WScript.Shell")

key="HKLM\SoftWARE\SQLRelay\prefix"

on error resume next
value = WshShell.RegRead(key)
if not IsNull(value) then
	WshShell.RegDelete key
end if

key="HKLM\SoftWARE\SQLRelay\"

on error resume next
value = WshShell.RegRead(key)
if not IsNull(value) then
	WshShell.RegDelete key
end if
