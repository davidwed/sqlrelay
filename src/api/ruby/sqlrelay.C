// Copyright (c) 2001  David Muse
// See the file COPYING for more information.

#include <ruby.h>
#include "../c++/include/sqlrelay/sqlrclient.h"

#include "rubyincludes.h"

extern "C" {

// sqlrconnection methods
static void sqlrcon_free(void *sqlrcon) {
	delete (sqlrconnection *)sqlrcon;
}

static VALUE sqlrcon_new(VALUE self, VALUE host, VALUE port, VALUE socket,
				VALUE user, VALUE password, 
				VALUE tries, VALUE retrytime) {
	char	*socketstr;
	if (socket==Qnil) {
		socketstr="";
	} else {
		socketstr=STR2CSTR(socket);
	}
	sqlrconnection	*sqlrcon=new sqlrconnection(STR2CSTR(host),
							NUM2INT(port),
							socketstr,
							STR2CSTR(user),
							STR2CSTR(password),
							NUM2INT(tries),
							NUM2INT(retrytime));
	sqlrcon->copyReferences();
	return Data_Wrap_Struct(self,0,sqlrcon_free,(void *)sqlrcon);
}

static VALUE sqlrcon_endSession(VALUE self) {
	sqlrconnection	*sqlrcon;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	return INT2NUM(sqlrcon->endSession());
}

static VALUE sqlrcon_suspendSession(VALUE self) {
	sqlrconnection	*sqlrcon;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	return INT2NUM(sqlrcon->suspendSession());
}

static VALUE sqlrcon_getConnectionPort(VALUE self) {
	sqlrconnection	*sqlrcon;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	return INT2NUM(sqlrcon->getConnectionPort());
}

static VALUE sqlrcon_getConnectionSocket(VALUE self) {
	sqlrconnection	*sqlrcon;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	char	*result=sqlrcon->getConnectionSocket();
	if (result) {
		return rb_str_new2(result);
	} else {
		return Qnil;
	}
}

static VALUE sqlrcon_resumeSession(VALUE self, VALUE port, VALUE socket) {
	sqlrconnection	*sqlrcon;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	return INT2NUM(sqlrcon->resumeSession(NUM2INT(port), 
							STR2CSTR(socket)));
}

static VALUE sqlrcon_ping(VALUE self) {
	sqlrconnection	*sqlrcon;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	return INT2NUM(sqlrcon->ping());
}

static VALUE sqlrcon_identify(VALUE self) {
	sqlrconnection	*sqlrcon;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	char	*result=sqlrcon->identify();
	if (result) {
		return rb_str_new2(result);
	} else {
		return Qnil;
	}
}

static VALUE sqlrcon_autoCommitOn(VALUE self) {
	sqlrconnection	*sqlrcon;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	return INT2NUM(sqlrcon->autoCommitOn());
}

static VALUE sqlrcon_autoCommitOff(VALUE self) {
	sqlrconnection	*sqlrcon;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	return INT2NUM(sqlrcon->autoCommitOff());
}

static VALUE sqlrcon_commit(VALUE self) {
	sqlrconnection	*sqlrcon;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	return INT2NUM(sqlrcon->commit());
}

static VALUE sqlrcon_rollback(VALUE self) {
	sqlrconnection	*sqlrcon;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	return INT2NUM(sqlrcon->rollback());
}

static VALUE sqlrcon_debugOn(VALUE self) {
	sqlrconnection	*sqlrcon;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	sqlrcon->debugOn();
	return Qnil;
}

static VALUE sqlrcon_debugOff(VALUE self) {
	sqlrconnection	*sqlrcon;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	sqlrcon->debugOff();
	return Qnil;
}

static VALUE sqlrcon_getDebug(VALUE self) {
	sqlrconnection	*sqlrcon;
	Data_Get_Struct(self,sqlrconnection,sqlrcon);
	return INT2NUM(sqlrcon->getDebug());
}



VALUE csqlrconnection;

void Init_SQLRConnection() {
	csqlrconnection=rb_define_class("SQLRConnection", rb_cObject);
	rb_define_singleton_method(csqlrconnection,"new",
				(CAST)sqlrcon_new,7);
	rb_define_method(csqlrconnection,"endSession",
				(CAST)sqlrcon_endSession,0);
	rb_define_method(csqlrconnection,"suspendSession",
				(CAST)sqlrcon_suspendSession,0);
	rb_define_method(csqlrconnection,"getConnectionPort",
				(CAST)sqlrcon_getConnectionPort,0);
	rb_define_method(csqlrconnection,"getConnectionSocket",
				(CAST)sqlrcon_getConnectionSocket,0);
	rb_define_method(csqlrconnection,"resumeSession",
				(CAST)sqlrcon_resumeSession,2);
	rb_define_method(csqlrconnection,"ping",
				(CAST)sqlrcon_ping,0);
	rb_define_method(csqlrconnection,"identify",
				(CAST)sqlrcon_identify,0);
	rb_define_method(csqlrconnection,"autoCommitOn",
				(CAST)sqlrcon_autoCommitOn,0);
	rb_define_method(csqlrconnection,"autoCommitOff",
				(CAST)sqlrcon_autoCommitOff,0);
	rb_define_method(csqlrconnection,"commit",
				(CAST)sqlrcon_commit,0);
	rb_define_method(csqlrconnection,"rollback",
				(CAST)sqlrcon_rollback,0);
	rb_define_method(csqlrconnection,"debugOn",
				(CAST)sqlrcon_debugOn,0);
	rb_define_method(csqlrconnection,"debugOff",
				(CAST)sqlrcon_debugOff,0);
	rb_define_method(csqlrconnection,"getDebug",
				(CAST)sqlrcon_getDebug,0);
}



// sqlrcursor methods
VALUE csqlrcursor;

static void sqlrcur_free(void *sqlrcur) {
	delete (sqlrcursor *)sqlrcur;
}

static VALUE sqlrcur_new(VALUE self, VALUE connection) {
	sqlrconnection	*sqlrcon;
	Data_Get_Struct(connection,sqlrconnection,sqlrcon);
	sqlrcursor	*sqlrcur=new sqlrcursor(sqlrcon);
	sqlrcon->copyReferences();
	return Data_Wrap_Struct(self,0,sqlrcur_free,(void *)sqlrcur);
}

static VALUE sqlrcur_setResultSetBufferSize(VALUE self, VALUE rows) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	sqlrcur->setResultSetBufferSize(NUM2INT(rows));
	return Qnil;
}

