Set wshShell = CreateObject("WScript.Shell")

' setup.exe is a 32-bit binary but we need to get the 64-bit version of
' HKLM\SOFTWARE\SQLRelay\prefix, so we have to do all of this instead of just
' calling wshShell.RegRead
Set oCtx = CreateObject("WbemScripting.SWbemNamedValueSet")
oCtx.Add "__ProviderArchitecture", 64
Set oReg = CreateObject("Wbemscripting.SWbemLocator").ConnectServer("","root\default","","",,,,oCtx).Get("StdRegProv")
Set oInParams = oReg.Methods_("GetStringValue").InParameters
oInParams.hDefKey = &H80000002
oInParams.sSubKeyName = "SOFTWARE\SQLRelay"
oInParams.sValueName = "prefix"

prefix = oReg.ExecMethod_("GetStringValue",oInParams,,oCtx).sValue

' update the path
Set wshSystemEnv = wshShell.Environment("SYSTEM")
wshSystemEnv("PATH") = Replace(wshSystemEnv("PATH"),";" & prefix & "bin","",1,-1)

Set wshSystemEnv = Nothing
Set wshShell = Nothing
