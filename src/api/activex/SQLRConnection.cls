VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
  Persistable = 0  'NotPersistable
  DataBindingBehavior = 0  'vbNone
  DataSourceBehavior  = 0  'vbNone
  MTSTransactionMode  = 0  'NotAnMTSObject
END
Attribute VB_Name = "SQLRConnection"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = True



Private Sub Class_Initialize()
On Error Resume Next
  
  ' Set the defaults (these are defined as the defaults in the MSDN for ADO)
  
  
End Sub

Private Sub Class_Terminate()
On Error Resume Next
  
   
End Sub

Public Function open_connection(ByVal server As String, ByVal port As Long, _
ByVal user As String, ByVal password As String) As Long

open_connection = sqlrconnect(server, port, "", user, password, CLng(0), CLng(1))

End Function

Public Function identify(ByVal sqlrcon As Long) As String
identify = sqlrcon_identify(sqlrcon)
End Function



Public Function ping(ByVal sqlrcon As Long) As Long
ping = sqlrcon_ping(sqlrcon)
End Function


Public Sub free(ByVal sqlrcon As Long)
Call sqlrcon_free(sqlrcon)
End Sub

Public Function endSession(ByVal sqlrcon As Long) As Long
endSession = sqlrcon_endSession(sqlrcon)
End Function

Public Function suspendSession(ByVal sqlrcon As Long) As Long
suspendSession = sqlrcon_suspendSession(sqlrcon)
End Function

Public Function getConnectionPort(ByVal sqlrcon As Long) As Long
getConnectionPort = sqlrcon_getConnectionPort(sqlrcon)
End Function

Public Function getConnectionSocket(ByVal sqlrcon As Long) As String
getConnectionSocket = sqlrcon_getConnectionSocket(sqlrcon)
End Function

Public Function resumeSession(ByVal sqlrcon As Long, ByVal port As Long, ByVal socket As String) As Long
resumeSession = sqlrcon_resumeSession(sqlrcon, port, socket)
End Function

Public Function autoCommit(ByVal sqlrcon As Long) As Long
autoCommit = sqlrcon_autoCommit(sqlrcon)
End Function

Public Function autoCommitOff(ByVal sqlrcon As Long) As Long
autoCommitOff = sqlrcon_autoCommitOff(sqlrcon)
End Function

Public Function commit(ByVal sqlrcon As Long) As Long
commit = sqlrcon_commit(sqlrcon)
End Function

Public Function rollback(ByVal sqlrcon As Long) As Long
rollback = sqlrcon_rollback(sqlrcon)
End Function

Public Sub debugOn(ByVal sqlrcon As Long)
Call sqlrcon_debugOn(sqlrcon)
End Sub

Public Sub debugOff(ByVal sqlrconf As Long)
Call sqlrcon_debugOff(sqlrcon)
End Sub

Public Function getDebug(ByVal sqlrconf As Long) As Long
getDebug = sqlrcon_getDebug(sqlrconf)
End Function