static VALUE sqlrcur_getResultSetBufferSize(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	return INT2NUM(sqlrcur->getResultSetBufferSize());
}

static VALUE sqlrcur_dontGetColumnInfo(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	sqlrcur->dontGetColumnInfo();
	return Qnil;
}

static VALUE sqlrcur_getColumnInfo(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	sqlrcur->getColumnInfo();
	return Qnil;
}

static VALUE sqlrcur_mixedCaseColumnNames(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	sqlrcur->mixedCaseColumnNames();
	return Qnil;
}

static VALUE sqlrcur_upperCaseColumnNames(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	sqlrcur->upperCaseColumnNames();
	return Qnil;
}

static VALUE sqlrcur_lowerCaseColumnNames(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	sqlrcur->lowerCaseColumnNames();
	return Qnil;
}

static VALUE sqlrcur_cacheToFile(VALUE self, VALUE filename) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	sqlrcur->cacheToFile(STR2CSTR(filename));
	return Qnil;
}

static VALUE sqlrcur_setCacheTtl(VALUE self, VALUE ttl) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	sqlrcur->setCacheTtl(NUM2INT(ttl));
	return Qnil;
}

static VALUE sqlrcur_getCacheFileName(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	char	*result=sqlrcur->getCacheFileName();
	if (result) {
		return rb_str_new2(result);
	} else {
		return Qnil;
	}
}

static VALUE sqlrcur_cacheOff(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	sqlrcur->cacheOff();
	return Qnil;
}

static VALUE sqlrcur_sendQuery(VALUE self, VALUE query) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	return INT2NUM(sqlrcur->sendQuery(STR2CSTR(query)));
}

static VALUE sqlrcur_sendQueryWithLength(VALUE self,
					VALUE query, VALUE length) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	return INT2NUM(sqlrcur->sendQuery(STR2CSTR(query),NUM2INT(length)));
}

