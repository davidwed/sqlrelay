VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
  Persistable = 0  'NotPersistable
  DataBindingBehavior = 0  'vbNone
  DataSourceBehavior  = 0  'vbNone
  MTSTransactionMode  = 0  'NotAnMTSObject
END
Attribute VB_Name = "SQLRCursor"
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

Public Function open_cursor(ByVal sqlrcon As Long) As Long

open_cursor = sqlrcursor(sqlrcon)
'we set copyrefs = 1 to reserve the variables transferred from VB code
Call sqlrcur_copyReferences(open_cursor)

End Function

Public Sub free(ByVal sqlrcon As Long)
Call sqlrcur_free(sqlrcon)
End Sub

Public Sub setResultSetBufferSize(ByVal sqlrcur As Long, ByVal rows As Long)
Call sqlrcur_setResultSetBufferSize(sqlrcur, rows)
End Sub

Public Function getResultSetBufferSize(ByVal sqlrcu As Long) As Long
getResultSetBufferSize = sqlrcur_getResultSetBufferSize(sqlrcur)
End Function

Public Sub dontGetColumnInfo(ByVal sqlrcur As Long)
  Call sqlrcur_dontGetColumnInfo(sqlrcur)
End Sub

Public Sub getColumnInfo(ByVal sqlrcur As Long)
Call sqlrcur_getColumnInfo(sqlrcur)
End Sub

Public Sub mixedCaseColumnNames(ByVal sqlrcur As Long)
Call sqlrcur_mixedCaseColumnNames(sqlrcur)
End Sub

Public Sub upperCaseColumnNames(ByVal sqlrcur As Long)
Call sqlrcur_upperCaseColumnNames(sqlrcur)
End Sub

Public Sub lowerCaseColumnNames(ByVal sqlrcur As Long)
Call sqlrcur_lowerCaseColumnNames(sqlrcur)
End Sub
Public Sub cacheToFile(ByVal sqlrcur As Long, ByVal filename As String)
Call sqlrcur_cacheToFile(sqlrcur, filename)
End Sub
Public Sub setCacheTtl(ByVal sqlrcur As Long, ByVal ttl As Long)
Call sqlrcur_setCacheTtl(sqlrcur, ttl)
End Sub

Public Function getCacheFileName(ByVal sqlrcur As Long) As String
getCacheFileName = sqlrcur_getCacheFileName(sqlrcur)
End Function

Public Sub cacheOff(ByVal sqlrcur As Long)
Call sqlrcur_cacheOff(sqlrcur)
End Sub

Public Function sendQuery(ByVal sqlrcur As Long, ByVal query As String) As Long
sendQuery = sqlrcur_sendQuery(sqlrcur, query)
End Function

Public Function sendQueryWithLength(ByVal sqlrcur As Long, ByVal query As String, _
ByVal length As Long) As Long
sendQueryWithLength = sendQueryWithLength(sqlrcur, query, length)
End Function

Public Function sendFileQuery(ByVal sqlrcur As Long, ByVal path As String, _
ByVal filename As String) As Long
sendFileQuery = sqlrcur_sendFileQuery(sqlrcur, path, filename)
End Function

Public Sub prepareQuery(ByVal sqlrcur As Long, ByVal query As String)
query = query & Chr(0)
Call sqlrcur_prepareQuery(sqlrcur, query)
End Sub

Public Sub prepareQueryWithLength(ByVal sqlrcur As Long, ByVal query As String, _
ByVal length As Long)
Call sqlrcur_prepareQueryWithLength(sqlrcur, query, length)
End Sub

Public Sub subString(ByVal sqlrcur As Long, ByVal variable As String, ByVal value As String)
variable = variable & Chr(0)
value = value & Chr(0)
Call sqlrcur_subString(sqlrcur, variable, value)
End Sub


Public Sub subLong(ByVal sqlrcur As Long, ByVal variable As String, ByVal value As Long)
Call subLong(sqlrcur, variable, value)
End Sub

Public Sub subDouble(ByVal sqlrcur As Long, ByVal variable As String, _
ByVal value As Double, _
ByVal precision As Integer, _
ByVal v_scale As Integer)
Call sqlrcur_subDouble(sqlrcur, variable, value, precision, v_scale)
End Sub

Public Sub clearBinds(ByVal sqlrcur As Long)
Call sqlrcur_clearBinds(sqlrcur)
End Sub

Public Sub inputBindString(ByVal sqlrcur As Long, _
ByVal variable As String, _
ByVal value As String)
variable = variable & Chr(0)
value = value & Chr(0)
Call sqlrcur_inputBindString(sqlrcur, variable, value)
End Sub

