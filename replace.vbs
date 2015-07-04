' sanity check
if WScript.Arguments.Count < 3 then
	WScript.quit
end if

initialstring=WScript.Arguments.Item(0)
newstring=WScript.Arguments.Item(1)
infilename=WScript.Arguments.Item(2)

' create file system object
set fso=CreateObject("Scripting.FileSystemObject")

' collapse backslashes and convert slashes to backslashes
infilename=replace(infilename,"\\","\",1,-1,0)
infilename=replace(infilename,"/","\",1,-1,0)

' replace \n with \\n and \a with \\a
newstring=replace(newstring,"\n","\\n",1,-1,0)
newstring=replace(newstring,"\a","\\a",1,-1,0)

' open the file
set infile=fso.OpenTextFile(infilename)
infilecontent=infile.ReadAll()

' replace initialstring with newstring
outfilecontent=replace(infilecontent,initialstring,newstring,1,-1,0)

' write outfilecontent
fso.GetStandardStream(1).Write(outfilecontent)
