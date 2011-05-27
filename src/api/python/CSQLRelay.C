/* Copyright (c) 2000 Roman Milner
   See the file COPYING for more information */

#ifdef __CYGWIN__
	#include <windows.h>
#endif

#include <Python.h>
#include <sqlrelay/sqlrclient.h>

#if PY_MAJOR_VERSION >= 2
#if PY_MINOR_VERSION >= 3
	#define SUPPORTS_UNSIGNED	1
#endif
#endif


extern "C" {

#define NEED_IS_NUMBER_TYPE_CHAR
#define NEED_IS_FLOAT_TYPE_CHAR
#define NEED_IS_BIT_TYPE_CHAR
#define NEED_BIT_STRING_TO_LONG
#define NEED_IS_BOOL_TYPE_CHAR
#include <datatypes.h>

PyObject *decimalmodule=NULL;
PyObject *decimal=NULL;

static PyObject *sqlrcon_alloc(PyObject *self, PyObject *args) {
  sqlrconnection *sqlrcon;
  char *host;
  char *user;
  char *password;
  char *socket;
  uint16_t port;
  int32_t retrytime;
  int32_t tries;
  if (!PyArg_ParseTuple(args,
#ifdef SUPPORTS_UNSIGNED
		"sHsssii",
#else
		"shsssii",
#endif
		&host, &port, &socket, &user, &password, &retrytime, &tries))
    return NULL;
  sqlrcon = new sqlrconnection(host, port, socket, user, password,
							retrytime, tries);
  sqlrcon->copyReferences();
  return Py_BuildValue("l", (long)sqlrcon);
}

static PyObject *sqlrcon_free(PyObject *self, PyObject *args) {
  long sqlrcon;
  if (!PyArg_ParseTuple(args, "l", &sqlrcon))
    return NULL;
  Py_BEGIN_ALLOW_THREADS
  delete ((sqlrconnection *)sqlrcon);
  Py_END_ALLOW_THREADS
  return Py_BuildValue("h", 0);
}

static PyObject *setTimeout(PyObject *self, PyObject *args) {
  long sqlrcon;
  int32_t timeoutsec;
  int32_t timeoutusec;
  if (!PyArg_ParseTuple(args, "lii", &sqlrcon, &timeoutsec, &timeoutusec))
    return NULL;
  ((sqlrconnection *)sqlrcon)->setTimeout(timeoutsec,timeoutusec);
  return Py_BuildValue("h", 0);
}

static PyObject *endSession(PyObject *self, PyObject *args) {
  long sqlrcon;
  if (!PyArg_ParseTuple(args, "l", &sqlrcon))
    return NULL;
  Py_BEGIN_ALLOW_THREADS
  ((sqlrconnection *)sqlrcon)->endSession();
  Py_END_ALLOW_THREADS
  return Py_BuildValue("h", 0);
}

static PyObject *suspendSession(PyObject *self, PyObject *args) {
  long sqlrcon;
  bool rc;
  if (!PyArg_ParseTuple(args, "l", &sqlrcon))
    return NULL;
  Py_BEGIN_ALLOW_THREADS
  rc=((sqlrconnection *)sqlrcon)->suspendSession();
  Py_END_ALLOW_THREADS
  return Py_BuildValue("h", (short)rc);
}

static PyObject *getConnectionPort(PyObject *self, PyObject *args) {
  long sqlrcon;
  bool rc;
  if (!PyArg_ParseTuple(args, "l", &sqlrcon))
    return NULL;
  rc=((sqlrconnection *)sqlrcon)->getConnectionPort();
  return Py_BuildValue("h", (short)rc);
}

static PyObject *getConnectionSocket(PyObject *self, PyObject *args) {
  long sqlrcon;
  const char *rc;
  if (!PyArg_ParseTuple(args, "l", &sqlrcon))
    return NULL;
  rc=((sqlrconnection *)sqlrcon)->getConnectionSocket();
  return Py_BuildValue("s", rc);
}

static PyObject *resumeSession(PyObject *self, PyObject *args) {
  long sqlrcon;
  uint16_t port;
  char *socket;
  bool rc;
  if (!PyArg_ParseTuple(args,
#ifdef SUPPORTS_UNSIGNED
	"lHs",
#else
	"lhs",
#endif
	&sqlrcon, &port, &socket))
    return NULL;
  Py_BEGIN_ALLOW_THREADS
  rc=((sqlrconnection *)sqlrcon)->resumeSession(port,socket);
  Py_END_ALLOW_THREADS
  return Py_BuildValue("h", (short)rc);
}

static PyObject *ping(PyObject *self, PyObject *args) {
  long sqlrcon;
  bool rc;
  if (!PyArg_ParseTuple(args, "l", &sqlrcon))
    return NULL;
  Py_BEGIN_ALLOW_THREADS
  rc=((sqlrconnection *)sqlrcon)->ping();
  Py_END_ALLOW_THREADS
  return Py_BuildValue("h", (short)rc);
}

static PyObject *identify(PyObject *self, PyObject *args) {
  long sqlrcon;
  const char *rc;
  if (!PyArg_ParseTuple(args, "l", &sqlrcon))
    return NULL;
  Py_BEGIN_ALLOW_THREADS
  rc=((sqlrconnection *)sqlrcon)->identify();
  Py_END_ALLOW_THREADS
  return Py_BuildValue("s", rc);
}

static PyObject *dbVersion(PyObject *self, PyObject *args) {
  long sqlrcon;
  const char *rc;
  if (!PyArg_ParseTuple(args, "l", &sqlrcon))
    return NULL;
  Py_BEGIN_ALLOW_THREADS
  rc=((sqlrconnection *)sqlrcon)->dbVersion();
  Py_END_ALLOW_THREADS
  return Py_BuildValue("s", rc);
}

static PyObject *serverVersion(PyObject *self, PyObject *args) {
  long sqlrcon;
  const char *rc;
  if (!PyArg_ParseTuple(args, "l", &sqlrcon))
    return NULL;
  Py_BEGIN_ALLOW_THREADS
  rc=((sqlrconnection *)sqlrcon)->serverVersion();
  Py_END_ALLOW_THREADS
  return Py_BuildValue("s", rc);
}

static PyObject *clientVersion(PyObject *self, PyObject *args) {
  long sqlrcon;
  const char *rc;
  if (!PyArg_ParseTuple(args, "l", &sqlrcon))
    return NULL;
  Py_BEGIN_ALLOW_THREADS
  rc=((sqlrconnection *)sqlrcon)->clientVersion();
  Py_END_ALLOW_THREADS
  return Py_BuildValue("s", rc);
}

static PyObject *bindFormat(PyObject *self, PyObject *args) {
  long sqlrcon;
  const char *rc;
  if (!PyArg_ParseTuple(args, "l", &sqlrcon))
    return NULL;
  Py_BEGIN_ALLOW_THREADS
  rc=((sqlrconnection *)sqlrcon)->bindFormat();
  Py_END_ALLOW_THREADS
  return Py_BuildValue("s", rc);
}

static PyObject *autoCommitOn(PyObject *self, PyObject *args) {
  long sqlrcon;
  bool rc;
  if (!PyArg_ParseTuple(args, "l", &sqlrcon))
    return NULL;
  Py_BEGIN_ALLOW_THREADS
  rc=((sqlrconnection *)sqlrcon)->autoCommitOn();
  Py_END_ALLOW_THREADS
  return Py_BuildValue("h", (short)rc);
}

static PyObject *autoCommitOff(PyObject *self, PyObject *args) {
  long sqlrcon;
  bool rc;
  if (!PyArg_ParseTuple(args, "l", &sqlrcon))
    return NULL;
  Py_BEGIN_ALLOW_THREADS
  rc=((sqlrconnection *)sqlrcon)->autoCommitOff();
  Py_END_ALLOW_THREADS
  return Py_BuildValue("h", (short)rc);
}

static PyObject *commit(PyObject *self, PyObject *args) {
  long sqlrcon;
  bool rc;
  if (!PyArg_ParseTuple(args, "l", &sqlrcon))
    return NULL;
  Py_BEGIN_ALLOW_THREADS
  rc=((sqlrconnection *)sqlrcon)->commit();
  Py_END_ALLOW_THREADS
  return Py_BuildValue("h", (short)rc);
}

static PyObject *rollback(PyObject *self, PyObject *args) {
  long sqlrcon;
  bool rc;
  if (!PyArg_ParseTuple(args, "l", &sqlrcon))
    return NULL;
  Py_BEGIN_ALLOW_THREADS
  rc=((sqlrconnection *)sqlrcon)->rollback();
  Py_END_ALLOW_THREADS
  return Py_BuildValue("h", (short)rc);
}

static PyObject *debugOn(PyObject *self, PyObject *args) {
  long sqlrcon;
  if (!PyArg_ParseTuple(args, "l", &sqlrcon))
    return NULL;
  ((sqlrconnection *)sqlrcon)->debugOn();
  return Py_BuildValue("h", 0);
}

static PyObject *debugOff(PyObject *self, PyObject *args) {
  long sqlrcon;
  if (!PyArg_ParseTuple(args, "l", &sqlrcon))
    return NULL;
  ((sqlrconnection *)sqlrcon)->debugOff();
  return Py_BuildValue("h", 0);
}

static PyObject *getDebug(PyObject *self, PyObject *args) {
  long sqlrcon;
  if (!PyArg_ParseTuple(args, "l", &sqlrcon))
    return NULL;
  return Py_BuildValue("h", (short)((sqlrconnection *)sqlrcon)->getDebug());
}

static PyObject *sqlrcur_alloc(PyObject *self, PyObject *args) {
  long	sqlrcon;
  sqlrcursor *sqlrcur;
  if (!PyArg_ParseTuple(args, "l", &sqlrcon))
    return NULL;
  sqlrcur = new sqlrcursor((sqlrconnection *)sqlrcon);
  sqlrcur->copyReferences();
  return Py_BuildValue("l", (long)sqlrcur);
}

static PyObject *sqlrcur_free(PyObject *self, PyObject *args) {
  long sqlrcur;
  if (!PyArg_ParseTuple(args, "l", &sqlrcur))
    return NULL;
  Py_BEGIN_ALLOW_THREADS
  delete ((sqlrcursor *)sqlrcur);
  Py_END_ALLOW_THREADS
  return Py_BuildValue("h", 0);
}

static PyObject *setResultSetBufferSize(PyObject *self, PyObject *args) {
  long sqlrcur;
  uint64_t rows;
  if (!PyArg_ParseTuple(args,
#ifdef SUPPORTS_UNSIGNED
	"lK",
#else
	"lL",
#endif
	&sqlrcur, &rows))
    return NULL;
  ((sqlrcursor *)sqlrcur)->setResultSetBufferSize(rows);
  return Py_BuildValue("h", 0);
}

static PyObject *getResultSetBufferSize(PyObject *self, PyObject *args) {
  long sqlrcur;
  uint64_t rc;
  if (!PyArg_ParseTuple(args, "l", &sqlrcur))
    return NULL;
  rc=((sqlrcursor *)sqlrcur)->getResultSetBufferSize();
  // FIXME: lame, python doesn't support building values from uint64_t's
  return Py_BuildValue("l", (long)rc);
}

static PyObject *dontGetColumnInfo(PyObject *self, PyObject *args) {
  long sqlrcur;
  if (!PyArg_ParseTuple(args, "l", &sqlrcur))
    return NULL;
  ((sqlrcursor *)sqlrcur)->dontGetColumnInfo();
  return Py_BuildValue("h", 0);
}

static PyObject *getColumnInfo(PyObject *self, PyObject *args) {
  long sqlrcur;
  if (!PyArg_ParseTuple(args, "l", &sqlrcur))
    return NULL;
  ((sqlrcursor *)sqlrcur)->getColumnInfo();
  return Py_BuildValue("h", 0);
}

static PyObject *mixedCaseColumnNames(PyObject *self, PyObject *args) {
  long sqlrcur;
  if (!PyArg_ParseTuple(args, "l", &sqlrcur))
    return NULL;
  ((sqlrcursor *)sqlrcur)->mixedCaseColumnNames();
  return Py_BuildValue("h", 0);
}

static PyObject *upperCaseColumnNames(PyObject *self, PyObject *args) {
  long sqlrcur;
  if (!PyArg_ParseTuple(args, "l", &sqlrcur))
    return NULL;
  ((sqlrcursor *)sqlrcur)->upperCaseColumnNames();
  return Py_BuildValue("h", 0);
}

static PyObject *lowerCaseColumnNames(PyObject *self, PyObject *args) {
  long sqlrcur;
  if (!PyArg_ParseTuple(args, "l", &sqlrcur))
    return NULL;
  ((sqlrcursor *)sqlrcur)->lowerCaseColumnNames();
  return Py_BuildValue("h", 0);
}

static PyObject *cacheToFile(PyObject *self, PyObject *args) {
  long sqlrcur;
  char *filename;
  if (!PyArg_ParseTuple(args, "ls", &sqlrcur, &filename))
    return NULL;
  ((sqlrcursor *)sqlrcur)->cacheToFile(filename);
  return Py_BuildValue("h", 0);
}

static PyObject *setCacheTtl(PyObject *self, PyObject *args) {
  long sqlrcur;
  uint32_t ttl;
  if (!PyArg_ParseTuple(args,
#ifdef SUPPORTS_UNSIGNED
	"lI",
#else
	"li",
#endif
	&sqlrcur, &ttl))
    return NULL;
  ((sqlrcursor *)sqlrcur)->setCacheTtl(ttl);
  return Py_BuildValue("h", 0);
}

static PyObject *getCacheFileName(PyObject *self, PyObject *args) {
  long sqlrcur;
  const char *rc;
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
  return Py_BuildValue("h", 0);
}

static PyObject *sendQuery(PyObject *self, PyObject *args) {
  char *sqlString;
  long sqlrcur;
  bool rc;
  if (!PyArg_ParseTuple(args, "ls", &sqlrcur, &sqlString))
    return NULL;
  Py_BEGIN_ALLOW_THREADS
  rc=((sqlrcursor *)sqlrcur)->sendQuery(sqlString);
  Py_END_ALLOW_THREADS
  return Py_BuildValue("h", (short)rc);
}

static PyObject *sendQueryWithLength(PyObject *self, PyObject *args) {
  char *sqlString;
  long sqlrcur;
  uint32_t length;
  bool rc;
  if (!PyArg_ParseTuple(args,
#ifdef SUPPORTS_UNSIGNED
	"lsI",
#else
	"lsi",
#endif
	&sqlrcur, &sqlString, &length))
    return NULL;
  Py_BEGIN_ALLOW_THREADS
  rc=((sqlrcursor *)sqlrcur)->sendQuery(sqlString,length);
  Py_END_ALLOW_THREADS
  return Py_BuildValue("h", (short)rc);
}

static PyObject *sendFileQuery(PyObject *self, PyObject *args) {
  char *path;
  char *file;
  long sqlrcur;
  bool rc;
  if (!PyArg_ParseTuple(args, "lss", &sqlrcur, &path, &file))
    return NULL;
  Py_BEGIN_ALLOW_THREADS
  rc=((sqlrcursor *)sqlrcur)->sendFileQuery(path, file);
  Py_END_ALLOW_THREADS
  return Py_BuildValue("h", (short)rc);
}

static PyObject *prepareQuery(PyObject *self, PyObject *args) {
  long sqlrcur;
  char *sqlString;
  if (!PyArg_ParseTuple(args, "ls", &sqlrcur, &sqlString))
    return NULL;
  ((sqlrcursor *)sqlrcur)->prepareQuery(sqlString);
  return Py_BuildValue("h", 0);
}

static PyObject *prepareQueryWithLength(PyObject *self, PyObject *args) {
  char *sqlString;
  long sqlrcur;
  uint32_t length;
  if (!PyArg_ParseTuple(args,
#ifdef SUPPORTS_UNSIGNED
	"lsI",
#else
	"lsi",
#endif
	&sqlrcur, &sqlString, &length))
    return NULL;
  ((sqlrcursor *)sqlrcur)->prepareQuery(sqlString,length);
  return Py_BuildValue("h", 0);
}

static PyObject *prepareFileQuery(PyObject *self, PyObject *args) {
  char *path;
  char *file;
  long sqlrcur;
  bool rc;
  if (!PyArg_ParseTuple(args, "lss", &sqlrcur, &path, &file))
    return NULL;
  Py_BEGIN_ALLOW_THREADS
  rc=((sqlrcursor *)sqlrcur)->prepareFileQuery(path, file);
  Py_END_ALLOW_THREADS
  return Py_BuildValue("h", (short)rc);
}

static PyObject *substitution(PyObject *self, PyObject *args) {
  long sqlrcur;
  char *variable;
  PyObject *value;
  uint32_t precision;
  uint32_t scale;
  uint16_t success;
  if (!PyArg_ParseTuple(args,
#ifdef SUPPORTS_UNSIGNED
	"lsOII",
#else
	"lsOii",
#endif
	&sqlrcur, &variable, &value, &precision, &scale))
    return NULL;
  success=1;
  if (value==Py_None) {
    ((sqlrcursor *)sqlrcur)->substitution(variable, (char *)NULL);
  } else if (PyString_Check(value)) {
    ((sqlrcursor *)sqlrcur)->substitution(variable, PyString_AsString(value));
  } else if (PyInt_Check(value)) {
    ((sqlrcursor *)sqlrcur)->substitution(variable, (int64_t)PyInt_AsLong(value));
  } else if (PyFloat_Check(value)) {
    ((sqlrcursor *)sqlrcur)->substitution(variable, (double)PyFloat_AsDouble(value), precision, scale);
  } else {
    success=0;
  }
  return Py_BuildValue("h", success);
}

static PyObject *substitutions(PyObject *self, PyObject *args) {
  PyObject *variables;
  PyObject *values;
  PyObject *precisions;
  PyObject *scales;
  long sqlrcur;
  char *variable;
  uint16_t success;
  PyObject *value;
  if (!PyArg_ParseTuple(args, "lOOOO", &sqlrcur, &variables, &values, &precisions, &scales))
    return NULL;
  success=1;
  if (PyList_Check(variables) && PyList_Check(values)) {
    for (int i=0; i<PyList_Size(variables); i++) {
      variable=PyString_AsString(PyList_GetItem(variables,i));
      value=PyList_GetItem(values,i);
      if (value==Py_None) {
        ((sqlrcursor *)sqlrcur)->substitution(variable, (char *)NULL);
      } else if (PyString_Check(value)) {
        ((sqlrcursor *)sqlrcur)->substitution(variable, PyString_AsString(value));
      } else if (PyInt_Check(value)) {
        ((sqlrcursor *)sqlrcur)->substitution(variable, (int64_t)PyInt_AsLong(value));
      } else if (PyFloat_Check(value)) {
        ((sqlrcursor *)sqlrcur)->substitution(variable, (double)PyFloat_AsDouble(value), (uint32_t)PyInt_AsLong(PyList_GetItem(precisions,i)), (uint32_t)PyInt_AsLong(PyList_GetItem(scales,i)));
      } else {
        success=0;
      }
    }
  }
  return Py_BuildValue("h", success);
}

static PyObject *clearBinds(PyObject *self, PyObject *args) {
  long sqlrcur;
  if (!PyArg_ParseTuple(args, "l", &sqlrcur))
    return NULL;
  ((sqlrcursor *)sqlrcur)->clearBinds();
  return Py_BuildValue("h", 0);
}

static PyObject *countBindVariables(PyObject *self, PyObject *args) {
  long sqlrcur;
  uint16_t rc;
  if (!PyArg_ParseTuple(args, "l", &sqlrcur))
    return NULL;
  rc=((sqlrcursor *)sqlrcur)->countBindVariables();
  // FIXME: lame, python doesn't support building values from uint16_t's
  return Py_BuildValue("h", (short)rc);
}

static PyObject *inputBind(PyObject *self, PyObject *args) {
  char *variable;
  PyObject *value;
  uint32_t precision;
  uint32_t scale;
  long sqlrcur;
  uint16_t success;
  if (!PyArg_ParseTuple(args,
#ifdef SUPPORTS_UNSIGNED
	"lsOII",
#else
	"lsOii",
#endif
	&sqlrcur, &variable, &value, &precision, &scale))
    return NULL;
  success=1;
  if (value==Py_None) {
    ((sqlrcursor *)sqlrcur)->inputBind(variable, (char *)NULL);
  } else if (PyString_Check(value)) {
    ((sqlrcursor *)sqlrcur)->inputBind(variable, PyString_AsString(value));
  } else if (PyBool_Check(value)) {
    ((sqlrcursor *)sqlrcur)->inputBind(variable, value == Py_True ? "1" : "0");
  } else if (PyInt_Check(value)) {
    ((sqlrcursor *)sqlrcur)->inputBind(variable, (int64_t)PyInt_AsLong(value));
  } else if (PyFloat_Check(value)) {
    ((sqlrcursor *)sqlrcur)->inputBind(variable, (double)PyFloat_AsDouble(value), (uint32_t)precision, (uint32_t)scale);
  } else {
    ((sqlrcursor *)sqlrcur)->inputBind(variable, PyString_AsString(PyObject_Str(value)));
  }
  return Py_BuildValue("h", success);
}

static PyObject *inputBindBlob(PyObject *self, PyObject *args) {
  char *variable;
  PyObject *value;
  uint32_t size;
  long sqlrcur;
  uint16_t success;
  if (!PyArg_ParseTuple(args,
#ifdef SUPPORTS_UNSIGNED
	"lsOI",
#else
	"lsOi",
#endif
	&sqlrcur, &variable, &value, &size))
    return NULL;
  success=1;
  if (value==Py_None) {
    ((sqlrcursor *)sqlrcur)->inputBindBlob(variable, NULL, size);
  } else if (PyString_Check(value)) {
    ((sqlrcursor *)sqlrcur)->inputBindBlob(variable, PyString_AsString(value), size);
  } else {
    success=0;
  }
  return Py_BuildValue("h", success);
}

static PyObject *inputBindClob(PyObject *self, PyObject *args) {
  char *variable;
  PyObject *value;
  uint32_t size;
  long sqlrcur;
  uint16_t success;
  if (!PyArg_ParseTuple(args,
#ifdef SUPPORTS_UNSIGNED
	"lsOI",
#else
	"lsOi",
#endif
	&sqlrcur, &variable, &value, &size))
    return NULL;
  success=1;
  if (value==Py_None) {
    ((sqlrcursor *)sqlrcur)->inputBindClob(variable, NULL, size);
  } else if (PyString_Check(value)) {
    ((sqlrcursor *)sqlrcur)->inputBindClob(variable, PyString_AsString(value), size);
  } else {
    success=0;
  }
  return Py_BuildValue("h", success);
}

static PyObject *inputBinds(PyObject *self, PyObject *args) {
  PyObject *variables;
  PyObject *values;
  PyObject *precisions;
  PyObject *scales;
  long sqlrcur;
  char *variable;
  uint16_t success;
  PyObject *value;
  if (!PyArg_ParseTuple(args, "lOOOO", &sqlrcur, &variables, &values, &precisions, &scales))
    return NULL;
  success=1;
  if (PyList_Check(variables) && PyList_Check(values)) {
    for (int i=0; i<PyList_Size(variables); i++) {
      variable=PyString_AsString(PyList_GetItem(variables,i));
      value=PyList_GetItem(values,i);
      if (value==Py_None) {
        ((sqlrcursor *)sqlrcur)->inputBind(variable, (char *)NULL);
      } else if (PyString_Check(value)) {
        ((sqlrcursor *)sqlrcur)->inputBind(variable, PyString_AsString(value));
      } else if (PyBool_Check(value)) {
	((sqlrcursor *)sqlrcur)->inputBind(variable, value == Py_True ? "1" : "0");
      } else if (PyInt_Check(value)) {
        ((sqlrcursor *)sqlrcur)->inputBind(variable, (int64_t)PyInt_AsLong(value));
      } else if (PyFloat_Check(value)) {
        ((sqlrcursor *)sqlrcur)->inputBind(variable, (double)PyFloat_AsDouble(value), (unsigned short)PyInt_AsLong(PyList_GetItem(precisions,i)), (unsigned short)PyInt_AsLong(PyList_GetItem(scales,i)));
      } else {
	((sqlrcursor *)sqlrcur)->inputBind(variable, PyString_AsString(PyObject_Str(value)));
      }
    }
  }
  return Py_BuildValue("h", success);
}

static PyObject *defineOutputBindString(PyObject *self, PyObject *args) {
  char *variable;
  uint32_t length;
  long sqlrcur;
  if (!PyArg_ParseTuple(args,
#ifdef SUPPORTS_UNSIGNED
	"lsI",
#else
	"lsi",
#endif
	&sqlrcur, &variable, &length))
    return NULL;
  ((sqlrcursor *)sqlrcur)->defineOutputBindString(variable, length);
  return Py_BuildValue("h", 0);
}

static PyObject *defineOutputBindInteger(PyObject *self, PyObject *args) {
  char *variable;
  long sqlrcur;
  if (!PyArg_ParseTuple(args,"ls",&sqlrcur, &variable))
    return NULL;
  ((sqlrcursor *)sqlrcur)->defineOutputBindInteger(variable);
  return Py_BuildValue("h", 0);
}

static PyObject *defineOutputBindDouble(PyObject *self, PyObject *args) {
  char *variable;
  long sqlrcur;
  if (!PyArg_ParseTuple(args,"ls",&sqlrcur, &variable))
    return NULL;
  ((sqlrcursor *)sqlrcur)->defineOutputBindDouble(variable);
  return Py_BuildValue("h", 0);
}

static PyObject *defineOutputBindBlob(PyObject *self, PyObject *args) {
  char *variable;
  long sqlrcur;
  if (!PyArg_ParseTuple(args, "ls", &sqlrcur, &variable))
    return NULL;
  ((sqlrcursor *)sqlrcur)->defineOutputBindBlob(variable);
  return Py_BuildValue("h", 0);
}

static PyObject *defineOutputBindClob(PyObject *self, PyObject *args) {
  char *variable;
  long sqlrcur;
  if (!PyArg_ParseTuple(args, "ls", &sqlrcur, &variable))
    return NULL;
  ((sqlrcursor *)sqlrcur)->defineOutputBindClob(variable);
  return Py_BuildValue("h", 0);
}

static PyObject *defineOutputBindCursor(PyObject *self, PyObject *args) {
  char *variable;
  long sqlrcur;
  if (!PyArg_ParseTuple(args, "ls", &sqlrcur, &variable))
    return NULL;
  ((sqlrcursor *)sqlrcur)->defineOutputBindCursor(variable);
  return Py_BuildValue("h", 0);
}

static PyObject *validateBinds(PyObject *self, PyObject *args) {
  long sqlrcur;
  if (!PyArg_ParseTuple(args, "l", &sqlrcur))
    return NULL;
  ((sqlrcursor *)sqlrcur)->validateBinds();
  return Py_BuildValue("h", 0);
}

static PyObject *validBind(PyObject *self, PyObject *args) {
  long sqlrcur;
  bool rc;
  char *variable;
  if (!PyArg_ParseTuple(args, "ls", &sqlrcur, &variable))
    return NULL;
  Py_BEGIN_ALLOW_THREADS
  rc=((sqlrcursor *)sqlrcur)->validBind(variable);
  Py_END_ALLOW_THREADS
  return Py_BuildValue("h", (short)rc);
}

static PyObject *executeQuery(PyObject *self, PyObject *args) {
  long sqlrcur;
  bool rc;
  if (!PyArg_ParseTuple(args, "l", &sqlrcur))
    return NULL;
  Py_BEGIN_ALLOW_THREADS
  rc=((sqlrcursor *)sqlrcur)->executeQuery();
  Py_END_ALLOW_THREADS
  return Py_BuildValue("h", (short)rc);
}

static PyObject *fetchFromBindCursor(PyObject *self, PyObject *args) {
  long sqlrcur;
  bool rc;
  if (!PyArg_ParseTuple(args, "l", &sqlrcur))
    return NULL;
  Py_BEGIN_ALLOW_THREADS
  rc=((sqlrcursor *)sqlrcur)->fetchFromBindCursor();
  Py_END_ALLOW_THREADS
  return Py_BuildValue("h", (short)rc);
}

static PyObject *getOutputBindString(PyObject *self, PyObject *args) {
  char *variable;
  long sqlrcur;
  const char *rc;
  uint32_t rl;
  if (!PyArg_ParseTuple(args, "ls", &sqlrcur, &variable))
    return NULL;
  rc=((sqlrcursor *)sqlrcur)->getOutputBindString(variable);
  rl=((sqlrcursor *)sqlrcur)->getOutputBindLength(variable);
  return Py_BuildValue("s#", rc, rl);
}

static PyObject *getOutputBindBlob(PyObject *self, PyObject *args) {
  char *variable;
  long sqlrcur;
  const char *rc;
  uint32_t rl;
  if (!PyArg_ParseTuple(args, "ls", &sqlrcur, &variable))
    return NULL;
  rc=((sqlrcursor *)sqlrcur)->getOutputBindBlob(variable);
  rl=((sqlrcursor *)sqlrcur)->getOutputBindLength(variable);
  return Py_BuildValue("s#", rc, rl);
}

static PyObject *getOutputBindClob(PyObject *self, PyObject *args) {
  char *variable;
  long sqlrcur;
  const char *rc;
  uint32_t rl;
  if (!PyArg_ParseTuple(args, "ls", &sqlrcur, &variable))
    return NULL;
  rc=((sqlrcursor *)sqlrcur)->getOutputBindClob(variable);
  rl=((sqlrcursor *)sqlrcur)->getOutputBindLength(variable);
  return Py_BuildValue("s#", rc, rl);
}

static PyObject *getOutputBindInteger(PyObject *self, PyObject *args) {
  char *variable;
  long sqlrcur;
  int64_t rc;
  if (!PyArg_ParseTuple(args, "ls", &sqlrcur, &variable))
    return NULL;
  rc=((sqlrcursor *)sqlrcur)->getOutputBindInteger(variable);
  // FIXME: lame, python doesn't support building values from int64_t's
  return Py_BuildValue("l", (long)rc);
}

static PyObject *getOutputBindDouble(PyObject *self, PyObject *args) {
  char *variable;
  long sqlrcur;
  double rc;
  if (!PyArg_ParseTuple(args, "ls", &sqlrcur, &variable))
    return NULL;
  rc=((sqlrcursor *)sqlrcur)->getOutputBindDouble(variable);
  return Py_BuildValue("d", rc);
}

static PyObject *getOutputBindLength(PyObject *self, PyObject *args) {
  char *variable;
  long sqlrcur;
  uint32_t rc;
  if (!PyArg_ParseTuple(args, "ls", &sqlrcur, &variable))
    return NULL;
  rc=((sqlrcursor *)sqlrcur)->getOutputBindLength(variable);
  // FIXME: lame, python doesn't support building values from uint32_t's
  return Py_BuildValue("l", (long)rc);
}

static PyObject *openCachedResultSet(PyObject *self, PyObject *args) {
  long sqlrcur;
  bool rc;
  char *filename;
  if (!PyArg_ParseTuple(args, "ls", &sqlrcur, &filename))
    return NULL;
  Py_BEGIN_ALLOW_THREADS
  rc=((sqlrcursor *)sqlrcur)->openCachedResultSet(filename);
  Py_END_ALLOW_THREADS
  return Py_BuildValue("h", (short)rc);
}

static PyObject *colCount(PyObject *self, PyObject *args) {
  long sqlrcur;
  uint32_t rc;
  if (!PyArg_ParseTuple(args, "l", &sqlrcur))
    return NULL;
  rc=((sqlrcursor *)sqlrcur)->colCount();
  // FIXME: lame, python doesn't support building values from uint32_t's
  return Py_BuildValue("l", (long)rc);
}

static PyObject *rowCount(PyObject *self, PyObject *args) {
  long sqlrcur;
  uint64_t rc;
  if (!PyArg_ParseTuple(args, "l", &sqlrcur))
    return NULL;
  rc=((sqlrcursor *)sqlrcur)->rowCount();
  // FIXME: lame, python doesn't support building values from uint64_t's
  return Py_BuildValue("l", (long)rc);
}

static PyObject *totalRows(PyObject *self, PyObject *args) {
  long sqlrcur;
  uint64_t rc;
  if (!PyArg_ParseTuple(args, "l", &sqlrcur))
    return NULL;
  rc=((sqlrcursor *)sqlrcur)->totalRows();
  // FIXME: lame, python doesn't support building values from uint64_t's
  return Py_BuildValue("l", (long)rc);
}

static PyObject *affectedRows(PyObject *self, PyObject *args) {
  long sqlrcur;
  uint64_t rc;
  if (!PyArg_ParseTuple(args, "l", &sqlrcur))
    return NULL;
  rc=((sqlrcursor *)sqlrcur)->affectedRows();
  // FIXME: lame, python doesn't support building values from uint64_t's
  return Py_BuildValue("l", (long)rc);
}

static PyObject *firstRowIndex(PyObject *self, PyObject *args) {
  long sqlrcur;
  uint64_t rc;
  if (!PyArg_ParseTuple(args, "l", &sqlrcur))
    return NULL;
  rc=((sqlrcursor *)sqlrcur)->firstRowIndex();
  // FIXME: lame, python doesn't support building values from uint64_t's
  return Py_BuildValue("l", (long)rc);
}

static PyObject *endOfResultSet(PyObject *self, PyObject *args) {
  long sqlrcur;
  bool rc;
  if (!PyArg_ParseTuple(args, "l", &sqlrcur))
    return NULL;
  rc=((sqlrcursor *)sqlrcur)->endOfResultSet();
  return Py_BuildValue("h", (short)rc);
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
  return Py_BuildValue("h", 0);
}

static PyObject *getNullsAsNone(PyObject *self, PyObject *args) {
  long sqlrcur;
  if (!PyArg_ParseTuple(args, "l", &sqlrcur))
    return NULL;
  ((sqlrcursor *)sqlrcur)->getNullsAsNulls();
  return Py_BuildValue("h", 0);
}

static PyObject *getField(PyObject *self, PyObject *args) {
  long sqlrcur;
  const char *rc="";
  uint32_t  rl=0;
  uint64_t row;
  PyObject *col;
  const char* type;
  if (!PyArg_ParseTuple(args,
#ifdef SUPPORTS_UNSIGNED
	"lKO",
#else
	"lLO",
#endif
	&sqlrcur, &row, &col))
    return NULL;
  Py_BEGIN_ALLOW_THREADS
  if (PyString_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getField(row, PyString_AsString(col));
    rl=((sqlrcursor *)sqlrcur)->getFieldLength(row, PyString_AsString(col));
    type = ((sqlrcursor *)sqlrcur)->getColumnType(PyString_AsString(col));
  } else if (PyInt_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getField(row, PyInt_AsLong(col));
    rl=((sqlrcursor *)sqlrcur)->getFieldLength(row, PyInt_AsLong(col));
    type = ((sqlrcursor *)sqlrcur)->getColumnType(PyInt_AsLong(col));
  }
  Py_END_ALLOW_THREADS
  if (!rc) {
    Py_INCREF(Py_None);
    return Py_None;
  } else if (isFloatTypeChar(type)) {
    if (decimal) {
      PyObject *tuple=PyTuple_New(1);
      PyTuple_SetItem(tuple, 0, Py_BuildValue("s#", rc, rl));
      return PyObject_CallObject(decimal, tuple);
    } else {
      if (!charstring::contains(rc,'.')) {
	return Py_BuildValue("L",charstring::toInteger(rc));
      } else {
	return Py_BuildValue("f",atof(rc));
      }
    }
  } else if (isNumberTypeChar(type)) {
    if (!charstring::contains(rc,'.')) {
      return Py_BuildValue("L",charstring::toInteger(rc));
    } else {
      return Py_BuildValue("f",atof(rc));
    }
  } else if (isBitTypeChar(type)) {
    return Py_BuildValue("L",bitStringToLong(rc));
  } else if (isBoolTypeChar(type)) {
    if (rc && tolower(rc[0]) == 't') {
      Py_INCREF(Py_True);
      return Py_True;
    } else if (rc && tolower(rc[0]) == 'f') {
      Py_INCREF(Py_False);
      return Py_False;
    } else {
      Py_INCREF(Py_None);
      return Py_None;
    }
  }
  return Py_BuildValue("s#", rc, rl);
}

static PyObject *getFieldAsInteger(PyObject *self, PyObject *args) {
  long sqlrcur;
  int64_t rc=0;
  uint64_t row;
  PyObject *col;
  if (!PyArg_ParseTuple(args,
#ifdef SUPPORTS_UNSIGNED
	"lKO",
#else
	"lLO",
#endif
	&sqlrcur, &row, &col))
    return NULL;
  Py_BEGIN_ALLOW_THREADS
  if (PyString_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getFieldAsInteger(row, PyString_AsString(col));
  } else if (PyInt_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getFieldAsInteger(row, PyInt_AsLong(col));
  }
  Py_END_ALLOW_THREADS
  // FIXME: lame, python doesn't support building values from int64_t's
  return Py_BuildValue("l", (long)rc);
}

static PyObject *getFieldAsDouble(PyObject *self, PyObject *args) {
  long sqlrcur;
  double rc=0.0;
  uint64_t row;
  PyObject *col;
  if (!PyArg_ParseTuple(args,
#ifdef SUPPORTS_UNSIGNED
	"lKO",
#else
	"lLO",
#endif
	&sqlrcur, &row, &col))
    return NULL;
  Py_BEGIN_ALLOW_THREADS
  if (PyString_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getFieldAsDouble(row, PyString_AsString(col));
  } else if (PyInt_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getFieldAsDouble(row, PyInt_AsLong(col));
  }
  Py_END_ALLOW_THREADS
  return Py_BuildValue("d", rc);
}

static PyObject *getFieldLength(PyObject *self, PyObject *args) {
  long sqlrcur;
  uint32_t rc=0;
  uint64_t row;
  PyObject *col;
  if (!PyArg_ParseTuple(args,
#ifdef SUPPORTS_UNSIGNED
	"lKO",
#else
	"lLO",
#endif
	&sqlrcur, &row, &col))
    return NULL;
  Py_BEGIN_ALLOW_THREADS
  if (PyString_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getFieldLength(row, PyString_AsString(col));
  } else if (PyInt_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getFieldLength(row, PyInt_AsLong(col));
  }
  Py_END_ALLOW_THREADS
  // FIXME: lame, python doesn't support building values from uint32_t's
  return Py_BuildValue("l", (long)rc);
}

static PyObject *
_get_row(sqlrcursor *sqlrcur, uint64_t row)
{
  uint32_t num_cols;
  uint32_t counter;
  const char * const *row_data;
  uint32_t *row_lengths;
  PyObject *my_list;
  num_cols=sqlrcur->colCount();
  my_list =  PyList_New(num_cols);
  Py_BEGIN_ALLOW_THREADS
  row_data=sqlrcur->getRow(row);
  row_lengths=sqlrcur->getRowLengths(row);
  Py_END_ALLOW_THREADS
  if (!row_data) {
    Py_INCREF(Py_None);
    return Py_None;
  }
  for (counter = 0; counter < num_cols; ++counter) {
    if (!row_data[counter]) {
        Py_INCREF(Py_None);
        PyList_SetItem(my_list, counter, Py_None);
    } else if (isNumberTypeChar(sqlrcur->getColumnType(counter))) {
      PyObject *obj;
      if (decimal) {
        PyObject *tuple=PyTuple_New(1);
        PyTuple_SetItem(tuple, 0, Py_BuildValue("s#", row_data[counter], row_lengths[counter]));
        obj=PyObject_CallObject(decimal, tuple);
      } else {
        if (!charstring::contains(row_data[counter], '.')) {
          obj=Py_BuildValue("L", charstring::toInteger(row_data[counter]));
        } else {
          obj=Py_BuildValue("f", atof(row_data[counter]));
        }
      }
      PyList_SetItem(my_list, counter, obj);
    } else {
      PyList_SetItem(my_list, counter, Py_BuildValue("s#", row_data[counter], row_lengths[counter]));
    }
  }
  return my_list;
}

static PyObject *getRow(PyObject *self, PyObject *args) {
  long sqlrcur;
  uint64_t row;
  if (!PyArg_ParseTuple(args,
#ifdef SUPPORTS_UNSIGNED
	"lK",
#else
	"lL",
#endif
	&sqlrcur, &row))
    return NULL;
  return _get_row((sqlrcursor *)sqlrcur, row);
}

static PyObject *getRowDictionary(PyObject *self, PyObject *args) {
  long sqlrcur;
  uint64_t row;
  PyObject *my_dictionary;
  uint32_t counter;
  const char *field;
  const char *name;
  if (!PyArg_ParseTuple(args,
#ifdef SUPPORTS_UNSIGNED
	"lK",
#else
	"lL",
#endif
	&sqlrcur, &row))
    return NULL;
  my_dictionary=PyDict_New();
  for (counter=0; counter<((sqlrcursor *)sqlrcur)->colCount(); counter++) {
    Py_BEGIN_ALLOW_THREADS
    field=((sqlrcursor *)sqlrcur)->getField(row, counter);
    Py_END_ALLOW_THREADS
    name=((sqlrcursor *)sqlrcur)->getColumnName(counter);
    if (isNumberTypeChar(((sqlrcursor *)sqlrcur)->getColumnType(counter))) {
      if (!charstring::contains(field, '.')) {
        PyDict_SetItem(my_dictionary, Py_BuildValue("s", name), Py_BuildValue("L", charstring::toInteger(field)));
      } else {
        if (decimal) {
          PyObject *tuple=PyTuple_New(1);
          PyTuple_SetItem(tuple, 0, Py_BuildValue("s", field));
          PyDict_SetItem(my_dictionary, Py_BuildValue("s", name), PyObject_CallObject(decimal, tuple));
        } else {
          PyDict_SetItem(my_dictionary, Py_BuildValue("s", name), Py_BuildValue("f",atof(field)));
        }
      }
    } else {
      if (field) {
        PyDict_SetItem(my_dictionary, Py_BuildValue("s", name), Py_BuildValue("s#", field, ((sqlrcursor *)sqlrcur)->getFieldLength(row, counter)));
      } else {
        Py_INCREF(Py_None);
        PyDict_SetItem(my_dictionary, Py_BuildValue("s", name), Py_None);
      }
    }
  }
  return my_dictionary;
}

static PyObject *getRowRange(PyObject *self, PyObject *args) {
  uint64_t beg_row;
  uint64_t end_row;
  long sqlrcur;
  uint64_t counter;
  PyObject *my_list;
  my_list = PyList_New(0);
  if (!PyArg_ParseTuple(args,
#ifdef SUPPORTS_UNSIGNED
	"lKK",
#else
	"lLL",
#endif
	&sqlrcur, &beg_row, &end_row))
    return NULL;
  uint64_t max_rows=((sqlrcursor *)sqlrcur)->rowCount();
  if (end_row>=max_rows) {
	  end_row=max_rows-1;
  }
  for (counter = beg_row; counter <= end_row; ++counter) {
    PyList_Append(my_list, _get_row((sqlrcursor *)sqlrcur, counter));
  }
  return my_list;
}

static PyObject *
_get_row_lengths(sqlrcursor *sqlrcur, uint64_t row)
{
  uint32_t num_cols;
  uint32_t counter;
  uint32_t *row_data;
  PyObject *my_list;
  num_cols=sqlrcur->colCount();
  my_list =  PyList_New(num_cols);
  Py_BEGIN_ALLOW_THREADS
  row_data=sqlrcur->getRowLengths(row);
  Py_END_ALLOW_THREADS
  if (!row_data) {
    Py_INCREF(Py_None);
    return Py_None;
  }
  for (counter = 0; counter < num_cols; ++counter) {
    if (!row_data[counter]) {
      Py_INCREF(Py_None);
      PyList_SetItem(my_list, counter, Py_None);
    } else {
      // FIXME: lame, python doesn't support building values from uint32_t's
      PyList_SetItem(my_list, counter, Py_BuildValue("l", (long)row_data[counter]));
    }
  }
  return my_list;
}

static PyObject *getRowLengths(PyObject *self, PyObject *args) {
  long sqlrcur;
  uint64_t row;
  if (!PyArg_ParseTuple(args,
#ifdef SUPPORTS_UNSIGNED
	"lK",
#else
	"lL",
#endif
	&sqlrcur, &row))
    return NULL;
  return _get_row_lengths((sqlrcursor *)sqlrcur, row);
}

static PyObject *getRowLengthsDictionary(PyObject *self, PyObject *args) {
  long sqlrcur;
  uint64_t row;
  PyObject *my_dictionary;
  if (!PyArg_ParseTuple(args,
#ifdef SUPPORTS_UNSIGNED
	"lK",
#else
	"lL",
#endif
	&sqlrcur, &row))
    return NULL;
  my_dictionary=PyDict_New();
  for (uint32_t counter=0; counter<((sqlrcursor *)sqlrcur)->colCount(); counter++) {
    Py_BEGIN_ALLOW_THREADS
    PyDict_SetItem(my_dictionary,
        Py_BuildValue("s",((sqlrcursor *)sqlrcur)->getColumnName(counter)),
        // FIXME: lame, python doesn't support building values from uint32_t's
        Py_BuildValue("l",(long)((sqlrcursor *)sqlrcur)->getFieldLength(row,counter)));
    Py_END_ALLOW_THREADS
  }
  return my_dictionary;
}

static PyObject *getRowLengthsRange(PyObject *self, PyObject *args) {
  uint64_t beg_row;
  uint64_t end_row;
  long sqlrcur;
  uint64_t counter;
  PyObject *my_list;
  my_list = PyList_New(0);
  if (!PyArg_ParseTuple(args,
#ifdef SUPPORTS_UNSIGNED
	"lKK",
#else
	"lLL",
#endif
	&sqlrcur, &beg_row, &end_row))
    return NULL;
  uint64_t max_rows=((sqlrcursor *)sqlrcur)->rowCount();
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
  const char *rc;
  uint32_t col;
  if (!PyArg_ParseTuple(args,
#ifdef SUPPORTS_UNSIGNED
	"lI",
#else
	"li",
#endif
	&sqlrcur, &col))
    return NULL;
  rc=((sqlrcursor *)sqlrcur)->getColumnName(col);
  return Py_BuildValue("s", rc);
}

static PyObject *getColumnNames(PyObject *self, PyObject *args) {
  long sqlrcur;
  uint32_t counter;
  uint32_t num_cols;
  const char * const *rc;
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
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *getColumnType(PyObject *self, PyObject *args) {
  long sqlrcur;
  const char *rc="";
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
  uint32_t rc=0;
  PyObject *col;
  if (!PyArg_ParseTuple(args, "lO", &sqlrcur, &col))
    return NULL;
  if (PyString_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getColumnLength(PyString_AsString(col));
  } else if (PyInt_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getColumnLength(PyInt_AsLong(col));
  }
  // FIXME: lame, python doesn't support building values from uint32_t's
  return Py_BuildValue("l", (long)rc);
}

static PyObject *getColumnPrecision(PyObject *self, PyObject *args) {
  long sqlrcur;
  uint32_t rc=0;
  PyObject *col;
  if (!PyArg_ParseTuple(args, "lO", &sqlrcur, &col))
    return NULL;
  if (PyString_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getColumnPrecision(PyString_AsString(col));
  } else if (PyInt_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getColumnPrecision(PyInt_AsLong(col));
  }
  // FIXME: lame, python doesn't support building values from uint32_t's
  return Py_BuildValue("l", (long)rc);
}

static PyObject *getColumnScale(PyObject *self, PyObject *args) {
  long sqlrcur;
  uint32_t rc=0;
  PyObject *col;
  if (!PyArg_ParseTuple(args, "lO", &sqlrcur, &col))
    return NULL;
  if (PyString_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getColumnScale(PyString_AsString(col));
  } else if (PyInt_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getColumnScale(PyInt_AsLong(col));
  }
  // FIXME: lame, python doesn't support building values from uint32_t's
  return Py_BuildValue("l", (long)rc);
}

static PyObject *getColumnIsNullable(PyObject *self, PyObject *args) {
  long sqlrcur;
  bool rc=false;
  PyObject *col;
  if (!PyArg_ParseTuple(args, "lO", &sqlrcur, &col))
    return NULL;
  if (PyString_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getColumnIsNullable(PyString_AsString(col));
  } else if (PyInt_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getColumnIsNullable(PyInt_AsLong(col));
  }
  return Py_BuildValue("h", (short)rc);
}

static PyObject *getColumnIsPrimaryKey(PyObject *self, PyObject *args) {
  long sqlrcur;
  bool rc=false;
  PyObject *col;
  if (!PyArg_ParseTuple(args, "lO", &sqlrcur, &col))
    return NULL;
  if (PyString_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getColumnIsPrimaryKey(PyString_AsString(col));
  } else if (PyInt_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getColumnIsPrimaryKey(PyInt_AsLong(col));
  }
  return Py_BuildValue("h", (short)rc);
}

static PyObject *getColumnIsUnique(PyObject *self, PyObject *args) {
  long sqlrcur;
  bool rc=false;
  PyObject *col;
  if (!PyArg_ParseTuple(args, "lO", &sqlrcur, &col))
    return NULL;
  if (PyString_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getColumnIsUnique(PyString_AsString(col));
  } else if (PyInt_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getColumnIsUnique(PyInt_AsLong(col));
  }
  return Py_BuildValue("h", (short)rc);
}

static PyObject *getColumnIsPartOfKey(PyObject *self, PyObject *args) {
  long sqlrcur;
  bool rc=false;
  PyObject *col;
  if (!PyArg_ParseTuple(args, "lO", &sqlrcur, &col))
    return NULL;
  if (PyString_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getColumnIsPartOfKey(PyString_AsString(col));
  } else if (PyInt_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getColumnIsPartOfKey(PyInt_AsLong(col));
  }
  return Py_BuildValue("h", (short)rc);
}

static PyObject *getColumnIsUnsigned(PyObject *self, PyObject *args) {
  long sqlrcur;
  bool rc=false;
  PyObject *col;
  if (!PyArg_ParseTuple(args, "lO", &sqlrcur, &col))
    return NULL;
  if (PyString_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getColumnIsUnsigned(PyString_AsString(col));
  } else if (PyInt_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getColumnIsUnsigned(PyInt_AsLong(col));
  }
  return Py_BuildValue("h", (short)rc);
}

static PyObject *getColumnIsZeroFilled(PyObject *self, PyObject *args) {
  long sqlrcur;
  bool rc=false;
  PyObject *col;
  if (!PyArg_ParseTuple(args, "lO", &sqlrcur, &col))
    return NULL;
  if (PyString_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getColumnIsZeroFilled(PyString_AsString(col));
  } else if (PyInt_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getColumnIsZeroFilled(PyInt_AsLong(col));
  }
  return Py_BuildValue("h", (short)rc);
}

static PyObject *getColumnIsBinary(PyObject *self, PyObject *args) {
  long sqlrcur;
  bool rc=false;
  PyObject *col;
  if (!PyArg_ParseTuple(args, "lO", &sqlrcur, &col))
    return NULL;
  if (PyString_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getColumnIsBinary(PyString_AsString(col));
  } else if (PyInt_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getColumnIsBinary(PyInt_AsLong(col));
  }
  return Py_BuildValue("h", (short)rc);
}

static PyObject *getColumnIsAutoIncrement(PyObject *self, PyObject *args) {
  long sqlrcur;
  bool rc=false;
  PyObject *col;
  if (!PyArg_ParseTuple(args, "lO", &sqlrcur, &col))
    return NULL;
  if (PyString_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getColumnIsAutoIncrement(PyString_AsString(col));
  } else if (PyInt_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getColumnIsAutoIncrement(PyInt_AsLong(col));
  }
  return Py_BuildValue("h", (short)rc);
}

static PyObject *getLongest(PyObject *self, PyObject *args) {
  long sqlrcur;
  uint32_t rc=false;
  PyObject *col;
  if (!PyArg_ParseTuple(args, "lO", &sqlrcur, &col))
    return NULL;
  if (PyString_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getLongest(PyString_AsString(col));
  } else if (PyInt_Check(col)) {
    rc=((sqlrcursor *)sqlrcur)->getLongest(PyInt_AsLong(col));
  }
  // FIXME: lame, python doesn't support building values from uint32_t's
  return Py_BuildValue("l", (long)rc);
}

static PyObject *getResultSetId(PyObject *self, PyObject *args) {
  long sqlrcur;
  uint16_t rc;
  if (!PyArg_ParseTuple(args, "l", &sqlrcur))
    return NULL;
  rc=((sqlrcursor *)sqlrcur)->getResultSetId();
  // FIXME: lame, python doesn't support building values from uint16_t's
  return Py_BuildValue("h", (short)rc);
}

static PyObject *suspendResultSet(PyObject *self, PyObject *args) {
  long sqlrcur;
  if (!PyArg_ParseTuple(args, "l", &sqlrcur))
    return NULL;
  Py_BEGIN_ALLOW_THREADS
  ((sqlrcursor *)sqlrcur)->suspendResultSet();
  Py_END_ALLOW_THREADS
  return Py_BuildValue("h", 0);
}

static PyObject *resumeResultSet(PyObject *self, PyObject *args) {
  long sqlrcur;
  bool rc;
  uint16_t id;
  if (!PyArg_ParseTuple(args,
#ifdef SUPPORTS_UNSIGNED
	"lH",
#else
	"lh",
#endif
	&sqlrcur, &id))
    return NULL;
  Py_BEGIN_ALLOW_THREADS
  rc=((sqlrcursor *)sqlrcur)->resumeResultSet(id);
  Py_END_ALLOW_THREADS
  return Py_BuildValue("h", (short)rc);
}

static PyObject *resumeCachedResultSet(PyObject *self, PyObject *args) {
  long sqlrcur;
  bool rc;
  uint16_t id;
  char *filename;
  if (!PyArg_ParseTuple(args,
#ifdef SUPPORTS_UNSIGNED
	"lHs",
#else
	"lhs",
#endif
	&sqlrcur, &id, &filename))
    return NULL;
  Py_BEGIN_ALLOW_THREADS
  rc=((sqlrcursor *)sqlrcur)->resumeCachedResultSet(id,filename);
  Py_END_ALLOW_THREADS
  return Py_BuildValue("h", (short)rc);
}

static PyObject *outputBindCursorIdIsValid(PyObject *self, PyObject *args) {
  long sqlrcur;
  bool rc;
  char *variable;
  if (!PyArg_ParseTuple(args, "ls", &sqlrcur, &variable))
    return NULL;
  Py_BEGIN_ALLOW_THREADS
  rc=((sqlrcursor *)sqlrcur)->outputBindCursorIdIsValid(variable);
  Py_END_ALLOW_THREADS
  return Py_BuildValue("h", (short)rc);
}

static PyObject *getOutputBindCursorId(PyObject *self, PyObject *args) {
  long sqlrcur;
  uint16_t rc;
  char *variable;
  if (!PyArg_ParseTuple(args, "ls", &sqlrcur, &variable))
    return NULL;
  Py_BEGIN_ALLOW_THREADS
  rc=((sqlrcursor *)sqlrcur)->getOutputBindCursorId(variable);
  Py_END_ALLOW_THREADS
  // FIXME: lame, python doesn't support building values from uint16_t's
  return Py_BuildValue("h", (short)rc);
}

static PyObject *attachToBindCursor(PyObject *self, PyObject *args) {
  long sqlrcur;
  uint16_t bindcursorid;
  if (!PyArg_ParseTuple(args,
#ifdef SUPPORTS_UNSIGNED
	"lH",
#else
	"lh",
#endif
	&sqlrcur, &bindcursorid))
    return NULL;
  Py_BEGIN_ALLOW_THREADS
  ((sqlrcursor *)sqlrcur)->attachToBindCursor(bindcursorid);
  Py_END_ALLOW_THREADS
  return Py_BuildValue("h", 0);
}

static PyMethodDef SQLRMethods[] = {
  {"sqlrcon_alloc",  sqlrcon_alloc, METH_VARARGS},
  {"sqlrcon_free", sqlrcon_free, METH_VARARGS},
  {"setTimeout", setTimeout, METH_VARARGS},
  {"endSession", endSession, METH_VARARGS},
  {"suspendSession", suspendSession, METH_VARARGS},
  {"getConnectionPort", getConnectionPort, METH_VARARGS},
  {"getConnectionSocket", getConnectionSocket, METH_VARARGS},
  {"resumeSession", resumeSession, METH_VARARGS},
  {"ping", ping, METH_VARARGS},
  {"identify", identify, METH_VARARGS},
  {"dbVersion", dbVersion, METH_VARARGS},
  {"serverVersion", serverVersion, METH_VARARGS},
  {"clientVersion", clientVersion, METH_VARARGS},
  {"bindFormat", bindFormat, METH_VARARGS},
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
  {"defineOutputBindString", defineOutputBindString, METH_VARARGS},
  {"defineOutputBindInteger", defineOutputBindInteger, METH_VARARGS},
  {"defineOutputBindDouble", defineOutputBindDouble, METH_VARARGS},
  {"defineOutputBindBlob", defineOutputBindBlob, METH_VARARGS},
  {"defineOutputBindClob", defineOutputBindClob, METH_VARARGS},
  {"defineOutputBindCursor", defineOutputBindCursor, METH_VARARGS},
  {"validateBinds", validateBinds, METH_VARARGS},
  {"validBind", validBind, METH_VARARGS},
  {"executeQuery", executeQuery, METH_VARARGS},
  {"fetchFromBindCursor", fetchFromBindCursor, METH_VARARGS},
  {"getOutputBindString", getOutputBindString, METH_VARARGS},
  {"getOutputBindBlob", getOutputBindBlob, METH_VARARGS},
  {"getOutputBindClob", getOutputBindClob, METH_VARARGS},
  {"getOutputBindInteger", getOutputBindInteger, METH_VARARGS},
  {"getOutputBindDouble", getOutputBindDouble, METH_VARARGS},
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
  {"getFieldAsInteger", getFieldAsInteger, METH_VARARGS},
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
  {"outputBindCursorIdIsValid", outputBindCursorIdIsValid, METH_VARARGS},
  {"getOutputBindCursorId", getOutputBindCursorId, METH_VARARGS},
  {"attachToBindCursor", attachToBindCursor, METH_VARARGS},
  {NULL,      NULL}        /* Sentinel */
};

void
initCSQLRelay()
{
  (void) Py_InitModule("SQLRelay.CSQLRelay", SQLRMethods);

  decimalmodule=PyImport_ImportModule("decimal");
  if (decimalmodule) {
    decimal=PyObject_GetAttrString(decimalmodule,"Decimal");
    if (!decimal) {
      PyErr_Clear();
    }
  } else {
    PyErr_Clear();
  }
}

}
