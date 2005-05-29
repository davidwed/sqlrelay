// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

bool sqlrconnection::getInputBinds(sqlrcursor *cursor) {

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"getting input binds...");
	#endif

	// get the number of input bind variable/values
	if (!getBindVarCount(&(cursor->inbindcount))) {
		return false;
	}
	
	// fill the buffers
	for (uint16_t i=0; i<cursor->inbindcount && i<MAXVAR; i++) {

		bindvar	*bv=&(cursor->inbindvars[i]);

		// get the variable name and type
		if (!(getBindVarName(bv) && getBindVarType(bv))) {
			return false;
		}

		// get the value
		if (bv->type==NULL_BIND) {
			getNullBind(bv);
		} else if (bv->type==STRING_BIND) {
			if (!getStringBind(bv)) {
				return false;
			}
		} else if (bv->type==LONG_BIND) {
			if (!getLongBind(bv)) {
				return false;
			}
		} else if (bv->type==DOUBLE_BIND) {
			if (!getDoubleBind(bv)) {
				return false;
			}
		} else if (bv->type==BLOB_BIND || bv->type==CLOB_BIND) {
			if (!getLobBind(bv)) {
				return false;
			}
		}		  
	}

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"done getting input binds");
	#endif
	return true;
}

bool sqlrconnection::getOutputBinds(sqlrcursor *cursor) {

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"getting output binds...");
	#endif

	// get the number of output bind variable/values
	if (!getBindVarCount(&(cursor->outbindcount))) {
		return false;
	}

	// fill the buffers
	for (uint16_t i=0; i<cursor->outbindcount && i<MAXVAR; i++) {

		bindvar	*bv=&(cursor->outbindvars[i]);

		// get the variable name and type
		if (!(getBindVarName(bv) && getBindVarType(bv))) {
			return false;
		}

		// get the size of the value
		if (bv->type==STRING_BIND) {
			if (!getBindSize(bv,stringbindvaluelength)) {
				return false;
			}
			// This must be a calloc because oracle8 get's angry if
			// these aren't initialized to NULL's.  It's possible
			// that just the first character needs to be NULL, but
			// for now I'm just going to go ahead and use calloc
			bv->value.stringval=
				(char *)bindpool->calloc(bv->valuesize+1);
			#ifdef SERVER_DEBUG
			debugPrint("connection",4,"STRING");
			#endif
		} else if (bv->type==BLOB_BIND || bv->type==CLOB_BIND) {
			if (!getBindSize(bv,lobbindvaluelength)) {
				return false;
			}
			#ifdef SERVER_DEBUG
			if (bv->type==BLOB_BIND) {
				debugPrint("connection",4,"BLOB");
			} else if (bv->type==CLOB_BIND) {
				debugPrint("connection",4,"CLOB");
			}
			#endif
		} else if (bv->type==CURSOR_BIND) {
			#ifdef SERVER_DEBUG
			debugPrint("connection",4,"CURSOR");
			#endif
			sqlrcursor	*curs=findAvailableCursor();
			if (!curs) {
				// FIXME: set error here
				return false;
			}
			bv->value.cursorid=curs->id;
		}
	}

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"done getting output binds");
	#endif
	return true;
}

bool sqlrconnection::getBindVarCount(uint16_t *count) {

	// get the number of input bind variable/values
	if (clientsock->read(count)!=sizeof(uint16_t)) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,
			"getting binds failed: client sent bad bind count size");
		#endif
		return false;
	}

	// bounds checking
	if (*count>MAXVAR) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,
			"getting binds failed: client tried to send too many binds:");
		debugPrint("connection",3,(int32_t)*count);
		#endif
		return false;
	}

	return true;
}

bool sqlrconnection::getBindVarName(bindvar *bv) {

	uint16_t	bindnamesize;

	// get the variable name size
	if (clientsock->read(&bindnamesize)!=sizeof(uint16_t)) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,
			"getting binds failed: bad variable name length size");
		#endif
		return false;
	}

	// bounds checking
	if (bindnamesize>BINDVARLENGTH) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,
			"getting binds failed: bad variable name length");
		#endif
		return false;
	}

	// get the variable name
	bv->variablesize=bindnamesize+1;
	bv->variable=(char *)bindpool->malloc(bv->variablesize+2);
	bv->variable[0]=bindVariablePrefix();
	if (clientsock->read(bv->variable+1,bindnamesize)!=bindnamesize) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,
			"getting binds failed: bad variable name");
		#endif
		return false;
	}
	bv->variable[bindnamesize+1]=(char)NULL;

	#ifdef SERVER_DEBUG
	debugPrint("connection",4,bv->variable);
	#endif

	return true;
}

bool sqlrconnection::getBindVarType(bindvar *bv) {

	// get the type
        uint16_t type;
	if (clientsock->read(&type)!=sizeof(uint16_t)) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,
				"getting binds failed: bad type size");
		#endif
		return false;
	}
	bv->type=(bindtype)type;
	
	return true;
}

