/* Copyright (c) 2000  Adam Kropielnicki
   See the file COPYING for more information */

#include "sqlrelay/sqlrclient.h"

#include "phpincludes.h"

extern "C" {

#if WIN32|WINNT
#include <windows.h>
#define DLEXPORT __declspec(dllexport)
#else
#define DLEXPORT
#endif

DLEXPORT ZEND_FUNCTION(sqlrcon_alloc) {
	zval **server,**port,**socket,**user,**password,**retrytime,**tries;
	sqlrconnection *s;
	if (ZEND_NUM_ARGS() != 7 || 
		zend_get_parameters_ex(7,&server,&port,&socket,
			&user,&password,&retrytime,&tries ) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(server);
	convert_to_long_ex(port);
	convert_to_string_ex(socket);
	convert_to_string_ex(user);
	convert_to_string_ex(password);
	convert_to_long_ex(retrytime);
	convert_to_long_ex(tries);
	s=new sqlrconnection((*server)->value.str.val,
			(*port)->value.lval,(*socket)->value.str.val,
			(*user)->value.str.val,(*password)->value.str.val,
			(*retrytime)->value.lval,(*tries)->value.lval);
	s->copyReferences();
	s->debugPrintFunction(zend_printf);
	RETURN_LONG((long)s);
}

DLEXPORT ZEND_FUNCTION(sqlrcon_free) {
	zval **sqlrcon;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcon);
	delete ((sqlrconnection *)(*sqlrcon)->value.lval);
}

DLEXPORT ZEND_FUNCTION(sqlrcon_endsession) {
	zval **sqlrcon;
	int r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcon);
	r=((sqlrconnection *)(*sqlrcon)->value.lval)->endSession();
	RETURN_LONG(r);
}

DLEXPORT ZEND_FUNCTION(sqlrcon_suspendsession) {
	zval **sqlrcon;
	int r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcon);
	r=((sqlrconnection *)(*sqlrcon)->value.lval)->suspendSession();
	RETURN_LONG(r);
}

DLEXPORT ZEND_FUNCTION(sqlrcon_getconnectionport) {
	zval **sqlrcon;
	int r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcon);
	r=((sqlrconnection *)(*sqlrcon)->value.lval)->getConnectionPort();
	RETURN_LONG(r);
}

DLEXPORT ZEND_FUNCTION(sqlrcon_getconnectionsocket) {
	zval **sqlrcon;
	char *r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcon);
	r=((sqlrconnection *)(*sqlrcon)->value.lval)->getConnectionSocket();
	if (!r) {
		RETURN_FALSE;
	}
	RETURN_STRING(r,1);
}

DLEXPORT ZEND_FUNCTION(sqlrcon_resumesession) {
	zval **sqlrcon,**port,**socket;
	int r;
	if (ZEND_NUM_ARGS() != 3 || 
		zend_get_parameters_ex(3,&sqlrcon,&port,&socket) 
					== FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcon);
	convert_to_long_ex(port);
	convert_to_string_ex(socket);
	r=((sqlrconnection *)(*sqlrcon)->value.lval)->resumeSession((*port)->value.lval,(*socket)->value.str.val);
	RETURN_LONG(r);
}

DLEXPORT ZEND_FUNCTION(sqlrcon_debugon) {
	zval **sqlrcon;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcon);
	((sqlrconnection *)(*sqlrcon)->value.lval)->debugOn();
}

DLEXPORT ZEND_FUNCTION(sqlrcon_debugoff) {
	zval **sqlrcon;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcon);
	((sqlrconnection *)(*sqlrcon)->value.lval)->debugOff();
}

DLEXPORT ZEND_FUNCTION(sqlrcon_getdebug) {
	zval **sqlrcon;
	int r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcon);
	r=((sqlrconnection *)(*sqlrcon)->value.lval)->getDebug();
	if (r) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_alloc) {
	zval	**sqlrcon;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcon);
	sqlrcursor	*s=new sqlrcursor((sqlrconnection *)(*sqlrcon)->value.lval);
	s->copyReferences();
	RETURN_LONG((long)s);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_free) {
	zval **sqlrcur;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	delete ((sqlrcursor *)(*sqlrcur)->value.lval);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_setresultsetbuffersize) {
	zval **sqlrcur,**rows;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&rows) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	convert_to_long_ex(rows);
	((sqlrcursor *)(*sqlrcur)->value.lval)->setResultSetBufferSize((*rows)->value.lval);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getresultsetbuffersize) {
	zval **sqlrcur;
	int r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	r=((sqlrcursor *)(*sqlrcur)->value.lval)->getResultSetBufferSize();
	RETURN_LONG(r);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_dontgetcolumninfo) {
	zval **sqlrcur;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	((sqlrcursor *)(*sqlrcur)->value.lval)->dontGetColumnInfo();
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getcolumninfo) {
	zval **sqlrcur;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	((sqlrcursor *)(*sqlrcur)->value.lval)->getColumnInfo();
}

DLEXPORT ZEND_FUNCTION(sqlrcur_mixedcasecolumnnames) {
	zval **sqlrcur;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	((sqlrcursor *)(*sqlrcur)->value.lval)->mixedCaseColumnNames();
}

DLEXPORT ZEND_FUNCTION(sqlrcur_uppercasecolumnnames) {
	zval **sqlrcur;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	((sqlrcursor *)(*sqlrcur)->value.lval)->upperCaseColumnNames();
}

