' sanity check
if WScript.Arguments.Count < 2 then
	WScript.quit
end if

' create file system object
set fso=CreateObject("Scripting.FileSystemObject")

' get the destination
dest=WScript.Arguments.Item(WScript.Arguments.Count-1)

' if it's a folder, append a backslash
if fso.FolderExists(dest) then
	dest=dest+"\"
end if

' collapse backslashes and convert slashes to backslashes
dest=replace(dest,"\\","\",1,-1,0)
dest=replace(dest,"/","\",1,-1,0)

' copy source files to destination...
for i=0 to WScript.Arguments.Count-2

	' get a source file
	source=WScript.Arguments.Item(i)

	' collapse backslashes and convert slashes to backslashes
	source=replace(source,"\\","\",1,-1,0)
	source=replace(source,"/","\",1,-1,0)

	' copy the file
	call fso.CopyFile(source,dest)
next