Public Sub inputBindLong(ByVal sqlrcur As Long, ByVal variable As String, ByVal value As Long)
variable = variable & Chr(0)
Call sqlrcur_inputBindLong(sqlrcur, variable, value)
End Sub

Public Sub inputBindDouble(ByVal sqlrcur As Long, ByVal variable As String, ByVal value As Double, _
ByVal precision As Integer, ByVal v_scale As Integer)
variable = variable & Chr(0)
Call sqlrcur_inputBindDouble(sqlrcur, variable, value, precision, v_scale)
End Sub

Public Sub inputBindBlob(ByVal sqlrcur As Long, ByVal variable As String, ByVal value As String, _
ByVal size As Long)
Call sqlrcur_inputBindBlob(sqlrcur, variable, value, size)
End Sub

Public Sub inputBindClob(ByVal sqlrcur As Long, ByVal variable As String, ByVal value As String, _
ByVal size As Long)
Call sqlrcur_inputBindClob(sqlrcur, variable, value, size)
End Sub

Public Sub subStrings(ByVal sqlrcur As Long, ByVal variable As String, ByVal value As String)
Call sqlrcur_subStrings(sqlrcur, variable, value)
End Sub

Public Sub subLongs(ByVal sqlrcur As Long, ByVal variable As String, ByVal value As String)
Call sqlrcur_subLongs(sqlrcur, variable, value)
End Sub

Public Sub subDoubles(ByVal sqlrcur As Long, ByVal variables As String, ByVal values As Long, _
ByVal precisions As Long, ByVal scales As Long)
Call sqlrcur_subDoubles(sqlrcur, variables, values, precisions, scales)
End Sub

Public Sub inputBindStrings(ByVal sqlrcur As Long, ByRef variables() As String, ByRef values() As String)
Call sqlrcur_inputBindStrings(sqlrcur, variables, values)
End Sub

Public Sub inputBindLongs(ByVal sqlrcur As Long, ByVal variables As Long, ByVal values As Long)
Call sqlrcur_inputBindLongs(sqlrcur, variables, values)
End Sub

Public Sub inputBindDoubles(ByVal sqlrcur As Long, ByVal variables As Long, _
ByVal values As Long, _
ByVal precisions As Long, _
ByVal scales As Long)
Call sqlrcur_inputBindDoubles(sqlrcur, variables, values, precisions, scales)
End Sub

Public Sub validdateBinds(ByVal sqlrcur As Long)
Call sqlrcur_validateBinds(sqlrcur)
End Sub

Public Function executeQuery(ByVal sqlrcur As Long) As Long
executeQuery = sqlrcur_executeQuery(sqlrcur)
End Function

Public Function fetchFromBindCursor(ByVal sqlrcur As Long) As Long
fetchFromBindCursor = sqlrcur_fetchFromBindCursor(sqlrcur)
End Function

Public Sub defineOutputBind(ByVal sqrcur As Long, ByVal variable As Long, ByVal length As Long)
Call sqlrcur_defineOutputBind(sqlrcur, variable, length)
End Sub

Public Sub defineOutputBindBlob(ByVal sqlrcur As Long, ByVal variable As Long)
Call sqlrcur_defineOutputBindBlob(sqlrcur, variable)
End Sub

Public Sub defineOutputBindClob(ByVal sqlrcur As Long, ByVal variable As Long)
Call sqlrcur_defineOutputBindClob(sqlrcur, variable)
End Sub

Public Sub defineOutputBindCursor(ByVal sqlrcur As Long, ByVal variable As Long)
Call sqlrcur_defineOutputBindCursor(sqlrcon, variable)
End Sub

Public Function getOutputBind(ByVal sqlrcur As Long, ByVal variable As String) As String
getOutputBind = sqlrcur_getOutputBind(sqlrcur, variable)
End Function

Public Function getOutputBindLength(ByVal sqlrcur As Long, ByVal variable As String) As Long
getOutputBindLength = sqlrcur_getOutputBindLength(sqlrcur, variable)
End Function

Public Function getOutputBindCursor(ByVal sqlrcur As Long, ByVal variable As String) As Long
getOutputBindCursor = sqlrcur_getOutputBindCursor(sqlrcur, variable)
End Function

Public Function openCachedResultSet(ByVal sqlrcur As Long, ByVal filename As String) As Long
openCachedResultSet = sqlrcur_openCachedResultSet(sqlrcur, filename)
End Function

Public Function rowCount(ByVal sqlrcur As Long) As Long
rowCount = sqlrcur_rowCount(sqlrcur)
End Function

Public Function colCount(ByVal sqlrcur As Long) As Long
colCount = sqlrcur_colCount(sqlrcur)
End Function