static VALUE sqlrcur_sendFileQuery(VALUE self, VALUE path, VALUE filename) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	return INT2NUM(sqlrcur->sendFileQuery(STR2CSTR(path),
						STR2CSTR(filename))); 
}

static VALUE sqlrcur_prepareQuery(VALUE self, VALUE query) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	sqlrcur->prepareQuery(STR2CSTR(query));
	return Qnil;
}

static VALUE sqlrcur_prepareQueryWithLength(VALUE self,
					VALUE query, VALUE length) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	sqlrcur->prepareQuery(STR2CSTR(query),NUM2INT(length));
	return Qnil;
}

static VALUE sqlrcur_prepareFileQuery(VALUE self, VALUE path, VALUE filename) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	return INT2NUM(sqlrcur->prepareFileQuery(STR2CSTR(path),
						STR2CSTR(filename)));
}

static VALUE sqlrcur_clearBinds(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	sqlrcur->clearBinds();
	return Qnil;
}

static VALUE sqlrcur_substitution(int argc, VALUE *argv, VALUE self) {
	sqlrcursor	*sqlrcur;
	VALUE	variable;
	VALUE	value;
	VALUE	precision;
	VALUE	scale;
	rb_scan_args(argc,argv,"22",&variable,&value,&precision,&scale);
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	if (rb_obj_is_instance_of(value,rb_cString)==Qtrue) {
		sqlrcur->substitution(STR2CSTR(variable),STR2CSTR(value));
	} else if (rb_obj_is_instance_of(value,rb_cBignum)==Qtrue ||
			rb_obj_is_instance_of(value,rb_cFixnum)==Qtrue ||
			rb_obj_is_instance_of(value,rb_cInteger)==Qtrue ||
			rb_obj_is_instance_of(value,rb_cNumeric)==Qtrue) {
		sqlrcur->substitution(STR2CSTR(variable),NUM2INT(value));
	} else if (rb_obj_is_instance_of(value,rb_cFloat)==Qtrue) {
		sqlrcur->substitution(STR2CSTR(variable),NUM2DBL(value), 
					(unsigned short)NUM2INT(precision),
					(unsigned short)NUM2INT(scale));
	} else if (rb_obj_is_instance_of(value,rb_cNilClass)==Qtrue) {
		sqlrcur->substitution(STR2CSTR(variable),(char *)NULL);
	}
	return Qnil;
}

static VALUE sqlrcur_inputBind(int argc, VALUE *argv, VALUE self) {
	sqlrcursor	*sqlrcur;
	VALUE	variable;
	VALUE	value;
	VALUE	precision;
	VALUE	scale;
	rb_scan_args(argc,argv,"22",&variable,&value,&precision,&scale);
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	if (rb_obj_is_instance_of(value,rb_cString)==Qtrue) {
		sqlrcur->inputBind(STR2CSTR(variable),STR2CSTR(value));
	} else if (rb_obj_is_instance_of(value,rb_cBignum)==Qtrue ||
			rb_obj_is_instance_of(value,rb_cFixnum)==Qtrue ||
			rb_obj_is_instance_of(value,rb_cInteger)==Qtrue ||
			rb_obj_is_instance_of(value,rb_cNumeric)==Qtrue) {
		sqlrcur->inputBind(STR2CSTR(variable),NUM2INT(value));
	} else if (rb_obj_is_instance_of(value,rb_cFloat)==Qtrue) {
		sqlrcur->inputBind(STR2CSTR(variable),NUM2DBL(value), 
					(unsigned short)NUM2INT(precision),
					(unsigned short)NUM2INT(scale));
	} else if (rb_obj_is_instance_of(value,rb_cNilClass)==Qtrue) {
		sqlrcur->inputBind(STR2CSTR(variable),(char *)NULL);
	}
	return Qnil;
}

static VALUE sqlrcur_inputBindBlob(VALUE self, VALUE variable,
					VALUE value, VALUE size) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	if (value==Qnil) {
		sqlrcur->inputBindBlob(STR2CSTR(variable),NULL,NUM2INT(size));
	} else {
		sqlrcur->inputBindBlob(STR2CSTR(variable),
				STR2CSTR(value),NUM2INT(size));
	}
	return Qnil;
}

