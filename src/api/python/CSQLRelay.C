/* Copyright (c) 2000 Roman Milner
   See the file COPYING for more information */

#ifdef __CYGWIN__
	#include <windows.h>
#endif

#include <Python.h>
#include <sqlrelay/sqlrclient.h>

extern "C" {

#define NEED_IS_NUMBER_TYPE_CHAR
#include <datatypes.h>

static PyObject *sqlrcon_alloc(PyObject *self, PyObject *args) {
  sqlrconnection *sqlrcon;
  char *host;
  char *user;
  char *password;
  char *socket;
  int port;
  int retrytime;
  int tries;
  if (!PyArg_ParseTuple(args, "sisssii", &host, &port, &socket, &user, &password, &retrytime, &tries))
    return NULL;
  sqlrcon = new sqlrconnection(host, port, socket, user, password, 1, 1);
  sqlrcon->copyReferences();
  return Py_BuildValue("l", sqlrcon);
}

static PyObject *sqlrcon_free(PyObject *self, PyObject *args) {
  long sqlrcon;
  if (!PyArg_ParseTuple(args, "l", &sqlrcon))
    return NULL;
  delete ((sqlrconnection *)sqlrcon);
  return Py_BuildValue("i", 0);
}

static PyObject *endSession(PyObject *self, PyObject *args) {
  long sqlrcon;
  if (!PyArg_ParseTuple(args, "l", &sqlrcon))
    return NULL;
  ((sqlrconnection *)sqlrcon)->endSession();
  return Py_BuildValue("i", 0);
}

static PyObject *suspendSession(PyObject *self, PyObject *args) {
  long sqlrcon;
  int rc;
  if (!PyArg_ParseTuple(args, "l", &sqlrcon))
    return NULL;
  rc=((sqlrconnection *)sqlrcon)->suspendSession();
  return Py_BuildValue("i", rc);
}

static PyObject *getConnectionPort(PyObject *self, PyObject *args) {
  long sqlrcon;
  int rc;
  if (!PyArg_ParseTuple(args, "l", &sqlrcon))
    return NULL;
  rc=((sqlrconnection *)sqlrcon)->getConnectionPort();
  return Py_BuildValue("i", rc);
}

static PyObject *getConnectionSocket(PyObject *self, PyObject *args) {
  long sqlrcon;
  char *rc;
  if (!PyArg_ParseTuple(args, "l", &sqlrcon))
    return NULL;
  rc=((sqlrconnection *)sqlrcon)->getConnectionSocket();
  return Py_BuildValue("s", rc);
}

static PyObject *resumeSession(PyObject *self, PyObject *args) {
  long sqlrcon;
  int port;
  char *socket;
  int rc;
  if (!PyArg_ParseTuple(args, "lis", &sqlrcon, &port, &socket))
    return NULL;
  rc=((sqlrconnection *)sqlrcon)->resumeSession(port,socket);
  return Py_BuildValue("i", rc);
}

static PyObject *ping(PyObject *self, PyObject *args) {
  long sqlrcon;
  int rc;
  if (!PyArg_ParseTuple(args, "l", &sqlrcon))
    return NULL;
  Py_BEGIN_ALLOW_THREADS
  rc=((sqlrconnection *)sqlrcon)->ping();
  Py_END_ALLOW_THREADS
  return Py_BuildValue("i", rc);
}

static PyObject *identify(PyObject *self, PyObject *args) {
  long sqlrcon;
  char *rc;
  if (!PyArg_ParseTuple(args, "l", &sqlrcon))
    return NULL;
  Py_BEGIN_ALLOW_THREADS
  rc=((sqlrconnection *)sqlrcon)->identify();
  Py_END_ALLOW_THREADS
  return Py_BuildValue("s", strdup(rc));
}

static PyObject *autoCommitOn(PyObject *self, PyObject *args) {
  long sqlrcon;
  int rc;
  if (!PyArg_ParseTuple(args, "l", &sqlrcon))
    return NULL;
  Py_BEGIN_ALLOW_THREADS
  rc=((sqlrconnection *)sqlrcon)->autoCommitOn();
  Py_END_ALLOW_THREADS
  return Py_BuildValue("i", rc);
}

static PyObject *autoCommitOff(PyObject *self, PyObject *args) {
  long sqlrcon;
  int rc;
  if (!PyArg_ParseTuple(args, "l", &sqlrcon))
    return NULL;
  Py_BEGIN_ALLOW_THREADS
  rc=((sqlrconnection *)sqlrcon)->autoCommitOff();
  Py_END_ALLOW_THREADS
  return Py_BuildValue("i", rc);
}

static PyObject *commit(PyObject *self, PyObject *args) {
  long sqlrcon;
  int rc;
  if (!PyArg_ParseTuple(args, "l", &sqlrcon))
    return NULL;
  Py_BEGIN_ALLOW_THREADS
  rc=((sqlrconnection *)sqlrcon)->commit();
  Py_END_ALLOW_THREADS
  return Py_BuildValue("i", rc);
}

static PyObject *rollback(PyObject *self, PyObject *args) {
  long sqlrcon;
  int rc;
  if (!PyArg_ParseTuple(args, "l", &sqlrcon))
    return NULL;
  Py_BEGIN_ALLOW_THREADS
  rc=((sqlrconnection *)sqlrcon)->rollback();
  Py_END_ALLOW_THREADS
  return Py_BuildValue("i", rc);
}

static PyObject *debugOn(PyObject *self, PyObject *args) {
  long sqlrcon;
  if (!PyArg_ParseTuple(args, "l", &sqlrcon))
    return NULL;
  ((sqlrconnection *)sqlrcon)->debugOn();
  return Py_BuildValue("i", 0);
}

static PyObject *debugOff(PyObject *self, PyObject *args) {
  long sqlrcon;
  if (!PyArg_ParseTuple(args, "l", &sqlrcon))
    return NULL;
  ((sqlrconnection *)sqlrcon)->debugOff();
  return Py_BuildValue("i", 0);
}

static PyObject *getDebug(PyObject *self, PyObject *args) {
  long sqlrcon;
  if (!PyArg_ParseTuple(args, "l", &sqlrcon))
    return NULL;
  return Py_BuildValue("i", ((sqlrconnection *)sqlrcon)->getDebug());
}

static PyObject *sqlrcur_alloc(PyObject *self, PyObject *args) {
  long	sqlrcon;
  sqlrcursor *sqlrcur;
  if (!PyArg_ParseTuple(args, "l", &sqlrcon))
    return NULL;
  sqlrcur = new sqlrcursor((sqlrconnection *)sqlrcon);
  sqlrcur->copyReferences();
  return Py_BuildValue("l", sqlrcur);
}

static PyObject *sqlrcur_free(PyObject *self, PyObject *args) {
  long sqlrcur;
  if (!PyArg_ParseTuple(args, "l", &sqlrcur))
    return NULL;
  delete ((sqlrcursor *)sqlrcur);
  return Py_BuildValue("i", 0);
}

static PyObject *setResultSetBufferSize(PyObject *self, PyObject *args) {
  int rows;
  long sqlrcur;
  if (!PyArg_ParseTuple(args, "li", &sqlrcur, &rows))
    return NULL;
  ((sqlrcursor *)sqlrcur)->setResultSetBufferSize(rows);
  return Py_BuildValue("i", 0);
}

static PyObject *getResultSetBufferSize(PyObject *self, PyObject *args) {
  long sqlrcur;
  int rc;
  if (!PyArg_ParseTuple(args, "l", &sqlrcur))
    return NULL;
  rc=((sqlrcursor *)sqlrcur)->getResultSetBufferSize();
  return Py_BuildValue("i", rc);
}

static PyObject *dontGetColumnInfo(PyObject *self, PyObject *args) {
  long sqlrcur;
  if (!PyArg_ParseTuple(args, "l", &sqlrcur))
    return NULL;
  ((sqlrcursor *)sqlrcur)->dontGetColumnInfo();
  return Py_BuildValue("i", 0);
}

static PyObject *getColumnInfo(PyObject *self, PyObject *args) {
  long sqlrcur;
  if (!PyArg_ParseTuple(args, "l", &sqlrcur))
    return NULL;
  ((sqlrcursor *)sqlrcur)->getColumnInfo();
  return Py_BuildValue("i", 0);
}

static PyObject *mixedCaseColumnNames(PyObject *self, PyObject *args) {
  long sqlrcur;
  if (!PyArg_ParseTuple(args, "l", &sqlrcur))
    return NULL;
  ((sqlrcursor *)sqlrcur)->mixedCaseColumnNames();
  return Py_BuildValue("i", 0);
}

static PyObject *upperCaseColumnNames(PyObject *self, PyObject *args) {
  long sqlrcur;
  if (!PyArg_ParseTuple(args, "l", &sqlrcur))
    return NULL;
  ((sqlrcursor *)sqlrcur)->upperCaseColumnNames();
  return Py_BuildValue("i", 0);
}

static PyObject *lowerCaseColumnNames(PyObject *self, PyObject *args) {
  long sqlrcur;
  if (!PyArg_ParseTuple(args, "l", &sqlrcur))
    return NULL;
  ((sqlrcursor *)sqlrcur)->lowerCaseColumnNames();
  return Py_BuildValue("i", 0);
}

static PyObject *cacheToFile(PyObject *self, PyObject *args) {
  long sqlrcur;
  char *filename;
  if (!PyArg_ParseTuple(args, "ls", &sqlrcur, &filename))
    return NULL;
  ((sqlrcursor *)sqlrcur)->cacheToFile(filename);
  return Py_BuildValue("i", 0);
}

static PyObject *setCacheTtl(PyObject *self, PyObject *args) {
  int ttl;
  long sqlrcur;
  if (!PyArg_ParseTuple(args, "li", &sqlrcur, &ttl))
    return NULL;
  ((sqlrcursor *)sqlrcur)->setCacheTtl(ttl);
  return Py_BuildValue("i", 0);
}

static PyObject *getCacheFileName(PyObject *self, PyObject *args) {
  long sqlrcur;
  char  *rc;
  if (!PyArg_ParseTuple(args, "l", &sqlrcur))
    return NULL;
  rc=((sqlrcursor *)sqlrcur)->getCacheFileName();
  return Py_BuildValue("s", rc);
}

static PyObject *cacheOff(PyObject *self, PyObject *args) {
  long sqlrcur;
  if (!PyArg_ParseTuple(args, "l", &sqlrcur))
    return NULL;
  ((sqlrcursor *)sqlrcur)->cacheOff();
  return Py_BuildValue("i", 0);
}

static PyObject *sendQuery(PyObject *self, PyObject *args) {
  char *sqlString;
  long sqlrcur;
  int rc;
  if (!PyArg_ParseTuple(args, "ls", &sqlrcur, &sqlString))
    return NULL;
  Py_BEGIN_ALLOW_THREADS
  rc=((sqlrcursor *)sqlrcur)->sendQuery(sqlString);
  Py_END_ALLOW_THREADS
  return Py_BuildValue("i", rc);
}

static PyObject *sendQueryWithLength(PyObject *self, PyObject *args) {
  char *sqlString;
  long sqlrcur;
  int length;
  int rc;
  if (!PyArg_ParseTuple(args, "lsi", &sqlrcur, &sqlString, &length))
    return NULL;
  Py_BEGIN_ALLOW_THREADS
  rc=((sqlrcursor *)sqlrcur)->sendQuery(sqlString,length);
  Py_END_ALLOW_THREADS
  return Py_BuildValue("i", rc);
}

static PyObject *sendFileQuery(PyObject *self, PyObject *args) {
  char *path;
  char *file;
  long sqlrcur;
  int rc;
  if (!PyArg_ParseTuple(args, "lss", &sqlrcur, &path, &file))
    return NULL;
  Py_BEGIN_ALLOW_THREADS
  rc=((sqlrcursor *)sqlrcur)->sendFileQuery(path, file);
  Py_END_ALLOW_THREADS
  return Py_BuildValue("i", rc);
}

static PyObject *prepareQuery(PyObject *self, PyObject *args) {
  char *sqlString;
  long sqlrcur;
  if (!PyArg_ParseTuple(args, "ls", &sqlrcur, &sqlString))
    return NULL;
  ((sqlrcursor *)sqlrcur)->prepareQuery(sqlString);
  return Py_BuildValue("i", 0);
}

static PyObject *prepareQueryWithLength(PyObject *self, PyObject *args) {
  char *sqlString;
  long sqlrcur;
  int length;
  if (!PyArg_ParseTuple(args, "lsi", &sqlrcur, &sqlString, &length))
    return NULL;
  ((sqlrcursor *)sqlrcur)->prepareQuery(sqlString,length);
  return Py_BuildValue("i", 0);
}

static PyObject *prepareFileQuery(PyObject *self, PyObject *args) {
  char *path;
  char *file;
  long sqlrcur;
  int rc;
  if (!PyArg_ParseTuple(args, "lss", &sqlrcur, &path, &file))
    return NULL;
  rc=((sqlrcursor *)sqlrcur)->prepareFileQuery(path, file);
  return Py_BuildValue("i", rc);
}

static PyObject *substitution(PyObject *self, PyObject *args) {
  char *variable;
  PyObject *value;
  int precision;
  int scale;
  long sqlrcur;
  if (!PyArg_ParseTuple(args, "lsOii", &sqlrcur, &variable, &value, &precision, &scale))
    return NULL;
  if (value==Py_None) {
    ((sqlrcursor *)sqlrcur)->substitution(variable, (char *)NULL);
  } else if (PyString_Check(value)) {
    ((sqlrcursor *)sqlrcur)->substitution(variable, PyString_AsString(value));
  } else if (PyInt_Check(value)) {
    ((sqlrcursor *)sqlrcur)->substitution(variable, (long)PyInt_AsLong(value));
  } else if (PyFloat_Check(value)) {
    ((sqlrcursor *)sqlrcur)->substitution(variable, (double)PyFloat_AsDouble(value), (unsigned short)precision, (unsigned short)scale);
  }
  return Py_BuildValue("i", 0);
}

static PyObject *substitutions(PyObject *self, PyObject *args) {
  PyObject *variables;
  PyObject *values;
  PyObject *precisions;
  PyObject *scales;
  long sqlrcur;
  char *variable;
  PyObject *value;
  if (!PyArg_ParseTuple(args, "lOOOO", &sqlrcur, &variables, &values, &precisions, &scales))
    return NULL;
  if (PyList_Check(variables) && PyList_Check(values)) {
    for (int i=0; i<PyList_Size(variables); i++) {
      variable=PyString_AsString(PyList_GetItem(variables,i));
      value=PyList_GetItem(values,i);
      if (value==Py_None) {
        ((sqlrcursor *)sqlrcur)->substitution(variable, (char *)NULL);
      } else if (PyString_Check(value)) {
        ((sqlrcursor *)sqlrcur)->substitution(variable, PyString_AsString(value));
      } else if (PyInt_Check(value)) {
        ((sqlrcursor *)sqlrcur)->substitution(variable, (long)PyInt_AsLong(value));
      } else if (PyFloat_Check(value)) {
        ((sqlrcursor *)sqlrcur)->substitution(variable, (double)PyFloat_AsDouble(value), (unsigned short)PyInt_AsLong(PyList_GetItem(precisions,i)), (unsigned short)PyInt_AsLong(PyList_GetItem(scales,i)));
      }
    }
  }
  return Py_BuildValue("i", 0);
}

static PyObject *clearBinds(PyObject *self, PyObject *args) {
  long sqlrcur;
  if (!PyArg_ParseTuple(args, "l", &sqlrcur))
    return NULL;
  ((sqlrcursor *)sqlrcur)->clearBinds();
  return Py_BuildValue("i", 0);
}

static PyObject *countBindVariables(PyObject *self, PyObject *args) {
  long sqlrcur;
  unsigned short rc;
  if (!PyArg_ParseTuple(args, "l", &sqlrcur))
    return NULL;
  rc=((sqlrcursor *)sqlrcur)->countBindVariables();
  return Py_BuildValue("i", rc);
}

static PyObject *inputBind(PyObject *self, PyObject *args) {
  char *variable;
  PyObject *value;
  int precision;
  int scale;
  long sqlrcur;
  if (!PyArg_ParseTuple(args, "lsOii", &sqlrcur, &variable, &value, &precision, &scale))
    return NULL;
  if (value==Py_None) {
    ((sqlrcursor *)sqlrcur)->inputBind(variable, (char *)NULL);
  } else if (PyString_Check(value)) {
    ((sqlrcursor *)sqlrcur)->inputBind(variable, PyString_AsString(value));
  } else if (PyInt_Check(value)) {
    ((sqlrcursor *)sqlrcur)->inputBind(variable, (long)PyInt_AsLong(value));
  } else if (PyFloat_Check(value)) {
    ((sqlrcursor *)sqlrcur)->inputBind(variable, (double)PyFloat_AsDouble(value), (unsigned short)precision, (unsigned short)scale);
  }
  return Py_BuildValue("i", 0);
}

static PyObject *inputBindBlob(PyObject *self, PyObject *args) {
  char *variable;
  PyObject *value;
  long size;
  long sqlrcur;
  if (!PyArg_ParseTuple(args, "lsOl", &sqlrcur, &variable, &value, &size))
    return NULL;
  if (value==Py_None) {
    ((sqlrcursor *)sqlrcur)->inputBindBlob(variable, NULL, size);
  } else if (PyString_Check(value)) {
    ((sqlrcursor *)sqlrcur)->inputBindBlob(variable, PyString_AsString(value), size);
  }
  return Py_BuildValue("i", 0);
}

static PyObject *inputBindClob(PyObject *self, PyObject *args) {
  char *variable;
  PyObject *value;
  long size;
  long sqlrcur;
  if (!PyArg_ParseTuple(args, "lsOl", &sqlrcur, &variable, &value, &size))
    return NULL;
  if (value==Py_None) {
    ((sqlrcursor *)sqlrcur)->inputBindClob(variable, NULL, size);
  } else if (PyString_Check(value)) {
    ((sqlrcursor *)sqlrcur)->inputBindClob(variable, PyString_AsString(value), size);
  }
  return Py_BuildValue("i", 0);
}

static PyObject *inputBinds(PyObject *self, PyObject *args) {
  PyObject *variables;
  PyObject *values;
  PyObject *precisions;
  PyObject *scales;
  long sqlrcur;
  char *variable;
  PyObject *value;
  if (!PyArg_ParseTuple(args, "lOOOO", &sqlrcur, &variables, &values, &precisions, &scales))
    return NULL;
  if (PyList_Check(variables) && PyList_Check(values)) {
    for (int i=0; i<PyList_Size(variables); i++) {
      variable=PyString_AsString(PyList_GetItem(variables,i));
      value=PyList_GetItem(values,i);
      if (value==Py_None) {
        ((sqlrcursor *)sqlrcur)->inputBind(variable, (char *)NULL);
      } else if (PyString_Check(value)) {
        ((sqlrcursor *)sqlrcur)->inputBind(variable, PyString_AsString(value));
      } else if (PyInt_Check(value)) {
        ((sqlrcursor *)sqlrcur)->inputBind(variable, (unsigned long)PyInt_AsLong(value));
      } else if (PyFloat_Check(value)) {
        ((sqlrcursor *)sqlrcur)->inputBind(variable, (double)PyFloat_AsDouble(value), (unsigned short)PyInt_AsLong(PyList_GetItem(precisions,i)), (unsigned short)PyInt_AsLong(PyList_GetItem(scales,i)));
      }
    }
  }
  return Py_BuildValue("i", 0);
}

static PyObject *defineOutputBind(PyObject *self, PyObject *args) {
  char *variable;
  int length;
  long sqlrcur;
  if (!PyArg_ParseTuple(args, "lsi", &sqlrcur, &variable, &length))
    return NULL;
  ((sqlrcursor *)sqlrcur)->defineOutputBind(variable, length);
  return Py_BuildValue("i", 0);
}

static PyObject *defineOutputBindBlob(PyObject *self, PyObject *args) {
  char *variable;
  long sqlrcur;
  if (!PyArg_ParseTuple(args, "ls", &sqlrcur, &variable))
    return NULL;
  ((sqlrcursor *)sqlrcur)->defineOutputBindBlob(variable);
  return Py_BuildValue("i", 0);
}

static PyObject *defineOutputBindClob(PyObject *self, PyObject *args) {
  char *variable;
  long sqlrcur;
  if (!PyArg_ParseTuple(args, "ls", &sqlrcur, &variable))
    return NULL;
  ((sqlrcursor *)sqlrcur)->defineOutputBindClob(variable);
  return Py_BuildValue("i", 0);
}

static PyObject *defineOutputBindCursor(PyObject *self, PyObject *args) {
  char *variable;
  long sqlrcur;
  if (!PyArg_ParseTuple(args, "ls", &sqlrcur, &variable))
    return NULL;
  ((sqlrcursor *)sqlrcur)->defineOutputBindCursor(variable);
  return Py_BuildValue("i", 0);
}

static PyObject *validateBinds(PyObject *self, PyObject *args) {
  long sqlrcur;
  if (!PyArg_ParseTuple(args, "l", &sqlrcur))
    return NULL;
  ((sqlrcursor *)sqlrcur)->validateBinds();
  return Py_BuildValue("i", 0);
}

static PyObject *executeQuery(PyObject *self, PyObject *args) {
  long sqlrcur;
  int rc;
  if (!PyArg_ParseTuple(args, "l", &sqlrcur))
    return NULL;
  rc=((sqlrcursor *)sqlrcur)->executeQuery();
  return Py_BuildValue("i", rc);
}

static PyObject *fetchFromBindCursor(PyObject *self, PyObject *args) {
  long sqlrcur;
  int rc;
  if (!PyArg_ParseTuple(args, "l", &sqlrcur))
    return NULL;
  rc=((sqlrcursor *)sqlrcur)->fetchFromBindCursor();
  return Py_BuildValue("i", rc);
}

static PyObject *getOutputBind(PyObject *self, PyObject *args) {
  char *variable;
  long sqlrcur;
  char *rc;
  int rl;
  if (!PyArg_ParseTuple(args, "ls", &sqlrcur, &variable))
    return NULL;
  rc=((sqlrcursor *)sqlrcur)->getOutputBind(variable);
  rl=((sqlrcursor *)sqlrcur)->getOutputBindLength(variable);
  return Py_BuildValue("s#", rc, rl);
}

static PyObject *getOutputBindAsLong(PyObject *self, PyObject *args) {
  char *variable;
  long sqlrcur;
  long rc;
  if (!PyArg_ParseTuple(args, "ls", &sqlrcur, &variable))
    return NULL;
  rc=((sqlrcursor *)sqlrcur)->getOutputBindAsLong(variable);
  return Py_BuildValue("l", rc);
}

static PyObject *getOutputBindAsDouble(PyObject *self, PyObject *args) {
  char *variable;
  long sqlrcur;
  double rc;
  if (!PyArg_ParseTuple(args, "ls", &sqlrcur, &variable))
    return NULL;
  rc=((sqlrcursor *)sqlrcur)->getOutputBindAsDouble(variable);
  return Py_BuildValue("d", rc);
}

static PyObject *getOutputBindLength(PyObject *self, PyObject *args) {
  char *variable;
  long sqlrcur;
  long rc;
  if (!PyArg_ParseTuple(args, "ls", &sqlrcur, &variable))
    return NULL;
  rc=((sqlrcursor *)sqlrcur)->getOutputBindLength(variable);
  return Py_BuildValue("l", rc);
}

static PyObject *openCachedResultSet(PyObject *self, PyObject *args) {
  long sqlrcur;
  int rc;
  char *filename;
  if (!PyArg_ParseTuple(args, "ls", &sqlrcur, &filename))
    return NULL;
  Py_BEGIN_ALLOW_THREADS
  rc=((sqlrcursor *)sqlrcur)->openCachedResultSet(filename);
  Py_END_ALLOW_THREADS
  return Py_BuildValue("i", rc);
}

static PyObject *colCount(PyObject *self, PyObject *args) {
  long sqlrcur;
  int rc;
  if (!PyArg_ParseTuple(args, "l", &sqlrcur))
    return NULL;
  rc=((sqlrcursor *)sqlrcur)->colCount();
  return Py_BuildValue("i", rc);
}

static PyObject *rowCount(PyObject *self, PyObject *args) {
  long sqlrcur;
  int rc;
  if (!PyArg_ParseTuple(args, "l", &sqlrcur))
    return NULL;
  rc=((sqlrcursor *)sqlrcur)->rowCount();
  return Py_BuildValue("i", rc);
}

static PyObject *totalRows(PyObject *self, PyObject *args) {
  long sqlrcur;
  int rc;
  if (!PyArg_ParseTuple(args, "l", &sqlrcur))
    return NULL;
  rc=((sqlrcursor *)sqlrcur)->totalRows();
  return Py_BuildValue("i", rc);
}

static PyObject *affectedRows(PyObject *self, PyObject *args) {
  long sqlrcur;
  int rc;
  if (!PyArg_ParseTuple(args, "l", &sqlrcur))
    return NULL;
  rc=((sqlrcursor *)sqlrcur)->affectedRows();
  return Py_BuildValue("i", rc);
}

static PyObject *firstRowIndex(PyObject *self, PyObject *args) {
  long sqlrcur;
  int rc;
  if (!PyArg_ParseTuple(args, "l", &sqlrcur))
    return NULL;
  rc=((sqlrcursor *)sqlrcur)->firstRowIndex();
  return Py_BuildValue("i", rc);
}

static PyObject *endOfResultSet(PyObject *self, PyObject *args) {
  long sqlrcur;
  int rc;
  if (!PyArg_ParseTuple(args, "l", &sqlrcur))
    return NULL;
  rc=((sqlrcursor *)sqlrcur)->endOfResultSet();
  return Py_BuildValue("i", rc);
}

static PyObject *errorMessage(PyObject *self, PyObject *args) {
  long sqlrcur;
  if (!PyArg_ParseTuple(args, "l", &sqlrcur))
    return NULL;
  return Py_BuildValue("s", ((sqlrcursor *)sqlrcur)->errorMessage());
}

static PyObject *getNullsAsEmptyStrings(PyObject *self, PyObject *args) {
  long sqlrcur;
  if (!PyArg_ParseTuple(args, "l", &sqlrcur))
    return NULL;
  ((sqlrcursor *)sqlrcur)->getNullsAsEmptyStrings();
  return Py_BuildValue("i", 0);
}

static PyObject *getNullsAsNone(PyObject *self, PyObject *args) {
  long sqlrcur;
  if (!PyArg_ParseTuple(args, "l", &sqlrcur))
    return NULL;
  ((sqlrcursor *)sqlrcur)->getNullsAsNulls();
  return Py_BuildValue("i", 0);
}

static PyObject *getField(PyObject *self, PyObject *args) {
  long sqlrcur;
  char  *rc;
  int  rl;
  int row;
  PyObject *col;
  if (!PyArg_ParseTuple(args, "liO", &sqlrcur, &row, &col))
    return NULL;
  if (PyString_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getField(row, PyString_AsString(col));
    rl=((sqlrcursor *)sqlrcur)->getFieldLength(row, PyString_AsString(col));
  } else if (PyInt_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getField(row, PyInt_AsLong(col));
    rl=((sqlrcursor *)sqlrcur)->getFieldLength(row, PyInt_AsLong(col));
  }
  if (!rc) {
    return Py_None;
  }
  return Py_BuildValue("s#", rc, rl);
}

static PyObject *getFieldAsLong(PyObject *self, PyObject *args) {
  long sqlrcur;
  long rc;
  int row;
  PyObject *col;
  if (!PyArg_ParseTuple(args, "liO", &sqlrcur, &row, &col))
    return NULL;
  if (PyString_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getFieldAsLong(row, PyString_AsString(col));
  } else if (PyInt_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getFieldAsLong(row, PyInt_AsLong(col));
  }
  return Py_BuildValue("l", rc);
}

static PyObject *getFieldAsDouble(PyObject *self, PyObject *args) {
  long sqlrcur;
  double rc;
  int row;
  PyObject *col;
  if (!PyArg_ParseTuple(args, "liO", &sqlrcur, &row, &col))
    return NULL;
  if (PyString_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getFieldAsDouble(row, PyString_AsString(col));
  } else if (PyInt_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getFieldAsDouble(row, PyInt_AsLong(col));
  }
  return Py_BuildValue("d", rc);
}

static PyObject *getFieldLength(PyObject *self, PyObject *args) {
  long sqlrcur;
  long rc;
  int row;
  PyObject *col;
  if (!PyArg_ParseTuple(args, "liO", &sqlrcur, &row, &col))
    return NULL;
  if (PyString_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getFieldLength(row, PyString_AsString(col));
  } else if (PyInt_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getFieldLength(row, PyInt_AsLong(col));
  }
  return Py_BuildValue("l", rc);
}

static PyObject *
_get_row(sqlrcursor *sqlrcur, int row)
{
  int num_cols;
  int counter;
  char **row_data;
  PyObject *my_list;
  num_cols=sqlrcur->colCount();
  my_list =  PyList_New(num_cols);
  row_data=sqlrcur->getRow(row);
  if (!row_data) {
    return Py_None;
  }
  for (counter = 0; counter < num_cols; ++counter) {
    if (!row_data[counter]) {
        PyList_SetItem(my_list, counter, Py_None);
    } else if (isNumberTypeChar(sqlrcur->getColumnType(counter))) {
      if (!strchr(row_data[counter], '.')) {
          PyList_SetItem(my_list, counter, Py_BuildValue("i", atoi(row_data[counter])));
      } else {
          PyList_SetItem(my_list, counter, Py_BuildValue("f", atof(row_data[counter])));
      }
    } else {
      PyList_SetItem(my_list, counter, Py_BuildValue("s", row_data[counter]));
    }
  }
  return my_list;
}

static PyObject *getRow(PyObject *self, PyObject *args) {
  long sqlrcur;
  int row;
  if (!PyArg_ParseTuple(args, "li", &sqlrcur, &row))
    return NULL;
  return _get_row((sqlrcursor *)sqlrcur, row);
}

static PyObject *getRowDictionary(PyObject *self, PyObject *args) {
  long sqlrcur;
  int row;
  PyObject *my_dictionary;
  int counter;
  char *field;
  char *name;
  if (!PyArg_ParseTuple(args, "li", &sqlrcur, &row))
    return NULL;
  my_dictionary=PyDict_New();
  for (counter=0; counter<((sqlrcursor *)sqlrcur)->colCount(); counter++) {
    field=((sqlrcursor *)sqlrcur)->getField(row,counter);
    name=((sqlrcursor *)sqlrcur)->getColumnName(counter);
    if (isNumberTypeChar(((sqlrcursor *)sqlrcur)->getColumnType(counter))) {
      if (!strchr(field,'.')) {
        PyDict_SetItem(my_dictionary,Py_BuildValue("s",name),Py_BuildValue("i",atoi(field)));
      } else {
        PyDict_SetItem(my_dictionary,Py_BuildValue("s",name),Py_BuildValue("f",atof(field)));
      }
    } else {
      if (field) {
        PyDict_SetItem(my_dictionary,Py_BuildValue("s",name),Py_BuildValue("s",field));
      } else {
        PyDict_SetItem(my_dictionary,Py_BuildValue("s",name),Py_None);
      }
    }
  }
  return my_dictionary;
}

static PyObject *getRowRange(PyObject *self, PyObject *args) {
  int beg_row;
  int end_row;
  long sqlrcur;
  int counter;
  PyObject *my_list;
  my_list = PyList_New(0);
  if (!PyArg_ParseTuple(args, "lii", &sqlrcur, &beg_row, &end_row))
    return NULL;
  int max_rows=((sqlrcursor *)sqlrcur)->rowCount();
  if (end_row>=max_rows) {
	  end_row=max_rows-1;
  }
  for (counter = beg_row; counter <= end_row; ++counter) {
    PyList_Append(my_list, _get_row((sqlrcursor *)sqlrcur, counter));
  }
  return my_list;
}

static PyObject *
_get_row_lengths(sqlrcursor *sqlrcur, int row)
{
  int num_cols;
  int counter;
  long *row_data;
  PyObject *my_list;
  num_cols=sqlrcur->colCount();
  my_list =  PyList_New(num_cols);
  row_data=sqlrcur->getRowLengths(row);
  if (!row_data) {
    return Py_None;
  }
  for (counter = 0; counter < num_cols; ++counter) {
    if (!row_data[counter]) {
        PyList_SetItem(my_list, counter, Py_None);
    } else {
      PyList_SetItem(my_list, counter, Py_BuildValue("l", row_data[counter]));
    }
  }
  return my_list;
}

static PyObject *getRowLengths(PyObject *self, PyObject *args) {
  long sqlrcur;
  int row;
  if (!PyArg_ParseTuple(args, "li", &sqlrcur, &row))
    return NULL;
  return _get_row_lengths((sqlrcursor *)sqlrcur, row);
}

static PyObject *getRowLengthsDictionary(PyObject *self, PyObject *args) {
  long sqlrcur;
  int row;
  PyObject *my_dictionary;
  if (!PyArg_ParseTuple(args, "li", &sqlrcur, &row))
    return NULL;
  my_dictionary=PyDict_New();
  for (int counter=0; counter<((sqlrcursor *)sqlrcur)->colCount(); counter++) {
    PyDict_SetItem(my_dictionary,
        Py_BuildValue("s",((sqlrcursor *)sqlrcur)->getColumnName(counter)),
        Py_BuildValue("i",((sqlrcursor *)sqlrcur)->getFieldLength(row,counter)));
  }
  return my_dictionary;
}

static PyObject *getRowLengthsRange(PyObject *self, PyObject *args) {
  int beg_row;
  int end_row;
  long sqlrcur;
  int counter;
  PyObject *my_list;
  my_list = PyList_New(0);
  if (!PyArg_ParseTuple(args, "lii", &sqlrcur, &beg_row, &end_row))
    return NULL;
  int max_rows=((sqlrcursor *)sqlrcur)->rowCount();
  if (end_row>=max_rows) {
	  end_row=max_rows-1;
  }
  for (counter = beg_row; counter <= end_row; ++counter) {
    PyList_Append(my_list, _get_row_lengths((sqlrcursor *)sqlrcur, counter));
  }
  return my_list;
}

static PyObject *getColumnName(PyObject *self, PyObject *args) {
  long sqlrcur;
  char  *rc;
  int col;
  if (!PyArg_ParseTuple(args, "li", &sqlrcur, &col))
    return NULL;
  rc=((sqlrcursor *)sqlrcur)->getColumnName(col);
  return Py_BuildValue("s", rc);
}

static PyObject *getColumnNames(PyObject *self, PyObject *args) {
  long sqlrcur;
  int counter;
  int num_cols;
  char **rc;
  PyObject *my_list;
  my_list = PyList_New(0);
  if (!PyArg_ParseTuple(args, "l", &sqlrcur))
    return NULL;
  num_cols=((sqlrcursor *)sqlrcur)->colCount();
  rc=((sqlrcursor *)sqlrcur)->getColumnNames();
  if (rc) {
    for (counter = 0; counter < num_cols; ++counter) {
      PyList_Append(my_list, Py_BuildValue("s", rc[counter]));
    }
    return my_list;
  }
  return Py_None;
}

static PyObject *getColumnType(PyObject *self, PyObject *args) {
  long sqlrcur;
  char *rc;
  PyObject *col;
  if (!PyArg_ParseTuple(args, "lO", &sqlrcur, &col))
    return NULL;
  if (PyString_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getColumnType(PyString_AsString(col));
  } else if (PyInt_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getColumnType(PyInt_AsLong(col));
  }
  return Py_BuildValue("s", rc);
}

static PyObject *getColumnLength(PyObject *self, PyObject *args) {
  long sqlrcur;
  int rc;
  PyObject *col;
  if (!PyArg_ParseTuple(args, "lO", &sqlrcur, &col))
    return NULL;
  if (PyString_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getColumnLength(PyString_AsString(col));
  } else if (PyInt_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getColumnLength(PyInt_AsLong(col));
  }
  return Py_BuildValue("i", rc);
}

static PyObject *getColumnPrecision(PyObject *self, PyObject *args) {
  long sqlrcur;
  unsigned long rc;
  PyObject *col;
  if (!PyArg_ParseTuple(args, "lO", &sqlrcur, &col))
    return NULL;
  if (PyString_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getColumnPrecision(PyString_AsString(col));
  } else if (PyInt_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getColumnPrecision(PyInt_AsLong(col));
  }
  return Py_BuildValue("l", rc);
}

static PyObject *getColumnScale(PyObject *self, PyObject *args) {
  long sqlrcur;
  unsigned long rc;
  PyObject *col;
  if (!PyArg_ParseTuple(args, "lO", &sqlrcur, &col))
    return NULL;
  if (PyString_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getColumnScale(PyString_AsString(col));
  } else if (PyInt_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getColumnScale(PyInt_AsLong(col));
  }
  return Py_BuildValue("l", rc);
}

static PyObject *getColumnIsNullable(PyObject *self, PyObject *args) {
  long sqlrcur;
  unsigned short rc;
  PyObject *col;
  if (!PyArg_ParseTuple(args, "lO", &sqlrcur, &col))
    return NULL;
  if (PyString_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getColumnIsNullable(PyString_AsString(col));
  } else if (PyInt_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getColumnIsNullable(PyInt_AsLong(col));
  }
  return Py_BuildValue("h", rc);
}

static PyObject *getColumnIsPrimaryKey(PyObject *self, PyObject *args) {
  long sqlrcur;
  unsigned short rc;
  PyObject *col;
  if (!PyArg_ParseTuple(args, "lO", &sqlrcur, &col))
    return NULL;
  if (PyString_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getColumnIsPrimaryKey(PyString_AsString(col));
  } else if (PyInt_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getColumnIsPrimaryKey(PyInt_AsLong(col));
  }
  return Py_BuildValue("h", rc);
}

static PyObject *getColumnIsUnique(PyObject *self, PyObject *args) {
  long sqlrcur;
  unsigned short rc;
  PyObject *col;
  if (!PyArg_ParseTuple(args, "lO", &sqlrcur, &col))
    return NULL;
  if (PyString_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getColumnIsUnique(PyString_AsString(col));
  } else if (PyInt_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getColumnIsUnique(PyInt_AsLong(col));
  }
  return Py_BuildValue("h", rc);
}

static PyObject *getColumnIsPartOfKey(PyObject *self, PyObject *args) {
  long sqlrcur;
  unsigned short rc;
  PyObject *col;
  if (!PyArg_ParseTuple(args, "lO", &sqlrcur, &col))
    return NULL;
  if (PyString_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getColumnIsPartOfKey(PyString_AsString(col));
  } else if (PyInt_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getColumnIsPartOfKey(PyInt_AsLong(col));
  }
  return Py_BuildValue("h", rc);
}

static PyObject *getColumnIsUnsigned(PyObject *self, PyObject *args) {
  long sqlrcur;
  unsigned short rc;
  PyObject *col;
  if (!PyArg_ParseTuple(args, "lO", &sqlrcur, &col))
    return NULL;
  if (PyString_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getColumnIsUnsigned(PyString_AsString(col));
  } else if (PyInt_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getColumnIsUnsigned(PyInt_AsLong(col));
  }
  return Py_BuildValue("h", rc);
}

static PyObject *getColumnIsZeroFilled(PyObject *self, PyObject *args) {
  long sqlrcur;
  unsigned short rc;
  PyObject *col;
  if (!PyArg_ParseTuple(args, "lO", &sqlrcur, &col))
    return NULL;
  if (PyString_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getColumnIsZeroFilled(PyString_AsString(col));
  } else if (PyInt_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getColumnIsZeroFilled(PyInt_AsLong(col));
  }
  return Py_BuildValue("h", rc);
}

static PyObject *getColumnIsBinary(PyObject *self, PyObject *args) {
  long sqlrcur;
  unsigned short rc;
  PyObject *col;
  if (!PyArg_ParseTuple(args, "lO", &sqlrcur, &col))
    return NULL;
  if (PyString_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getColumnIsBinary(PyString_AsString(col));
  } else if (PyInt_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getColumnIsBinary(PyInt_AsLong(col));
  }
  return Py_BuildValue("h", rc);
}

static PyObject *getColumnIsAutoIncrement(PyObject *self, PyObject *args) {
  long sqlrcur;
  unsigned short rc;
  PyObject *col;
  if (!PyArg_ParseTuple(args, "lO", &sqlrcur, &col))
    return NULL;
  if (PyString_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getColumnIsAutoIncrement(PyString_AsString(col));
  } else if (PyInt_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getColumnIsAutoIncrement(PyInt_AsLong(col));
  }
  return Py_BuildValue("h", rc);
}

static PyObject *getLongest(PyObject *self, PyObject *args) {
  long sqlrcur;
  int rc;
  PyObject *col;
  if (!PyArg_ParseTuple(args, "lO", &sqlrcur, &col))
    return NULL;
  if (PyString_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getLongest(PyString_AsString(col));
  } else if (PyInt_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getLongest(PyInt_AsLong(col));
  }
  return Py_BuildValue("i", rc);
}

static PyObject *getResultSetId(PyObject *self, PyObject *args) {
  long sqlrcur;
  int rc;
  if (!PyArg_ParseTuple(args, "l", &sqlrcur))
    return NULL;
  rc=((sqlrcursor *)sqlrcur)->getResultSetId();
  return Py_BuildValue("i", rc);
}

static PyObject *suspendResultSet(PyObject *self, PyObject *args) {
  long sqlrcur;
  if (!PyArg_ParseTuple(args, "l", &sqlrcur))
    return NULL;
  ((sqlrcursor *)sqlrcur)->suspendResultSet();
  return Py_BuildValue("i", 0);
}

static PyObject *resumeResultSet(PyObject *self, PyObject *args) {
  long sqlrcur;
  int rc;
  int id;
  if (!PyArg_ParseTuple(args, "li", &sqlrcur, &id))
    return NULL;
  rc=((sqlrcursor *)sqlrcur)->resumeResultSet(id);
  return Py_BuildValue("i", rc);
}

static PyObject *resumeCachedResultSet(PyObject *self, PyObject *args) {
  long sqlrcur;
  int rc;
  int id;
  char *filename;
  if (!PyArg_ParseTuple(args, "lis", &sqlrcur, &id, &filename))
    return NULL;
  rc=((sqlrcursor *)sqlrcur)->resumeCachedResultSet(id,filename);
  return Py_BuildValue("i", rc);
}

static PyObject *getOutputBindCursorId(PyObject *self, PyObject *args) {
  long sqlrcur;
  short rc;
  char *variable;
  if (!PyArg_ParseTuple(args, "ls", &sqlrcur, &variable))
    return NULL;
  rc=((sqlrcursor *)sqlrcur)->getOutputBindCursorId(variable);
  return Py_BuildValue("i", rc);
}

static PyObject *attachToBindCursor(PyObject *self, PyObject *args) {
  long sqlrcur;
  int bindcursorid;
  if (!PyArg_ParseTuple(args, "li", &sqlrcur, &bindcursorid))
    return NULL;
  ((sqlrcursor *)sqlrcur)->attachToBindCursor(bindcursorid);
  return Py_BuildValue("i", 0);
}

static PyMethodDef SQLRMethods[] = {
  {"sqlrcon_alloc",  sqlrcon_alloc, METH_VARARGS},
  {"sqlrcon_free", sqlrcon_free, METH_VARARGS},
  {"endSession", endSession, METH_VARARGS},
  {"suspendSession", suspendSession, METH_VARARGS},
  {"getConnectionPort", getConnectionPort, METH_VARARGS},
  {"getConnectionSocket", getConnectionSocket, METH_VARARGS},
  {"resumeSession", resumeSession, METH_VARARGS},
  {"ping", ping, METH_VARARGS},
  {"identify", identify, METH_VARARGS},
  {"autoCommitOn", autoCommitOn, METH_VARARGS},
  {"autoCommitOff", autoCommitOff, METH_VARARGS},
  {"commit", commit, METH_VARARGS},
  {"rollback", rollback, METH_VARARGS},
  {"debugOn", debugOn, METH_VARARGS},
  {"debugOff", debugOff, METH_VARARGS},
  {"getDebug", getDebug, METH_VARARGS},
  {"sqlrcur_alloc",  sqlrcur_alloc, METH_VARARGS},
  {"sqlrcur_free",  sqlrcur_free, METH_VARARGS},
  {"setResultSetBufferSize", setResultSetBufferSize, METH_VARARGS},
  {"getResultSetBufferSize", getResultSetBufferSize, METH_VARARGS},
  {"dontGetColumnInfo", dontGetColumnInfo, METH_VARARGS},
  {"getColumnInfo", getColumnInfo, METH_VARARGS},
  {"mixedCaseColumnNames", mixedCaseColumnNames, METH_VARARGS},
  {"upperCaseColumnNames", upperCaseColumnNames, METH_VARARGS},
  {"lowerCaseColumnNames", lowerCaseColumnNames, METH_VARARGS},
  {"cacheToFile", cacheToFile, METH_VARARGS},
  {"setCacheTtl", setCacheTtl, METH_VARARGS},
  {"getCacheFileName", getCacheFileName, METH_VARARGS},
  {"cacheOff", cacheOff, METH_VARARGS},
  {"sendQuery", sendQuery, METH_VARARGS},
  {"sendQueryWithLength", sendQueryWithLength, METH_VARARGS},
  {"sendFileQuery", sendFileQuery, METH_VARARGS},
  {"prepareQuery", prepareQuery, METH_VARARGS},
  {"prepareQueryWithLength", prepareQueryWithLength, METH_VARARGS},
  {"prepareFileQuery", prepareFileQuery, METH_VARARGS},
  {"substitution", substitution, METH_VARARGS},
  {"substitutions", substitutions, METH_VARARGS},
  {"clearBinds", clearBinds, METH_VARARGS},
  {"countBindVariables", countBindVariables, METH_VARARGS},
  {"inputBind", inputBind, METH_VARARGS},
  {"inputBindBlob", inputBindBlob, METH_VARARGS},
  {"inputBindClob", inputBindClob, METH_VARARGS},
  {"inputBinds", inputBinds, METH_VARARGS},
  {"defineOutputBind", defineOutputBind, METH_VARARGS},
  {"defineOutputBindBlob", defineOutputBindBlob, METH_VARARGS},
  {"defineOutputBindClob", defineOutputBindClob, METH_VARARGS},
  {"defineOutputBindCursor", defineOutputBindCursor, METH_VARARGS},
  {"validateBinds", validateBinds, METH_VARARGS},
  {"executeQuery", executeQuery, METH_VARARGS},
  {"fetchFromBindCursor", fetchFromBindCursor, METH_VARARGS},
  {"getOutputBind", getOutputBind, METH_VARARGS},
  {"getOutputBindAsLong", getOutputBindAsLong, METH_VARARGS},
  {"getOutputBindAsDouble", getOutputBindAsDouble, METH_VARARGS},
  {"getOutputBindLength", getOutputBindLength, METH_VARARGS},
  {"openCachedResultSet", openCachedResultSet, METH_VARARGS},
  {"colCount", colCount, METH_VARARGS},
  {"rowCount", rowCount, METH_VARARGS},
  {"totalRows", totalRows, METH_VARARGS},
  {"affectedRows", affectedRows, METH_VARARGS},
  {"firstRowIndex", firstRowIndex, METH_VARARGS},
  {"endOfResultSet", endOfResultSet, METH_VARARGS},
  {"errorMessage", errorMessage, METH_VARARGS},
  {"getNullsAsEmptyStrings", getNullsAsEmptyStrings, METH_VARARGS},
  {"getNullsAsNone", getNullsAsNone, METH_VARARGS},
  {"getField", getField, METH_VARARGS},
  {"getFieldAsLong", getFieldAsLong, METH_VARARGS},
  {"getFieldAsDouble", getFieldAsDouble, METH_VARARGS},
  {"getFieldLength", getFieldLength, METH_VARARGS},
  {"getRow", getRow, METH_VARARGS},
  {"getRowDictionary", getRowDictionary, METH_VARARGS},
  {"getRowRange", getRowRange, METH_VARARGS},
  {"getRowLengths", getRowLengths, METH_VARARGS},
  {"getRowLengthsDictionary", getRowLengthsDictionary, METH_VARARGS},
  {"getRowLengthsRange", getRowLengthsRange, METH_VARARGS},
  {"getColumnName", getColumnName, METH_VARARGS},
  {"getColumnNames", getColumnNames, METH_VARARGS},
  {"getColumnType", getColumnType, METH_VARARGS},
  {"getColumnLength", getColumnLength, METH_VARARGS},
  {"getColumnPrecision", getColumnPrecision, METH_VARARGS},
  {"getColumnScale", getColumnScale, METH_VARARGS},
  {"getColumnIsNullable", getColumnIsNullable, METH_VARARGS},
  {"getColumnIsPrimaryKey", getColumnIsPrimaryKey, METH_VARARGS},
  {"getColumnIsUnique", getColumnIsUnique, METH_VARARGS},
  {"getColumnIsPartOfKey", getColumnIsPartOfKey, METH_VARARGS},
  {"getColumnIsUnsigned", getColumnIsUnsigned, METH_VARARGS},
  {"getColumnIsZeroFilled", getColumnIsZeroFilled, METH_VARARGS},
  {"getColumnIsBinary", getColumnIsBinary, METH_VARARGS},
  {"getColumnIsAutoIncrement", getColumnIsAutoIncrement, METH_VARARGS},
  {"getLongest", getLongest, METH_VARARGS},
  {"getResultSetId", getResultSetId, METH_VARARGS},
  {"suspendResultSet", suspendResultSet, METH_VARARGS},
  {"resumeResultSet", resumeResultSet, METH_VARARGS},
  {"resumeCachedResultSet", resumeCachedResultSet, METH_VARARGS},
  {"getOutputBindCursorId", getOutputBindCursorId, METH_VARARGS},
  {"attachToBindCursor", attachToBindCursor, METH_VARARGS},
  {NULL,      NULL}        /* Sentinel */
};

void
initCSQLRelay()
{
  (void) Py_InitModule("SQLRelay.CSQLRelay", SQLRMethods);
}

}