DLEXPORT ZEND_FUNCTION(sqlrcur_lowercasecolumnnames) {
	zval **sqlrcur;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	((sqlrcursor *)(*sqlrcur)->value.lval)->lowerCaseColumnNames();
}

DLEXPORT ZEND_FUNCTION(sqlrcur_cachetofile) {
	zval **sqlrcur,**filename;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&filename) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	convert_to_string_ex(filename);
	((sqlrcursor *)(*sqlrcur)->value.lval)->cacheToFile((*filename)->value.str.val);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_setcachettl) {
	zval **sqlrcur,**ttl;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&ttl) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	convert_to_long_ex(ttl);
	((sqlrcursor *)(*sqlrcur)->value.lval)->setCacheTtl((*ttl)->value.lval);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getcachefilename) {
	zval **sqlrcur;
	char *r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	r=((sqlrcursor *)(*sqlrcur)->value.lval)->getCacheFileName();
	if (!r) {
		RETURN_FALSE;
	}
	RETURN_STRING(r,1);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_cacheoff) {
	zval **sqlrcur;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	((sqlrcursor *)(*sqlrcur)->value.lval)->cacheOff();
}

DLEXPORT ZEND_FUNCTION(sqlrcur_sendquery) {
	zval **sqlrcur,**query;
	int r;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&query) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	convert_to_string_ex(query);
	r=((sqlrcursor *)(*sqlrcur)->value.lval)->sendQuery((*query)->value.str.val);
	RETURN_LONG(r);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_sendquerywithlength) {
	zval **sqlrcur,**query,**length;
	int r;
	if (ZEND_NUM_ARGS() != 3 || 
		zend_get_parameters_ex(3,&sqlrcur,&query,&length) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	convert_to_string_ex(query);
	convert_to_long_ex(length);
	r=((sqlrcursor *)(*sqlrcur)->value.lval)->sendQuery((*query)->value.str.val,(*length)->value.lval);
	RETURN_LONG(r);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_sendfilequery) {
	zval **sqlrcur,**path,**filename;
	int r;
	if (ZEND_NUM_ARGS() != 3 || 
		zend_get_parameters_ex(3,&sqlrcur,&path,&filename) 
					== FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	convert_to_string_ex(path);
	convert_to_string_ex(filename);
	r=((sqlrcursor *)(*sqlrcur)->value.lval)->sendFileQuery((*path)->value.str.val,(*filename)->value.str.val);
	RETURN_LONG(r);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_preparequery) {
	zval **sqlrcur,**query;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&query) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	convert_to_string_ex(query);
	((sqlrcursor *)(*sqlrcur)->value.lval)->prepareQuery((*query)->value.str.val);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_preparequerywithlength) {
	zval **sqlrcur,**query,**length;
	if (ZEND_NUM_ARGS() != 3 || 
		zend_get_parameters_ex(3,&sqlrcur,&query,&length) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	convert_to_string_ex(query);
	convert_to_long_ex(length);
	((sqlrcursor *)(*sqlrcur)->value.lval)->prepareQuery((*query)->value.str.val,(*length)->value.lval);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_preparefilequery) {
	zval **sqlrcur,**path,**filename;
	int r;
	if (ZEND_NUM_ARGS() != 3 || 
		zend_get_parameters_ex(3,&sqlrcur,&path,&filename) 
					== FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	convert_to_string_ex(path);
	convert_to_string_ex(filename);
	r=((sqlrcursor *)(*sqlrcur)->value.lval)->prepareFileQuery((*path)->value.str.val,(*filename)->value.str.val);
	RETURN_LONG(r);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_substitution) {
	zval **sqlrcur,**variable,**value,**precision,**scale;
	if (ZEND_NUM_ARGS() != 3 || 
		zend_get_parameters_ex(3,&sqlrcur,&variable,&value) 
					== FAILURE) {
		if (ZEND_NUM_ARGS() != 5 || 
			zend_get_parameters_ex(5,&sqlrcur,
				&variable,&value,&precision,&scale)== FAILURE) {
			WRONG_PARAM_COUNT;
		}
	}
	convert_to_long_ex(sqlrcur);
	convert_to_string_ex(variable);
	if (Z_TYPE_PP(value)==IS_STRING) {
		convert_to_string_ex(value);
		((sqlrcursor *)(*sqlrcur)->value.lval)->substitution((*variable)->value.str.val,(*value)->value.str.val);
	} else if (Z_TYPE_PP(value)==IS_LONG) {
		convert_to_long_ex(value);
		((sqlrcursor *)(*sqlrcur)->value.lval)->substitution((*variable)->value.str.val,(*value)->value.lval);
	} else if (ZEND_NUM_ARGS()==5 && Z_TYPE_PP(value)==IS_DOUBLE) {
		convert_to_double_ex(value);
		((sqlrcursor *)(*sqlrcur)->value.lval)->substitution((*variable)->value.str.val,(*value)->value.dval,(unsigned short)(*precision)->value.lval,(unsigned short)(*scale)->value.lval);
	} else if (Z_TYPE_PP(value)==IS_NULL) {
		((sqlrcursor *)(*sqlrcur)->value.lval)->substitution((*variable)->value.str.val,(char *)NULL);
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_clearbinds) {
	zval **sqlrcur;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	((sqlrcursor *)(*sqlrcur)->value.lval)->clearBinds();
}

DLEXPORT ZEND_FUNCTION(sqlrcur_inputbind) {
	zval **sqlrcur,**variable,**value,**precision,**scale;
	if (ZEND_NUM_ARGS() != 3 || 
		zend_get_parameters_ex(3,&sqlrcur,&variable,&value) 
					== FAILURE) {
		if (ZEND_NUM_ARGS() != 5 || 
			zend_get_parameters_ex(5,&sqlrcur,
				&variable,&value,&precision,&scale)== FAILURE) {
			WRONG_PARAM_COUNT;
		}
	}
	convert_to_long_ex(sqlrcur);
	convert_to_string_ex(variable);
	if (Z_TYPE_PP(value)==IS_STRING) {
		convert_to_string_ex(value);
		((sqlrcursor *)(*sqlrcur)->value.lval)->inputBind((*variable)->value.str.val,(*value)->value.str.val);
	} else if (Z_TYPE_PP(value)==IS_LONG) {
		convert_to_long_ex(value);
		((sqlrcursor *)(*sqlrcur)->value.lval)->inputBind((*variable)->value.str.val,(*value)->value.lval);
	} else if (ZEND_NUM_ARGS()==5 && Z_TYPE_PP(value)==IS_DOUBLE) {
		convert_to_double_ex(value);
		((sqlrcursor *)(*sqlrcur)->value.lval)->inputBind((*variable)->value.str.val,(*value)->value.dval,(unsigned short)(*precision)->value.lval,(unsigned short)(*scale)->value.lval);
	} else if (Z_TYPE_PP(value)==IS_NULL) {
		((sqlrcursor *)(*sqlrcur)->value.lval)->inputBind((*variable)->value.str.val,(char *)NULL);
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_inputbindblob) {
	zval **sqlrcur,**variable,**value,**size;
	if (ZEND_NUM_ARGS() != 4 || 
		zend_get_parameters_ex(4,&sqlrcur,&variable,&value,&size) 
					== FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	convert_to_string_ex(variable);
	convert_to_string_ex(value);
	convert_to_long_ex(size);
	((sqlrcursor *)(*sqlrcur)->value.lval)->inputBindBlob((*variable)->value.str.val,(*value)->value.str.val,(*size)->value.lval);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_inputbindclob) {
	zval **sqlrcur,**variable,**value,**size;
	if (ZEND_NUM_ARGS() != 4 || 
		zend_get_parameters_ex(4,&sqlrcur,&variable,&value,&size) 
					== FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	convert_to_string_ex(variable);
	convert_to_string_ex(value);
	convert_to_long_ex(size);
	((sqlrcursor *)(*sqlrcur)->value.lval)->inputBindClob((*variable)->value.str.val,(*value)->value.str.val,(*size)->value.lval);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_defineoutputbind) {
	zval **sqlrcur,**variable,**length;
	if (ZEND_NUM_ARGS() != 3 || 
		zend_get_parameters_ex(3,&sqlrcur,&variable,&length) 
					== FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	convert_to_string_ex(variable);
	convert_to_long_ex(length);
	((sqlrcursor *)(*sqlrcur)->value.lval)->defineOutputBind((*variable)->value.str.val,(*length)->value.lval);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_defineoutputbindblob) {
	zval **sqlrcur,**variable;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&variable) 
					== FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	convert_to_string_ex(variable);
	((sqlrcursor *)(*sqlrcur)->value.lval)->defineOutputBindBlob((*variable)->value.str.val);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_defineoutputbindclob) {
	zval **sqlrcur,**variable;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&variable) 
					== FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	convert_to_string_ex(variable);
	((sqlrcursor *)(*sqlrcur)->value.lval)->defineOutputBindClob((*variable)->value.str.val);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_defineoutputbindcursor) {
	zval **sqlrcur,**variable;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&variable) 
					== FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	convert_to_string_ex(variable);
	((sqlrcursor *)(*sqlrcur)->value.lval)->defineOutputBindCursor((*variable)->value.str.val);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_substitutions) {
	zval **sqlrcur,**variables,**values,**precisions,**scales,**var,**val,**precision,**scale;
	int i;
	if (ZEND_NUM_ARGS() != 3 ||
		zend_get_parameters_ex(3,&sqlrcur,&variables,&values) 
					== FAILURE) {
		if (ZEND_NUM_ARGS() != 5 || 
			zend_get_parameters_ex(5,&sqlrcur,
				&variables,&values,
				&precisions,&scales)== FAILURE) {
			WRONG_PARAM_COUNT;
		} else {
			convert_to_array_ex(precisions);
			convert_to_array_ex(scales);
		}
	}
	convert_to_long_ex(sqlrcur);
	convert_to_array_ex(variables);
	convert_to_array_ex(values);
	for (i=0; i<(*variables)->value.ht->nNumOfElements; i++) {
		zend_hash_index_find((*variables)->value.ht,i,(void **)&var);
		zend_hash_index_find((*values)->value.ht,i,(void **)&val);
		if (Z_TYPE_PP(val)==IS_STRING) {
			convert_to_string_ex(val);
			((sqlrcursor *)(*sqlrcur)->value.lval)->substitution((*var)->value.str.val,(*val)->value.str.val);
		} else if (Z_TYPE_PP(val)==IS_LONG) {
			convert_to_long_ex(val);
			((sqlrcursor *)(*sqlrcur)->value.lval)->substitution((*var)->value.str.val,(*val)->value.lval);
		} else if (ZEND_NUM_ARGS()==5 && Z_TYPE_PP(val)==IS_DOUBLE) {
			zend_hash_index_find((*precisions)->value.ht,i,
							(void **)&precision);
			zend_hash_index_find((*scales)->value.ht,i,
							(void **)&scale);
			convert_to_double_ex(val);
			convert_to_long_ex(precision);
			convert_to_long_ex(scale);
			((sqlrcursor *)(*sqlrcur)->value.lval)->substitution((*var)->value.str.val,(*val)->value.dval,(unsigned short)(*precision)->value.lval,(unsigned short)(*scale)->value.lval);
		} else if (Z_TYPE_PP(val)==IS_NULL) {
			((sqlrcursor *)(*sqlrcur)->value.lval)->substitution((*var)->value.str.val,(char *)NULL);
		}
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_inputbinds) {
	zval **sqlrcur,**variables,**values,**precisions,**scales,**var,**val,**precision,**scale;
	int i;
	if (ZEND_NUM_ARGS() != 3 ||
		zend_get_parameters_ex(3,&sqlrcur,&variables,&values) 
					== FAILURE) {
		if (ZEND_NUM_ARGS() != 5 || 
			zend_get_parameters_ex(5,&sqlrcur,
				&variables,&values,
				&precisions,&scales)== FAILURE) {
			WRONG_PARAM_COUNT;
		} else {
			convert_to_array_ex(precisions);
			convert_to_array_ex(scales);
		}
	}
	convert_to_long_ex(sqlrcur);
	convert_to_array_ex(variables);
	convert_to_array_ex(values);
	for (i=0; i<zend_hash_num_elements((*variables)->value.ht); i++) {
		zend_hash_index_find((*variables)->value.ht,i,(void **)&var);
		zend_hash_index_find((*values)->value.ht,i,(void **)&val);
		if (Z_TYPE_PP(val)==IS_STRING) {
			convert_to_string_ex(val);
			((sqlrcursor *)(*sqlrcur)->value.lval)->inputBind((*var)->value.str.val,(*val)->value.str.val);
		} else if (Z_TYPE_PP(val)==IS_LONG) {
			convert_to_long_ex(val);
			((sqlrcursor *)(*sqlrcur)->value.lval)->inputBind((*var)->value.str.val,(*val)->value.lval);
		} else if (ZEND_NUM_ARGS()==5 && Z_TYPE_PP(val)==IS_DOUBLE) {
			zend_hash_index_find((*precisions)->value.ht,i,
							(void **)&precision);
			zend_hash_index_find((*scales)->value.ht,i,
							(void **)&scale);
			convert_to_long_ex(precision);
			convert_to_long_ex(scale);
			convert_to_double_ex(val);
			((sqlrcursor *)(*sqlrcur)->value.lval)->inputBind((*var)->value.str.val,(*val)->value.dval,(unsigned short)(*precision)->value.lval,(unsigned short)(*scale)->value.lval);
		} else if (Z_TYPE_PP(val)==IS_NULL) {
			((sqlrcursor *)(*sqlrcur)->value.lval)->inputBind((*var)->value.str.val,(char *)NULL);
		}
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_validatebinds) {
	zval **sqlrcur;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	((sqlrcursor *)(*sqlrcur)->value.lval)->validateBinds();
}

DLEXPORT ZEND_FUNCTION(sqlrcur_executequery) {
	zval **sqlrcur;
	int r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	r=((sqlrcursor *)(*sqlrcur)->value.lval)->executeQuery();
	RETURN_LONG(r);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_fetchfrombindcursor) {
	zval **sqlrcur;
	int r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	r=((sqlrcursor *)(*sqlrcur)->value.lval)->fetchFromBindCursor();
	RETURN_LONG(r);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getoutputbind) {
	zval **sqlrcur,**variable;
	char *r;
	long rl;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&variable) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	convert_to_string_ex(variable);
	r=((sqlrcursor *)(*sqlrcur)->value.lval)->getOutputBind((*variable)->value.str.val);
	rl=((sqlrcursor *)(*sqlrcur)->value.lval)->getOutputBindLength((*variable)->value.str.val);
	if (!r) {
		RETURN_NULL();
	}
	RETURN_STRINGL(r,rl,1);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getoutputbindaslong) {
	zval **sqlrcur,**variable;
	long r;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&variable) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	convert_to_string_ex(variable);
	r=((sqlrcursor *)(*sqlrcur)->value.lval)->getOutputBindAsLong((*variable)->value.str.val);
	RETURN_LONG(r);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getoutputbindasdouble) {
	zval **sqlrcur,**variable;
	double r;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&variable) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	convert_to_string_ex(variable);
	r=((sqlrcursor *)(*sqlrcur)->value.lval)->getOutputBindAsDouble((*variable)->value.str.val);
	RETURN_DOUBLE(r);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getoutputbindlength) {
	zval **sqlrcur,**variable;
	long r;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&variable) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	convert_to_string_ex(variable);
	r=((sqlrcursor *)(*sqlrcur)->value.lval)->getOutputBindLength((*variable)->value.str.val);
	RETURN_LONG(r);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getoutputbindcursor) {
	zval **sqlrcur,**variable;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&variable) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	convert_to_string_ex(variable);
	sqlrcursor	*s=((sqlrcursor *)(*sqlrcur)->value.lval)->getOutputBindCursor((*variable)->value.str.val);
	s->copyReferences();
	RETURN_LONG((long)s);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_opencachedresultset) {
	zval **sqlrcur,**filename;
	int r;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&filename) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	convert_to_string_ex(filename);
	r=((sqlrcursor *)(*sqlrcur)->value.lval)->openCachedResultSet((*filename)->value.str.val);
	RETURN_LONG(r);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_colcount) {
	zval **sqlrcur;
	int r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	r=((sqlrcursor *)(*sqlrcur)->value.lval)->colCount();
	RETURN_LONG(r);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_rowcount) {
	zval **sqlrcur;
	int r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	r=((sqlrcursor *)(*sqlrcur)->value.lval)->rowCount();
	RETURN_LONG(r);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_totalrows) {
	zval **sqlrcur;
	int r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	r=((sqlrcursor *)(*sqlrcur)->value.lval)->totalRows();
	RETURN_LONG(r);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_affectedrows) {
	zval **sqlrcur;
	int r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	r=((sqlrcursor *)(*sqlrcur)->value.lval)->affectedRows();
	RETURN_LONG(r);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_firstrowindex) {
	zval **sqlrcur;
	int r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	r=((sqlrcursor *)(*sqlrcur)->value.lval)->firstRowIndex();
	RETURN_LONG(r);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_endofresultset) {
	zval **sqlrcur;
	int r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	r=((sqlrcursor *)(*sqlrcur)->value.lval)->endOfResultSet();
	RETURN_LONG(r);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_errormessage) {
	zval **sqlrcur;
	char *r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	r=((sqlrcursor *)(*sqlrcur)->value.lval)->errorMessage();
	if (!r) {
		RETURN_NULL();
	}
	RETURN_STRING(r,1);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getnullsasemptystrings) {
	zval **sqlrcur;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	((sqlrcursor *)(*sqlrcur)->value.lval)->getNullsAsEmptyStrings();
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getnullsasnulls) {
	zval **sqlrcur;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	((sqlrcursor *)(*sqlrcur)->value.lval)->getNullsAsNulls();
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getfield) {
	zval **sqlrcur,**row,**col;
	char *r=NULL;
	long rl;
	if (ZEND_NUM_ARGS() != 3 || 
		zend_get_parameters_ex(3,&sqlrcur,&row,&col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	convert_to_long_ex(row);
	if (Z_TYPE_PP(col)==IS_LONG) {
		convert_to_long_ex(col);
		r=((sqlrcursor *)(*sqlrcur)->value.lval)->getField((*row)->value.lval,(*col)->value.lval);
		rl=((sqlrcursor *)(*sqlrcur)->value.lval)->getFieldLength((*row)->value.lval,(*col)->value.lval);
	} else if (Z_TYPE_PP(col)==IS_STRING) {
		convert_to_string_ex(col);
		r=((sqlrcursor *)(*sqlrcur)->value.lval)->getField((*row)->value.lval,(*col)->value.str.val);
		rl=((sqlrcursor *)(*sqlrcur)->value.lval)->getFieldLength((*row)->value.lval,(*col)->value.str.val);
	}
	if (!r) {
		RETURN_NULL();
	}
	RETURN_STRINGL(r,rl,1);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getfieldaslong) {
	zval **sqlrcur,**row,**col;
	long r=0;
	if (ZEND_NUM_ARGS() != 3 || 
		zend_get_parameters_ex(3,&sqlrcur,&row,&col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	convert_to_long_ex(row);
	if (Z_TYPE_PP(col)==IS_LONG) {
		convert_to_long_ex(col);
		r=((sqlrcursor *)(*sqlrcur)->value.lval)->getFieldAsLong((*row)->value.lval,(*col)->value.lval);
	} else if (Z_TYPE_PP(col)==IS_STRING) {
		convert_to_string_ex(col);
		r=((sqlrcursor *)(*sqlrcur)->value.lval)->getFieldAsLong((*row)->value.lval,(*col)->value.str.val);
	}
	RETURN_LONG(r);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getfieldasdouble) {
	zval **sqlrcur,**row,**col;
	double r=0.0;
	if (ZEND_NUM_ARGS() != 3 || 
		zend_get_parameters_ex(3,&sqlrcur,&row,&col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	convert_to_long_ex(row);
	if (Z_TYPE_PP(col)==IS_LONG) {
		convert_to_long_ex(col);
		r=((sqlrcursor *)(*sqlrcur)->value.lval)->getFieldAsDouble((*row)->value.lval,(*col)->value.lval);
	} else if (Z_TYPE_PP(col)==IS_STRING) {
		convert_to_string_ex(col);
		r=((sqlrcursor *)(*sqlrcur)->value.lval)->getFieldAsDouble((*row)->value.lval,(*col)->value.str.val);
	}
	RETURN_DOUBLE(r);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getfieldlength) {
	zval **sqlrcur,**row,**col;
	long r=-1;
	if (ZEND_NUM_ARGS() != 3 || 
		zend_get_parameters_ex(3,&sqlrcur,&row,&col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	convert_to_long_ex(row);
	if (Z_TYPE_PP(col)==IS_LONG) {
		convert_to_long_ex(col);
		r=((sqlrcursor *)(*sqlrcur)->value.lval)->getFieldLength((*row)->value.lval,(*col)->value.lval);
	} else if (Z_TYPE_PP(col)==IS_STRING) {
		convert_to_string_ex(col);
		r=((sqlrcursor *)(*sqlrcur)->value.lval)->getFieldLength((*row)->value.lval,(*col)->value.str.val);
	}
	RETURN_LONG(r);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getrow) {
	zval **sqlrcur,**row;
	char **r;
	int i;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&row) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	convert_to_long_ex(row);
	r=((sqlrcursor *)(*sqlrcur)->value.lval)->getRow((*row)->value.lval);
	if (!r) {
		RETURN_FALSE;
	}
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	for (i=0; i<((sqlrcursor *)(*sqlrcur)->value.lval)->colCount(); i++) {
		if (!r[i]) {
			add_next_index_null(return_value);
		} else {
			add_next_index_string(return_value,r[i],1);
		}
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getrowassoc) {
	zval **sqlrcur,**row;
	char **r,**rC;
	int i;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&row) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(sqlrcur);
	convert_to_long_ex(row);

	rC=((sqlrcursor *)(*sqlrcur)->value.lval)->getColumnNames();
	if (!rC) {
		RETURN_FALSE;
	}

	r=((sqlrcursor *)(*sqlrcur)->value.lval)->getRow((*row)->value.lval);
	if (!r) {
		RETURN_FALSE;
	}
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	for (i=0; i<((sqlrcursor *)(*sqlrcur)->value.lval)->colCount(); i++) {
		if (!r[i]) {
			add_assoc_null(return_value,rC[i]);
		} else {
			add_assoc_string(return_value,rC[i],r[i],1);
		}
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getrowlengths) {
	zval **sqlrcur,**row;
	long *r;
	int i;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&row) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	convert_to_long_ex(row);
	r=((sqlrcursor *)(*sqlrcur)->value.lval)->getRowLengths((*row)->value.lval);
	if (!r) {
		RETURN_FALSE;
	}
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	for (i=0; i<((sqlrcursor *)(*sqlrcur)->value.lval)->colCount(); i++) {
		add_next_index_long(return_value,r[i]);
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getrowlengthsassoc) {
	zval **sqlrcur,**row;
	long *r;
	char **rC;
	int i;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&row) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(sqlrcur);
	convert_to_long_ex(row);

	rC=((sqlrcursor *)(*sqlrcur)->value.lval)->getColumnNames();
	if (!rC) {
		RETURN_FALSE;
	}

	r=((sqlrcursor *)(*sqlrcur)->value.lval)->getRowLengths((*row)->value.lval);
	if (!r) {
		RETURN_FALSE;
	}
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	for (i=0; i<((sqlrcursor *)(*sqlrcur)->value.lval)->colCount(); i++) {
		add_assoc_long(return_value,rC[i],r[i]);
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getcolumnnames) {
	zval **sqlrcur;
	char **r;
	int i;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	r=((sqlrcursor *)(*sqlrcur)->value.lval)->getColumnNames();
	if (!r) {
		RETURN_FALSE;
	}
	if (array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}
	for (i=0; i<((sqlrcursor *)(*sqlrcur)->value.lval)->colCount(); i++) {
		add_next_index_string(return_value,r[i],1);
	}
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getcolumnname) {
	zval **sqlrcur,**col;
	char *r;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	convert_to_long_ex(col);
	r=((sqlrcursor *)(*sqlrcur)->value.lval)->getColumnName((*col)->value.lval);
	if (!r) {
		RETURN_FALSE;
	}
	RETURN_STRING(r,1);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getcolumntype) {
	zval **sqlrcur,**col;
	char *r=NULL;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	if (Z_TYPE_PP(col)==IS_LONG) {
		convert_to_long_ex(col);
		r=((sqlrcursor *)(*sqlrcur)->value.lval)->getColumnType((*col)->value.lval);
	} else if (Z_TYPE_PP(col)==IS_STRING) {
		convert_to_string_ex(col);
		r=((sqlrcursor *)(*sqlrcur)->value.lval)->getColumnType((*col)->value.str.val);
	}
	if (!r) {
		RETURN_FALSE;
	}
	RETURN_STRING(r,1);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getcolumnlength) {
	zval **sqlrcur,**col;
	int r=0;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	if (Z_TYPE_PP(col)==IS_LONG) {
		convert_to_long_ex(col);
		r=((sqlrcursor *)(*sqlrcur)->value.lval)->getColumnLength((*col)->value.lval);
	} else if (Z_TYPE_PP(col)==IS_STRING) {
		convert_to_string_ex(col);
		r=((sqlrcursor *)(*sqlrcur)->value.lval)->getColumnLength((*col)->value.str.val);
	}
	RETURN_LONG(r);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getcolumnprecision) {
	zval **sqlrcur,**col;
	int r=0;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	if (Z_TYPE_PP(col)==IS_LONG) {
		convert_to_long_ex(col);
		r=((sqlrcursor *)(*sqlrcur)->value.lval)->getColumnPrecision((*col)->value.lval);
	} else if (Z_TYPE_PP(col)==IS_STRING) {
		convert_to_string_ex(col);
		r=((sqlrcursor *)(*sqlrcur)->value.lval)->getColumnPrecision((*col)->value.str.val);
	}
	RETURN_LONG(r);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getcolumnscale) {
	zval **sqlrcur,**col;
	int r=0;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	if (Z_TYPE_PP(col)==IS_LONG) {
		convert_to_long_ex(col);
		r=((sqlrcursor *)(*sqlrcur)->value.lval)->getColumnScale((*col)->value.lval);
	} else if (Z_TYPE_PP(col)==IS_STRING) {
		convert_to_string_ex(col);
		r=((sqlrcursor *)(*sqlrcur)->value.lval)->getColumnScale((*col)->value.str.val);
	}
	RETURN_LONG(r);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getcolumnisnullable) {
	zval **sqlrcur,**col;
	int r=0;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	if (Z_TYPE_PP(col)==IS_LONG) {
		convert_to_long_ex(col);
		r=((sqlrcursor *)(*sqlrcur)->value.lval)->getColumnIsNullable((*col)->value.lval);
	} else if (Z_TYPE_PP(col)==IS_STRING) {
		convert_to_string_ex(col);
		r=((sqlrcursor *)(*sqlrcur)->value.lval)->getColumnIsNullable((*col)->value.str.val);
	}
	RETURN_LONG(r);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getcolumnisprimarykey) {
	zval **sqlrcur,**col;
	int r=0;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	if (Z_TYPE_PP(col)==IS_LONG) {
		convert_to_long_ex(col);
		r=((sqlrcursor *)(*sqlrcur)->value.lval)->getColumnIsPrimaryKey((*col)->value.lval);
	} else if (Z_TYPE_PP(col)==IS_STRING) {
		convert_to_string_ex(col);
		r=((sqlrcursor *)(*sqlrcur)->value.lval)->getColumnIsPrimaryKey((*col)->value.str.val);
	}
	RETURN_LONG(r);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getlongest) {
	zval **sqlrcur,**col;
	int r=0;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&col) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	if (Z_TYPE_PP(col)==IS_LONG) {
		convert_to_long_ex(col);
		r=((sqlrcursor *)(*sqlrcur)->value.lval)->getLongest((*col)->value.lval);
	} else if (Z_TYPE_PP(col)==IS_STRING) {
		convert_to_string_ex(col);
		r=((sqlrcursor *)(*sqlrcur)->value.lval)->getLongest((*col)->value.str.val);
	}
	RETURN_LONG(r);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_getresultsetid) {
	zval **sqlrcur;
	int r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	r=((sqlrcursor *)(*sqlrcur)->value.lval)->getResultSetId();
	RETURN_LONG(r);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_suspendresultset) {
	zval **sqlrcur;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcur) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	((sqlrcursor *)(*sqlrcur)->value.lval)->suspendResultSet();
}

DLEXPORT ZEND_FUNCTION(sqlrcur_resumeresultset) {
	zval **sqlrcur, **id;
	int r;
	if (ZEND_NUM_ARGS() != 2 || 
		zend_get_parameters_ex(2,&sqlrcur,&id) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	convert_to_long_ex(id);
	r=((sqlrcursor *)(*sqlrcur)->value.lval)->resumeResultSet((*id)->value.lval);
	RETURN_LONG(r);
}

DLEXPORT ZEND_FUNCTION(sqlrcur_resumecachedresultset) {
	zval **sqlrcur, **id, **filename;
	int r;
	if (ZEND_NUM_ARGS() != 3 || 
		zend_get_parameters_ex(3,&sqlrcur,&id,&filename) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcur);
	convert_to_long_ex(id);
	convert_to_string_ex(filename);
	r=((sqlrcursor *)(*sqlrcur)->value.lval)->resumeCachedResultSet((*id)->value.lval,(*filename)->value.str.val);
	RETURN_LONG(r);
}

DLEXPORT ZEND_FUNCTION(sqlrcon_ping) {
	zval **sqlrcon;
	int r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcon);
	r=((sqlrconnection *)(*sqlrcon)->value.lval)->ping();
	RETURN_LONG(r);
}

DLEXPORT ZEND_FUNCTION(sqlrcon_autocommiton) {
	zval **sqlrcon;
	int r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcon);
	r=((sqlrconnection *)(*sqlrcon)->value.lval)->autoCommitOn();
	RETURN_LONG(r);
}

DLEXPORT ZEND_FUNCTION(sqlrcon_autocommitoff) {
	zval **sqlrcon;
	int r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcon);
	r=((sqlrconnection *)(*sqlrcon)->value.lval)->autoCommitOff();
	RETURN_LONG(r);
}

DLEXPORT ZEND_FUNCTION(sqlrcon_commit) {
	zval **sqlrcon;
	int r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcon);
	r=((sqlrconnection *)(*sqlrcon)->value.lval)->commit();
	RETURN_LONG(r);
}

DLEXPORT ZEND_FUNCTION(sqlrcon_rollback) {
	zval **sqlrcon;
	int r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcon);
	r=((sqlrconnection *)(*sqlrcon)->value.lval)->rollback();
	RETURN_LONG(r);
}

DLEXPORT ZEND_FUNCTION(sqlrcon_identify) {
	zval **sqlrcon;
	char *r;
	if (ZEND_NUM_ARGS() != 1 || 
		zend_get_parameters_ex(1,&sqlrcon) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long_ex(sqlrcon);
	r=((sqlrconnection *)(*sqlrcon)->value.lval)->identify();
	if (!r) {
		RETURN_FALSE;
	}
	RETURN_STRING(r,1);
}

zend_function_entry sql_relay_functions[] = {
	ZEND_FE(sqlrcon_alloc,NULL)
	ZEND_FE(sqlrcon_free,NULL)
	ZEND_FE(sqlrcon_endsession,NULL)
	ZEND_FE(sqlrcon_suspendsession,NULL)
	ZEND_FE(sqlrcon_getconnectionport,NULL)
	ZEND_FE(sqlrcon_getconnectionsocket,NULL)
	ZEND_FE(sqlrcon_resumesession,NULL)
	ZEND_FE(sqlrcon_debugon,NULL)
	ZEND_FE(sqlrcon_debugoff,NULL)
	ZEND_FE(sqlrcon_getdebug,NULL)
	ZEND_FE(sqlrcur_alloc,NULL)
	ZEND_FE(sqlrcur_free,NULL)
	ZEND_FE(sqlrcur_setresultsetbuffersize,NULL)
	ZEND_FE(sqlrcur_getresultsetbuffersize,NULL)
	ZEND_FE(sqlrcur_dontgetcolumninfo,NULL)
	ZEND_FE(sqlrcur_getcolumninfo,NULL)
	ZEND_FE(sqlrcur_mixedcasecolumnnames,NULL)
	ZEND_FE(sqlrcur_uppercasecolumnnames,NULL)
	ZEND_FE(sqlrcur_lowercasecolumnnames,NULL)
	ZEND_FE(sqlrcur_cachetofile,NULL)
	ZEND_FE(sqlrcur_setcachettl,NULL)
	ZEND_FE(sqlrcur_getcachefilename,NULL)
	ZEND_FE(sqlrcur_cacheoff,NULL)
	ZEND_FE(sqlrcur_sendquery,NULL)
	ZEND_FE(sqlrcur_sendquerywithlength,NULL)
	ZEND_FE(sqlrcur_sendfilequery,NULL)
	ZEND_FE(sqlrcur_preparequery,NULL)
	ZEND_FE(sqlrcur_preparequerywithlength,NULL)
	ZEND_FE(sqlrcur_preparefilequery,NULL)
	ZEND_FE(sqlrcur_substitution,NULL)
	ZEND_FE(sqlrcur_clearbinds,NULL)
	ZEND_FE(sqlrcur_inputbind,NULL)
	ZEND_FE(sqlrcur_inputbindblob,NULL)
	ZEND_FE(sqlrcur_inputbindclob,NULL)
	ZEND_FE(sqlrcur_defineoutputbind,NULL)
	ZEND_FE(sqlrcur_defineoutputbindblob,NULL)
	ZEND_FE(sqlrcur_defineoutputbindclob,NULL)
	ZEND_FE(sqlrcur_defineoutputbindcursor,NULL)
	ZEND_FE(sqlrcur_substitutions,NULL)
	ZEND_FE(sqlrcur_inputbinds,NULL)
	ZEND_FE(sqlrcur_validatebinds,NULL)
	ZEND_FE(sqlrcur_executequery,NULL)
	ZEND_FE(sqlrcur_fetchfrombindcursor,NULL)
	ZEND_FE(sqlrcur_getoutputbind,NULL)
	ZEND_FE(sqlrcur_getoutputbindlength,NULL)
	ZEND_FE(sqlrcur_getoutputbindcursor,NULL)
	ZEND_FE(sqlrcur_opencachedresultset,NULL)
	ZEND_FE(sqlrcur_colcount,NULL)
	ZEND_FE(sqlrcur_rowcount,NULL)
	ZEND_FE(sqlrcur_totalrows,NULL)
	ZEND_FE(sqlrcur_affectedrows,NULL)
	ZEND_FE(sqlrcur_firstrowindex,NULL)
	ZEND_FE(sqlrcur_endofresultset,NULL)
	ZEND_FE(sqlrcur_errormessage,NULL)
	ZEND_FE(sqlrcur_getnullsasemptystrings,NULL)
	ZEND_FE(sqlrcur_getnullsasnulls,NULL)
	ZEND_FE(sqlrcur_getfield,NULL)
	ZEND_FE(sqlrcur_getfieldlength,NULL)
	ZEND_FE(sqlrcur_getrow,NULL)
	ZEND_FE(sqlrcur_getrowassoc,NULL)
	ZEND_FE(sqlrcur_getrowlengths,NULL)
	ZEND_FE(sqlrcur_getrowlengthsassoc,NULL)
	ZEND_FE(sqlrcur_getcolumnnames,NULL)
	ZEND_FE(sqlrcur_getcolumnname,NULL)
	ZEND_FE(sqlrcur_getcolumntype,NULL)
	ZEND_FE(sqlrcur_getcolumnlength,NULL)
	ZEND_FE(sqlrcur_getlongest,NULL)
	ZEND_FE(sqlrcur_getresultsetid,NULL)
	ZEND_FE(sqlrcur_suspendresultset,NULL)
	ZEND_FE(sqlrcur_resumeresultset,NULL)
	ZEND_FE(sqlrcur_resumecachedresultset,NULL)
	ZEND_FE(sqlrcon_ping,NULL)
	ZEND_FE(sqlrcon_identify,NULL)
	ZEND_FE(sqlrcon_autocommiton,NULL)
	ZEND_FE(sqlrcon_autocommitoff,NULL)
	ZEND_FE(sqlrcon_commit,NULL)
	ZEND_FE(sqlrcon_rollback,NULL)
	{NULL,NULL,NULL}
};

zend_module_entry sql_relay_module_entry = {
	#if ZEND_MODULE_API_NO >= 20010901
		STANDARD_MODULE_HEADER,
	#endif	
	"sql_relay",
	sql_relay_functions,
	NULL,NULL,NULL,NULL,NULL,
	#if ZEND_MODULE_API_NO >= 20010901
		"0.30",	
	#endif	
	STANDARD_MODULE_PROPERTIES
};

DLEXPORT ZEND_GET_MODULE(sql_relay)

}