static VALUE sqlrcur_inputBindClob(VALUE self, VALUE variable,
					VALUE value, VALUE size) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	if (value==Qnil) {
		sqlrcur->inputBindClob(STR2CSTR(variable),NULL,NUM2INT(size));
	} else {
		sqlrcur->inputBindClob(STR2CSTR(variable),
				STR2CSTR(value),NUM2INT(size));
	}
	return Qnil;
}

static VALUE sqlrcur_defineOutputBind(VALUE self, VALUE variable,
						VALUE bufferlength) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	sqlrcur->defineOutputBind(STR2CSTR(variable),NUM2INT(bufferlength));
	return Qnil;
}

static VALUE sqlrcur_defineOutputBindBlob(VALUE self, VALUE variable) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	sqlrcur->defineOutputBindBlob(STR2CSTR(variable));
	return Qnil;
}

static VALUE sqlrcur_defineOutputBindClob(VALUE self, VALUE variable) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	sqlrcur->defineOutputBindClob(STR2CSTR(variable));
	return Qnil;
}

static VALUE sqlrcur_defineOutputBindCursor(VALUE self, VALUE variable) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	sqlrcur->defineOutputBindCursor(STR2CSTR(variable));
	return Qnil;
}

static VALUE sqlrcur_substitutions(int argc, VALUE *argv, VALUE self) {
	sqlrcursor	*sqlrcur;
	VALUE	variables;
	VALUE	values;
	VALUE	precisions;
	VALUE	scales;
	int	argcount=rb_scan_args(argc,argv,"22",
					&variables,&values,&precisions,&scales);
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	if (variables==Qnil || values==Qnil) {
		return Qnil;
	}
	VALUE	variable;
	VALUE	value;
	VALUE	precision;
	VALUE	scale;
	for (;;) {
		variable=rb_ary_shift(variables);
		if (variable==Qnil) {
			break;
		}
		value=rb_ary_shift(values);
		if (argcount==4) {
			precision=rb_ary_shift(precisions);
			scale=rb_ary_shift(scales);
		}
		if (rb_obj_is_instance_of(value,rb_cString)==Qtrue) {
			sqlrcur->substitution(STR2CSTR(variable),
						STR2CSTR(value));
		} else if (rb_obj_is_instance_of(value,rb_cBignum)==Qtrue ||
			rb_obj_is_instance_of(value,rb_cFixnum)==Qtrue ||
			rb_obj_is_instance_of(value,rb_cInteger)==Qtrue ||
			rb_obj_is_instance_of(value,rb_cNumeric)==Qtrue) {
			sqlrcur->substitution(STR2CSTR(variable),
						NUM2INT(value));
		} else if (rb_obj_is_instance_of(value,rb_cFloat)==Qtrue) {
			sqlrcur->substitution(STR2CSTR(variable),
					NUM2DBL(value), 
					(unsigned short)NUM2INT(precision),
					(unsigned short)NUM2INT(scale));
		} else if (rb_obj_is_instance_of(value,rb_cNilClass)==Qtrue) {
			sqlrcur->substitution(STR2CSTR(variable),(char *)NULL);
		}
	}
	return Qnil;
}

static VALUE sqlrcur_inputBinds(int argc, VALUE *argv, VALUE self) {
	sqlrcursor	*sqlrcur;
	VALUE	variables;
	VALUE	values;
	VALUE	precisions;
	VALUE	scales;
	int	argcount=rb_scan_args(argc,argv,"22",
				&variables,&values,&precisions,&scales);
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	if (variables==Qnil || values==Qnil) {
		return Qnil;
	}
	VALUE	variable;
	VALUE	value;
	VALUE	precision;
	VALUE	scale;
	for (;;) {
		variable=rb_ary_shift(variables);
		if (variable==Qnil) {
			break;
		}
		value=rb_ary_shift(values);
		if (argcount==4) {
			precision=rb_ary_shift(precisions);
			scale=rb_ary_shift(scales);
		}
		if (rb_obj_is_instance_of(value,rb_cString)==Qtrue) {
			sqlrcur->inputBind(STR2CSTR(variable),STR2CSTR(value));
		} else if (rb_obj_is_instance_of(value,rb_cBignum)==Qtrue ||
			rb_obj_is_instance_of(value,rb_cFixnum)==Qtrue ||
			rb_obj_is_instance_of(value,rb_cInteger)==Qtrue ||
			rb_obj_is_instance_of(value,rb_cNumeric)==Qtrue) {
			sqlrcur->inputBind(STR2CSTR(variable),NUM2INT(value));
		} else if (rb_obj_is_instance_of(value,rb_cFloat)==Qtrue) {
			sqlrcur->inputBind(STR2CSTR(variable),NUM2DBL(value), 
					(unsigned short)NUM2INT(precision),
					(unsigned short)NUM2INT(scale));
		} else if (rb_obj_is_instance_of(value,rb_cNilClass)==Qtrue) {
			sqlrcur->inputBind(STR2CSTR(variable),(char *)NULL);
		}
	}
	return Qnil;
}