Public Function totalRows(ByVal sqlrcur As Long) As Long
totalRows = sqlrcur_totalRows(sqlrcur)
End Function

Public Function affectedRows(ByVal sqlrcur As Long) As Long
affectedRows = sqlrcur_affectedRows(sqlrcur)

End Function

Public Function firstRowIndex(ByVal sqlrcur As Long) As Long
firstRowIndex = sqlrcur_firstRowIndex(sqlrcur)
End Function

Public Function endOfResultSet(ByVal sqlrcur As Long) As Long
endOfResultSet = sqlrcur_endOfResultSet(sqlrcur)
End Function

Public Function errorMessage(ByVal sqlrcur As Long) As String
errorMessage = sqlrcur_errorMessage(sqlrcur)
End Function

Public Sub getNullAsEmptyStrings(ByVal sqlrcur As Long)
Call sqlrcur_getNullsAsEmptyStrings(sqlrcur)
End Sub

Public Sub getNullAsNulls(ByVal sqlrcur As Long)
Call sqlrcur_getNullsAsNulls(sqlrcur)
End Sub

Public Function getFieldByIndex(ByVal sqlrcur As Long, ByVal row As Long, ByVal col As Long) As String
 getFieldByIndex = sqlrcur_getFieldByIndex(sqlrcur, row, col)
End Function

Public Function getFieldByName(ByVal sqlrcur As Long, ByVal row As Long, ByVal col As String) As String
col = col & Chr(0)
getFieldByName = sqlrcur_getFieldByName(sqlrcur, row, col)
End Function

Public Function getFieldLengthByIndex(ByVal sqlrcur As Long, ByVal row As Long, ByVal col As Long) As Long
getFieldLengthByIndex = sqlrcur_getFieldLengthByIndex(sqlrcur, row, col)
End Function

Public Function getFieldLengthByName(ByVal sqlrcur As Long, ByVal row As Long, ByVal col As String) As Long
getFieldLengthByName = sqlrcur_getFieldLengthByName(sqlrcur, row, col)
End Function

Public Function getRow(ByVal sqlrcur As Long, ByVal row As Long) _
As Long
getRow = sqlrcur_getRow(sqlrcur, row)
End Function

Public Function getRowLengths(ByVal sqlrcur As Long, ByVal row As Long) As Long
getRowLengths = sqlrcur_getRowLengths(sqlrcur, row)
End Function

Public Function getColumnNames(ByVal sqlrcur As Long) As Long
getColumNames = sqlrcur_getColumnNames(sqlrcur)
End Function

Public Function getColumnName(ByVal sqlrcur As Long, ByVal col As Long) As Long
getColumnName = sqlrcur_getColumnName(sqlrcur, col)
End Function

Public Function getColumnTypeByIndex(ByVal sqlrcur As Long, ByVal col As Long) As Long
getColumnByIndex = sqlrcur_getColumnTypeByIndex(sqlrcur, col)
End Function

Public Function getColumnLengthByIndex(ByVal sqlrcur As Long, ByVal col As Long) As Long
getColumnLengthByIndex = sqlrcur_getColumnLengthByIndex(sqlrcur, col)
End Function

Public Function getColumnTypeByName(ByVal sqrcur As Long, ByVal col As String) As String
getColumnTypeByName = sqlrcur_getColumnTypeByName(sqlrcur, col)
End Function

Public Function getColumnLengthByName(ByVal sqlrcur As Long, ByVal col As String) As String
getColumnLengthByName = sqlrcur_getColumnLengthByName(sqlrcur, col)
End Function

Public Function getLongestByName(ByVal sqlrcur As Long, ByVal col As String) As Long
getLongestByName = sqlrcur_getLongestByName(sqlrcur, col)
End Function

Public Function getLongestByIndex(ByVal sqlrcur As Long, ByVal col As Long) As Long
getLongestByIndex = sqlrcur_getLongestByIndex(sqlrcur, col)
End Function

Public Function getResultSetId(ByVal sqlrcur As Long) As Long
getResultSetId = sqlrcur_getResultSetId(sqlrcur)
End Function

Public Sub suspendResultSet(ByVal sqlrcur As Long)
Call sqlrcur_suspendResultSet(sqlrcur)
End Sub

Public Function resumeResultSet(ByVal sqlrcur As Long, ByVal col As Long) As Long
resumeResultSet = sqlrcur_resumeResultSet(sqlrcur, col)
End Function

Public Function resumeCachedResultSet(ByVal sqlrcur As Long, ByVal id As Long, ByVal filename As String) As Long
resumeCachedResultSet = sqlrcur_resumeCachedResultSet(sqlrcur, id, filename)
End Function


