' sanity check
if WScript.Arguments.Count<1 then
	WScript.quit
end if

' create file system object
set fso=CreateObject("Scripting.FileSystemObject")

' remove folders
for i=0 to WScript.Arguments.Count-1

	' get the folder to remove
	folder=WScript.Arguments.Item(i)

	' collapse backslashes and convert slashes to backslashes
	folder=replace(folder,"\\","\",1,-1,0)
	folder=replace(folder,"/","\",1,-1,0)

	' remove the folder (ignoring errors)
	on error resume next
	call fso.DeleteFolder(folder,true)
next