static VALUE sqlrcur_validateBinds(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	sqlrcur->validateBinds();
	return Qnil;
}

static VALUE sqlrcur_executeQuery(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	return INT2NUM(sqlrcur->executeQuery());
}

static VALUE sqlrcur_fetchFromBindCursor(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	return INT2NUM(sqlrcur->fetchFromBindCursor());
}

static VALUE sqlrcur_getOutputBind(VALUE self, VALUE variable) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	char	*result=sqlrcur->getOutputBind(STR2CSTR(variable));
	if (result) {
		return rb_str_new2(result);
	} else {
		return Qnil;
	}
}

static VALUE sqlrcur_getOutputBindLength(VALUE self, VALUE variable) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	return INT2NUM(sqlrcur->getOutputBindLength(STR2CSTR(variable)));
}

static VALUE sqlrcur_getOutputBindCursor(VALUE self, VALUE variable) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	sqlrcursor	*returnsqlrcur=sqlrcur->getOutputBindCursor(
							STR2CSTR(variable));
	returnsqlrcur->copyReferences();
	return Data_Wrap_Struct(csqlrcursor,0,sqlrcon_free,
					(void *)returnsqlrcur);
}

static VALUE sqlrcur_openCachedResultSet(VALUE self, VALUE filename) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	return INT2NUM(sqlrcur->openCachedResultSet(STR2CSTR(filename)));
}

static VALUE sqlrcur_colCount(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	return INT2NUM(sqlrcur->colCount());
}

static VALUE sqlrcur_rowCount(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	return INT2NUM(sqlrcur->rowCount());
}

static VALUE sqlrcur_totalRows(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	return INT2NUM(sqlrcur->totalRows());
}

static VALUE sqlrcur_affectedRows(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	return INT2NUM(sqlrcur->affectedRows());
}

static VALUE sqlrcur_firstRowIndex(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	return INT2NUM(sqlrcur->firstRowIndex());
}

static VALUE sqlrcur_endOfResultSet(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	return INT2NUM(sqlrcur->endOfResultSet());
}

static VALUE sqlrcur_errorMessage(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	char	*result=sqlrcur->errorMessage();
	if (result) {
		return rb_str_new2(result);
	} else {
		return Qnil;
	}
}

static VALUE sqlrcur_getNullsAsEmptyStrings(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	sqlrcur->getNullsAsEmptyStrings();
	return Qnil;
}

static VALUE sqlrcur_getNullsAsNils(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	sqlrcur->getNullsAsNulls();
	return Qnil;
}

static VALUE sqlrcur_getField(VALUE self, VALUE row, VALUE col) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	char	*result;
	if (rb_obj_is_instance_of(col,rb_cString)==Qtrue) {
		result=sqlrcur->getField(NUM2INT(row),STR2CSTR(col));
	} else {
		result=sqlrcur->getField(NUM2INT(row),NUM2INT(col));
	}
	if (result) {
		return rb_str_new2(result);
	} else {
		return Qnil;
	}
}

static VALUE sqlrcur_getFieldLength(VALUE self, VALUE row, VALUE col) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	char	*result;
	if (rb_obj_is_instance_of(col,rb_cString)==Qtrue) {
		return INT2NUM((int)sqlrcur->getFieldLength(NUM2INT(row),
								STR2CSTR(col)));
	} else {
		return INT2NUM((int)sqlrcur->getFieldLength(NUM2INT(row),
								NUM2INT(col)));
	}
}

