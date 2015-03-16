' sanity check
if WScript.Arguments.Count<1 then
	WScript.quit
end if

' create file system object
set fso=CreateObject("Scripting.FileSystemObject")

' remove files
for i=0 to WScript.Arguments.Count-1

	' get the file to remove
	file=WScript.Arguments.Item(i)

	' collapse backslashes and convert slashes to backslashes
	file=replace(file,"\\","\",1,-1,0)
	file=replace(file,"/","\",1,-1,0)
	
	' remove the file (ignoring errors)
	on error resume next
	call fso.DeleteFile(file,true)
next
