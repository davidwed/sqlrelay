// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

int	sqlrconnection::getInputBinds() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"getting input binds...");
	#endif

	// get the number of input bind variable/values
	if (!getBindVarCount(&(cur[currentcur]->inbindcount))) {
		return 0;
	}
	
	// fill the buffers
	for (int i=0; i<cur[currentcur]->inbindcount && i<MAXVAR; i++) {

		bindvar	*bv=&(cur[currentcur]->inbindvars[i]);

		// get the variable name and type
		if (!(getBindVarName(bv) && getBindVarType(bv))) {
			return 0;
		}

		// get the value
		if (bv->type==NULL_BIND) {
			getNullBind(bv);
		} else if (bv->type==STRING_BIND) {
			if (!getStringBind(bv)) {
				return 0;
			}
		} else if (bv->type==LONG_BIND) {
			if (!getLongBind(bv)) {
				return 0;
			}
		} else if (bv->type==DOUBLE_BIND) {
			if (!getDoubleBind(bv)) {
				return 0;
			}
		} else if (bv->type==BLOB_BIND || bv->type==CLOB_BIND) {
			if (!getLobBind(bv)) {
				return 0;
			}
		}		  
	}

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"done getting input binds");
	#endif
	return 1;
}

int	sqlrconnection::getOutputBinds() {

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"getting output binds...");
	#endif

	// get the number of output bind variable/values
	if (!getBindVarCount(&(cur[currentcur]->outbindcount))) {
		return 0;
	}

	// fill the buffers
	for (int i=0; i<cur[currentcur]->outbindcount && i<MAXVAR; i++) {

		bindvar	*bv=&(cur[currentcur]->outbindvars[i]);

		// get the variable name and type
		if (!(getBindVarName(bv) && getBindVarType(bv))) {
			return 0;
		}

		// get the size of the value
		if (bv->type==STRING_BIND) {
			if (!getBindSize(bv,STRINGBINDVALUELENGTH)) {
				return 0;
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
			if (!getBindSize(bv,LOBBINDVALUELENGTH)) {
				return 0;
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
			short	curs=findAvailableCursor();
			if (curs==-1) {
				// FIXME: set error here
				return 0;
			}
			bv->value.cursorid=curs;
		}
	}

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"done getting output binds");
	#endif
	return 1;
}

int	sqlrconnection::getBindVarCount(unsigned short *count) {

	// get the number of input bind variable/values
	if (clientsock->read(count)!=sizeof(unsigned short)) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,
			"getting binds failed: client sent bad bind count size");
		#endif
		return 0;
	}

	// bounds checking
	if (*count>MAXVAR) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,
			"getting binds failed: client tried to send too many binds");
		#endif
		return 0;
	}

	return 1;
}

int	sqlrconnection::getBindVarName(bindvar *bv) {

	unsigned short	bindnamesize;

	// get the variable name size
	if (clientsock->read(&bindnamesize)!=sizeof(unsigned short)) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,
			"getting binds failed: bad variable name length size");
		#endif
		return 0;
	}

	// bounds checking
	if (bindnamesize>BINDVARLENGTH) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,
			"getting binds failed: bad variable name length");
		#endif
		return 0;
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
		return 0;
	}
	bv->variable[bindnamesize+1]=(char)NULL;

	#ifdef SERVER_DEBUG
	debugPrint("connection",4,bv->variable);
	#endif

	return 1;
}

int	sqlrconnection::getBindVarType(bindvar *bv) {

	// get the type
        unsigned short type;
	if (clientsock->read((unsigned short *)&type)!=sizeof(unsigned short)) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,
				"getting binds failed: bad type size");
		#endif
		return 0;
	}
	bv->type=(bindtype)type;
	
	return 1;
}

int	sqlrconnection::getBindSize(bindvar *bv, unsigned long maxsize) {

	// get the size of the value
	if (clientsock->read(&(bv->valuesize))!=sizeof(unsigned long)) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",
			2,"getting binds failed: bad value length size");
		#endif
		return 0;
	}

	// bounds checking
	if (bv->valuesize>maxsize) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,
				"getting binds failed: bad value length");
		debugPrint("connection",3,(long)bv->valuesize);
		#endif
		return 0;
	}

	return 1;
}

void	sqlrconnection::getNullBind(bindvar *bv) {

	#ifdef SERVER_DEBUG
		debugPrint("connection",4,"NULL");
	#endif

	bv->value.stringval=(char *)bindpool->malloc(1);
	bv->value.stringval[0]=(char)NULL;
	bv->valuesize=0;
	bv->isnull=nullBindValue();
}