static VALUE sqlrcur_getRow(VALUE self, VALUE row) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	char	**fields=sqlrcur->getRow(NUM2INT(row));
	VALUE	fieldary=rb_ary_new2(sqlrcur->colCount());
	for (int i=0; i<sqlrcur->colCount(); i++) {
		if (fields[i]) {
			rb_ary_store(fieldary,i,rb_str_new2(fields[i]));
		} else {
			rb_ary_store(fieldary,i,Qnil);
		}
	}
	return fieldary;
}

static VALUE sqlrcur_getRowHash(VALUE self, VALUE row) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	char	**fields=sqlrcur->getRow(NUM2INT(row));
	VALUE	fieldhash=rb_hash_new();
	for (int i=0; i<sqlrcur->colCount(); i++) {
		if (fields[i]) {
			rb_hash_aset(fieldhash,
					rb_str_new2(sqlrcur->getColumnName(i)),
					rb_str_new2(fields[i]));
		} else {
			rb_hash_aset(fieldhash,
					rb_str_new2(sqlrcur->getColumnName(i)),
					Qnil);
		}
	}
	return fieldhash;
}

static VALUE sqlrcur_getRowLengths(VALUE self, VALUE row) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	long	*lengths=sqlrcur->getRowLengths(NUM2INT(row));
	if (!lengths) {
		return Qnil;
	}
	VALUE	lengthary=rb_ary_new2(sqlrcur->colCount());
	for (int i=0; i<sqlrcur->colCount(); i++) {
		rb_ary_store(lengthary,i,INT2NUM(lengths[i]));
	}
	return lengthary;
}

static VALUE sqlrcur_getRowLengthsHash(VALUE self, VALUE row) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	long	*lengths=sqlrcur->getRowLengths(NUM2INT(row));
	VALUE	lengthhash=rb_hash_new();
	for (int i=0; i<sqlrcur->colCount(); i++) {
		rb_hash_aset(lengthhash,
				rb_str_new2(sqlrcur->getColumnName(i)),
				INT2NUM(lengths[i]));
	}
	return lengthhash;
}

static VALUE sqlrcur_getColumnNames(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	char	**names=sqlrcur->getColumnNames();
	if (!names) {
		return Qnil;
	}
	VALUE	nameary=rb_ary_new2(sqlrcur->colCount());
	for (int i=0; i<sqlrcur->colCount(); i++) {
		if (names[i]) {
			rb_ary_store(nameary,i,rb_str_new2(names[i]));
		} else {
			rb_ary_store(nameary,i,Qnil);
		}
	}
	return nameary;
}

static VALUE sqlrcur_getColumnName(VALUE self, VALUE col) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	char	*result=sqlrcur->getColumnName(NUM2INT(col));
	if (result) {
		return rb_str_new2(result);
	} else {
		return Qnil;
	}
}

static VALUE sqlrcur_getColumnType(VALUE self, VALUE col) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	char		*result;
	if (rb_obj_is_instance_of(col,rb_cString)==Qtrue) {
		result=sqlrcur->getColumnType(STR2CSTR(col));
	} else {
		result=sqlrcur->getColumnType(NUM2INT(col));
	}
	if (result) {
		return rb_str_new2(result);
	} else {
		return Qnil;
	}
}

static VALUE sqlrcur_getColumnLength(VALUE self, VALUE col) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	if (rb_obj_is_instance_of(col,rb_cString)==Qtrue) {
		return INT2NUM(sqlrcur->getColumnLength(STR2CSTR(col)));
	} else {
		return INT2NUM(sqlrcur->getColumnLength(NUM2INT(col)));
	}
}

static VALUE sqlrcur_getLongest(VALUE self, VALUE col) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	if (rb_obj_is_instance_of(col,rb_cString)==Qtrue) {
		return INT2NUM(sqlrcur->getLongest(STR2CSTR(col)));
	} else {
		return INT2NUM(sqlrcur->getLongest(NUM2INT(col)));
	}
}

static VALUE sqlrcur_getResultSetId(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	return INT2NUM(sqlrcur->getResultSetId());
}