bool sqlrconnection::getBindSize(bindvar *bv, uint32_t maxsize) {

	// get the size of the value
	if (clientsock->read(&(bv->valuesize))!=sizeof(uint32_t)) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",
			2,"getting binds failed: bad value length size");
		#endif
		return false;
	}

	// bounds checking
	/*if (bv->valuesize>maxsize) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,
				"getting binds failed: bad value length");
		debugPrint("connection",3,bv->valuesize);
		#endif
		return false;
	}*/

	return true;
}

void sqlrconnection::getNullBind(bindvar *bv) {

	#ifdef SERVER_DEBUG
		debugPrint("connection",4,"NULL");
	#endif

	bv->value.stringval=(char *)bindpool->malloc(1);
	bv->value.stringval[0]=(char)NULL;
	bv->valuesize=0;
	bv->isnull=nullBindValue();
}

bool sqlrconnection::getStringBind(bindvar *bv) {

	// get the size of the value
	if (!getBindSize(bv,stringbindvaluelength)) {
		return false;
	}

	// allocate space to store the value
	bv->value.stringval=(char *)bindpool->malloc(bv->valuesize+1);

	#ifdef SERVER_DEBUG
		debugPrint("connection",4,"STRING");
	#endif

	// get the bind value
	if ((uint32_t)(clientsock->read(bv->value.stringval,
			bv->valuesize))!=(uint32_t)(bv->valuesize)) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,"getting binds failed: bad value");
		#endif
		return false;
	}
	bv->value.stringval[bv->valuesize]=(char)NULL;
	bv->isnull=nonNullBindValue();

	#ifdef SERVER_DEBUG
		debugPrint("connection",4,bv->value.stringval);
	#endif

	return true;
}

bool sqlrconnection::getLongBind(bindvar *bv) {

	#ifdef SERVER_DEBUG
		debugPrint("connection",4,"LONG");
	#endif

	// get positive/negative
	char	negative;
	if (clientsock->read(&negative)!=sizeof(char)) {
		#ifdef SERVER_DEBUG
			debugPrint("connection",2,
				"getting binds failed: bad positive/negative");
		#endif
		return false;
	}

	// get the value itself
	uint32_t	value;
	if (clientsock->read(&value)!=sizeof(uint32_t)) {
		#ifdef SERVER_DEBUG
			debugPrint("connection",2,
					"getting binds failed: bad value");
		#endif
		return false;
	}

	// set the value
	bv->value.longval=((int32_t)value)*(negative?-1:1);

	#ifdef SERVER_DEBUG
		debugPrint("connection",4,bv->value.longval);
	#endif

	return true;
}

bool sqlrconnection::getDoubleBind(bindvar *bv) {

	#ifdef SERVER_DEBUG
		debugPrint("connection",4,"DOUBLE");
	#endif

	// get the value
	if (clientsock->read(&(bv->value.doubleval.value))!=sizeof(double)) {
		#ifdef SERVER_DEBUG
			debugPrint("connection",2,
					"getting binds failed: bad value");
		#endif
		return false;
	}

	// get the precision
	if (clientsock->read(&(bv->value.doubleval.precision))!=
						sizeof(uint32_t)) {
		#ifdef SERVER_DEBUG
			debugPrint("connection",2,
					"getting binds failed: bad precision");
		#endif
		return false;
	}

	// get the scale
	if (clientsock->read(&(bv->value.doubleval.scale))!=
						sizeof(uint32_t)) {
		#ifdef SERVER_DEBUG
			debugPrint("connection",2,
					"getting binds failed: bad scale");
		#endif
		return false;
	}

	#ifdef SERVER_DEBUG
		debugPrint("connection",4,bv->value.doubleval.value);
	#endif

	return true;
}

bool sqlrconnection::getLobBind(bindvar *bv) {

	#ifdef SERVER_DEBUG
		if (bv->type==BLOB_BIND) {
			debugPrint("connection",4,"BLOB");
		}
		if (bv->type==CLOB_BIND) {
			debugPrint("connection",4,"CLOB");
		}
	#endif

	// get the size of the value
	if (!getBindSize(bv,lobbindvaluelength)) {
		return false;
	}

	// allocate space to store the value
	bv->value.stringval=(char *)bindpool->malloc(bv->valuesize+1);

	// get the bind value
	if ((uint32_t)(clientsock->read(bv->value.stringval,
			bv->valuesize))!=(uint32_t)(bv->valuesize)) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,
				"getting binds failed: bad value");
		#endif
		return false;
	}

	// It shouldn't hurt to NULL-terminate the lob because the actual size
	// (which doesn't include the NULL terminator) should be used when
	// binding.
	bv->value.stringval[bv->valuesize]=(char)NULL;
	bv->isnull=nonNullBindValue();

	#ifdef SERVER_DEBUG
		/*if (bv->type==BLOB_BIND) {
			debugPrintBlob(bv->value.stringval,bv->valuesize);
		}
		if (bv->type==CLOB_BIND) {
			debugPrintClob(bv->value.stringval,bv->valuesize);
		}*/
	#endif

	return true;
}