int	sqlrconnection::getStringBind(bindvar *bv) {

	// get the size of the value
	if (!getBindSize(bv,STRINGBINDVALUELENGTH)) {
		return 0;
	}

	// allocate space to store the value
	bv->value.stringval=(char *)bindpool->malloc(bv->valuesize+1);

	#ifdef SERVER_DEBUG
		debugPrint("connection",4,"STRING");
	#endif

	// get the bind value
	if ((unsigned long)(clientsock->read(bv->value.stringval,
			bv->valuesize))!=(unsigned long)(bv->valuesize)) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,"getting binds failed: bad value");
		#endif
		return 0;
	}
	bv->value.stringval[bv->valuesize]=(char)NULL;
	bv->isnull=nonNullBindValue();

	#ifdef SERVER_DEBUG
		debugPrint("connection",4,bv->value.stringval);
	#endif

	return 1;
}

int	sqlrconnection::getLongBind(bindvar *bv) {

	#ifdef SERVER_DEBUG
		debugPrint("connection",4,"LONG");
	#endif

	// get positive/negative
	char		negative;
	if (clientsock->read(&negative)!=sizeof(char)) {
		#ifdef SERVER_DEBUG
			debugPrint("connection",2,
				"getting binds failed: bad positive/negative");
		#endif
		return 0;
	}

	// get the value itself
	unsigned long	value;
	if (clientsock->read(&value)!=sizeof(unsigned long)) {
		#ifdef SERVER_DEBUG
			debugPrint("connection",2,
					"getting binds failed: bad value");
		#endif
		return 0;
	}

	// set the value
	bv->value.longval=((long)value)*(negative?-1:1);

	#ifdef SERVER_DEBUG
		debugPrint("connection",4,bv->value.longval);
	#endif

	return 1;
}

int	sqlrconnection::getDoubleBind(bindvar *bv) {

	#ifdef SERVER_DEBUG
		debugPrint("connection",4,"DOUBLE");
	#endif

	// get the value
	if (clientsock->read(&(bv->value.doubleval.value))!=sizeof(double)) {
		#ifdef SERVER_DEBUG
			debugPrint("connection",2,
					"getting binds failed: bad value");
		#endif
		return 0;
	}

	// get the precision
	if (clientsock->read(&(bv->value.doubleval.precision))!=
						sizeof(unsigned short)) {
		#ifdef SERVER_DEBUG
			debugPrint("connection",2,
					"getting binds failed: bad precision");
		#endif
		return 0;
	}

	// get the scale
	if (clientsock->read(&(bv->value.doubleval.scale))!=
						sizeof(unsigned short)) {
		#ifdef SERVER_DEBUG
			debugPrint("connection",2,
					"getting binds failed: bad scale");
		#endif
		return 0;
	}

	#ifdef SERVER_DEBUG
		debugPrint("connection",4,bv->value.doubleval.value);
	#endif

	return 1;
}

int	sqlrconnection::getLobBind(bindvar *bv) {

	#ifdef SERVER_DEBUG
		if (bv->type==BLOB_BIND) {
			debugPrint("connection",4,"BLOB");
		}
		if (bv->type==CLOB_BIND) {
			debugPrint("connection",4,"CLOB");
		}
	#endif

	// get the size of the value
	if (!getBindSize(bv,LOBBINDVALUELENGTH)) {
		return 0;
	}

	// allocate space to store the value
	bv->value.stringval=(char *)bindpool->malloc(bv->valuesize+1);

	// get the bind value
	if ((unsigned long)(clientsock->read(bv->value.stringval,
			bv->valuesize))!=(unsigned long)(bv->valuesize)) {
		#ifdef SERVER_DEBUG
		debugPrint("connection",2,
				"getting binds failed: bad value");
		#endif
		return 0;
	}

	// It shouldn't hurt to NULL-terminate the lob because the actual size
	// (which doesn't include the NULL terminator) should be used when
	// binding.
	bv->value.stringval[bv->valuesize]=(char)NULL;
	bv->isnull=nonNullBindValue();

	#ifdef SERVER_DEBUG
		if (bv->type==BLOB_BIND) {
			debugPrintBlob(bv->value.stringval,bv->valuesize);
		}
		if (bv->type==CLOB_BIND) {
			debugPrintClob(bv->value.stringval,bv->valuesize);
		}
	#endif

	return 1;
}