static VALUE sqlrcur_suspendResultSet(VALUE self) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	sqlrcur->suspendResultSet();
	return Qnil;
}

static VALUE sqlrcur_resumeResultSet(VALUE self, VALUE id) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	return INT2NUM(sqlrcur->resumeResultSet(NUM2INT(id)));
}

static VALUE sqlrcur_resumeCachedResultSet(VALUE self, 
						VALUE id, VALUE filename) {
	sqlrcursor	*sqlrcur;
	Data_Get_Struct(self,sqlrcursor,sqlrcur);
	return INT2NUM(sqlrcur->resumeCachedResultSet(NUM2INT(id),
							STR2CSTR(filename)));
}

void Init_SQLRCursor() {
	csqlrcursor=rb_define_class("SQLRCursor", rb_cObject);
	rb_define_singleton_method(csqlrcursor,"new",
				(CAST)sqlrcur_new,1);
	rb_define_method(csqlrcursor,"setResultSetBufferSize",
				(CAST)sqlrcur_setResultSetBufferSize,1);
	rb_define_method(csqlrcursor,"getResultSetBufferSize",
				(CAST)sqlrcur_getResultSetBufferSize,0);
	rb_define_method(csqlrcursor,"dontGetColumnInfo",
				(CAST)sqlrcur_dontGetColumnInfo,0);
	rb_define_method(csqlrcursor,"getColumnInfo",
				(CAST)sqlrcur_getColumnInfo,0);
	rb_define_method(csqlrcursor,"mixedCaseColumnNames",
				(CAST)sqlrcur_mixedCaseColumnNames,0);
	rb_define_method(csqlrcursor,"upperCaseColumnNames",
				(CAST)sqlrcur_upperCaseColumnNames,0);
	rb_define_method(csqlrcursor,"lowerCaseColumnNames",
				(CAST)sqlrcur_lowerCaseColumnNames,0);
	rb_define_method(csqlrcursor,"cacheToFile",
				(CAST)sqlrcur_cacheToFile,1);
	rb_define_method(csqlrcursor,"setCacheTtl",
				(CAST)sqlrcur_setCacheTtl,1);
	rb_define_method(csqlrcursor,"getCacheFileName",
				(CAST)sqlrcur_getCacheFileName,0);
	rb_define_method(csqlrcursor,"cacheOff",
				(CAST)sqlrcur_cacheOff,0);
	rb_define_method(csqlrcursor,"sendQuery",
				(CAST)sqlrcur_sendQuery,1);
	rb_define_method(csqlrcursor,"sendQueryWithLength",
				(CAST)sqlrcur_sendQueryWithLength,2);
	rb_define_method(csqlrcursor,"sendFileQuery",
				(CAST)sqlrcur_sendFileQuery,2);
	rb_define_method(csqlrcursor,"prepareQuery",
				(CAST)sqlrcur_prepareQuery,1);
	rb_define_method(csqlrcursor,"prepareQueryWithLength",
				(CAST)sqlrcur_prepareQueryWithLength,2);
	rb_define_method(csqlrcursor,"prepareFileQuery",
				(CAST)sqlrcur_prepareFileQuery,2);
	rb_define_method(csqlrcursor,"clearBinds",
				(CAST)sqlrcur_clearBinds,0);
	rb_define_method(csqlrcursor,"substitution",
				(CAST)sqlrcur_substitution,-1);
	rb_define_method(csqlrcursor,"inputBind",
				(CAST)sqlrcur_inputBind,-1);
	rb_define_method(csqlrcursor,"inputBindBlob",
				(CAST)sqlrcur_inputBindBlob,3);
	rb_define_method(csqlrcursor,"inputBindClob",
				(CAST)sqlrcur_inputBindClob,3);
	rb_define_method(csqlrcursor,"defineOutputBind",
				(CAST)sqlrcur_defineOutputBind,2);
	rb_define_method(csqlrcursor,"defineOutputBindBlob",
				(CAST)sqlrcur_defineOutputBindBlob,1);
	rb_define_method(csqlrcursor,"defineOutputBindClob",
				(CAST)sqlrcur_defineOutputBindClob,1);
	rb_define_method(csqlrcursor,"defineOutputBindCursor",
				(CAST)sqlrcur_defineOutputBindCursor,1);
	rb_define_method(csqlrcursor,"substitutions",
				(CAST)sqlrcur_substitutions,-1);
	rb_define_method(csqlrcursor,"inputBinds",
				(CAST)sqlrcur_inputBinds,-1);
	rb_define_method(csqlrcursor,"validateBinds",
				(CAST)sqlrcur_validateBinds,0);
	rb_define_method(csqlrcursor,"executeQuery",
				(CAST)sqlrcur_executeQuery,0);
	rb_define_method(csqlrcursor,"fetchFromBindCursor",
				(CAST)sqlrcur_fetchFromBindCursor,0);
	rb_define_method(csqlrcursor,"getOutputBind",
				(CAST)sqlrcur_getOutputBind,1);
	rb_define_method(csqlrcursor,"getOutputBindLength",
				(CAST)sqlrcur_getOutputBindLength,1);
	rb_define_method(csqlrcursor,"getOutputBindCursor",
				(CAST)sqlrcur_getOutputBindCursor,1);
	rb_define_method(csqlrcursor,"openCachedResultSet",
				(CAST)sqlrcur_openCachedResultSet,1);
	rb_define_method(csqlrcursor,"colCount",
				(CAST)sqlrcur_colCount,0);
	rb_define_method(csqlrcursor,"rowCount",
				(CAST)sqlrcur_rowCount,0);
	rb_define_method(csqlrcursor,"totalRows",
				(CAST)sqlrcur_totalRows,0);
	rb_define_method(csqlrcursor,"affectedRows",
				(CAST)sqlrcur_affectedRows,0);
	rb_define_method(csqlrcursor,"firstRowIndex",
				(CAST)sqlrcur_firstRowIndex,0);
	rb_define_method(csqlrcursor,"endOfResultSet",
				(CAST)sqlrcur_endOfResultSet,0);
	rb_define_method(csqlrcursor,"errorMessage",
				(CAST)sqlrcur_errorMessage,0);
	rb_define_method(csqlrcursor,"getNullsAsEmptyStrings",
				(CAST)sqlrcur_getNullsAsEmptyStrings,0);
	rb_define_method(csqlrcursor,"getNullsAsNils",
				(CAST)sqlrcur_getNullsAsNils,0);
	rb_define_method(csqlrcursor,"getField",
				(CAST)sqlrcur_getField,2);
	rb_define_method(csqlrcursor,"getFieldLength",
				(CAST)sqlrcur_getFieldLength,2);
	rb_define_method(csqlrcursor,"getRow",
				(CAST)sqlrcur_getRow,1);
	rb_define_method(csqlrcursor,"getRowHash",
				(CAST)sqlrcur_getRowHash,1);
	rb_define_method(csqlrcursor,"getRowLengths",
				(CAST)sqlrcur_getRowLengths,1);
	rb_define_method(csqlrcursor,"getRowLengthsHash",
				(CAST)sqlrcur_getRowLengthsHash,1);
	rb_define_method(csqlrcursor,"getColumnNames",
				(CAST)sqlrcur_getColumnNames,0);
	rb_define_method(csqlrcursor,"getColumnName",
				(CAST)sqlrcur_getColumnName,1);
	rb_define_method(csqlrcursor,"getColumnType",
				(CAST)sqlrcur_getColumnType,1);
	rb_define_method(csqlrcursor,"getColumnLength",
				(CAST)sqlrcur_getColumnLength,1);
	rb_define_method(csqlrcursor,"getLongest",
				(CAST)sqlrcur_getLongest,1);
	rb_define_method(csqlrcursor,"getResultSetId",
				(CAST)sqlrcur_getResultSetId,0);
	rb_define_method(csqlrcursor,"suspendResultSet",
				(CAST)sqlrcur_suspendResultSet,0);
	rb_define_method(csqlrcursor,"resumeResultSet",
				(CAST)sqlrcur_resumeResultSet,1);
	rb_define_method(csqlrcursor,"resumeCachedResultSet",
				(CAST)sqlrcur_resumeCachedResultSet,2);
}

void Init_sqlrelay() {
	Init_SQLRConnection();
	Init_SQLRCursor();
}

}
