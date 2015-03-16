' sanity check
if WScript.Arguments.Count < 1 then
	WScript.quit
end if

' create file system object
set fso=CreateObject("Scripting.FileSystemObject")

' get the directory to create
fullpath=WScript.Arguments.Item(0)

' collapse backslashes and convert slashes to backslashes
fullpath=replace(fullpath,"\\","\",1,-1,0)
fullpath=replace(fullpath,"/","\",1,-1,0)

' split on backslashes
parts=split(fullpath,"\")

' create directories
path=""
for i=lbound(parts) to ubound(parts)

	' build path
	if strcomp(path,"")=0 then
		path=parts(i)
	else
		path=path+"\"+parts(i)
	end if

	' create the directory unless it already exists
	if fso.FolderExists(path)=false then
		call fso.CreateFolder(path)
	end if
next
