// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

bool sqlrconnection_svr::getInputBinds(sqlrcursor_svr *cursor) {

	dbgfile.debugPrint("connection",2,"getting input binds...");

	// get the number of input bind variable/values
	if (!getBindVarCount(&(cursor->inbindcount))) {
		return false;
	}
	
	// fill the buffers
	for (uint16_t i=0; i<cursor->inbindcount && i<MAXVAR; i++) {

		bindvar_svr	*bv=&(cursor->inbindvars[i]);

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
		} else if (bv->type==INTEGER_BIND) {
			if (!getIntegerBind(bv)) {
				return false;
			}
		} else if (bv->type==DOUBLE_BIND) {
			if (!getDoubleBind(bv)) {
				return false;
			}
		} else if (bv->type==DATE_BIND) {
			if (!getDateBind(bv)) {
				return false;
			}
		} else if (bv->type==BLOB_BIND) {
			// can't fake blob binds
			cursor->fakeinputbindsforthisquery=false;
			if (!getLobBind(bv)) {
				return false;
			}
		} else if (bv->type==CLOB_BIND) {
			if (!getLobBind(bv)) {
				return false;
			}
		}		  
	}

	dbgfile.debugPrint("connection",2,"done getting input binds");
	return true;
}

bool sqlrconnection_svr::getOutputBinds(sqlrcursor_svr *cursor) {

	dbgfile.debugPrint("connection",2,"getting output binds...");

	// get the number of output bind variable/values
	if (!getBindVarCount(&(cursor->outbindcount))) {
		return false;
	}

	// fill the buffers
	for (uint16_t i=0; i<cursor->outbindcount && i<MAXVAR; i++) {

		bindvar_svr	*bv=&(cursor->outbindvars[i]);

		// get the variable name and type
		if (!(getBindVarName(bv) && getBindVarType(bv))) {
			return false;
		}

		// get the size of the value
		if (bv->type==STRING_BIND) {
			if (!getBindSize(bv,maxstringbindvaluelength)) {
				return false;
			}
			// This must be a calloc because oracle8 gets angry if
			// these aren't initialized to NULL's.  It's possible
			// that just the first character needs to be NULL, but
			// for now I'm just going to go ahead and use calloc
			bv->value.stringval=
				(char *)bindpool->calloc(bv->valuesize+1);
			dbgfile.debugPrint("connection",4,"STRING");
		} else if (bv->type==INTEGER_BIND) {
			dbgfile.debugPrint("connection",4,"INTEGER");
		} else if (bv->type==DOUBLE_BIND) {
			dbgfile.debugPrint("connection",4,"DOUBLE");
			// these don't typically get set, but they get used
			// when building debug strings, so we need to
			// initialize them
			bv->value.doubleval.precision=0;
			bv->value.doubleval.scale=0;
		} else if (bv->type==DATE_BIND) {
			dbgfile.debugPrint("connection",4,"DATE");
			bv->value.dateval.year=0;
			bv->value.dateval.month=0;
			bv->value.dateval.day=0;
			bv->value.dateval.hour=0;
			bv->value.dateval.minute=0;
			bv->value.dateval.second=0;
			bv->value.dateval.tz=NULL;
			// allocate enough space to store the date/time string
			// 64 bytes ought to be enough
			bv->value.dateval.buffersize=64;
			bv->value.dateval.buffer=(char *)bindpool->malloc(
						bv->value.dateval.buffersize);
		} else if (bv->type==BLOB_BIND || bv->type==CLOB_BIND) {
			if (!getBindSize(bv,maxlobbindvaluelength)) {
				return false;
			}
			if (bv->type==BLOB_BIND) {
				dbgfile.debugPrint("connection",4,"BLOB");
			} else if (bv->type==CLOB_BIND) {
				dbgfile.debugPrint("connection",4,"CLOB");
			}
		} else if (bv->type==CURSOR_BIND) {
			dbgfile.debugPrint("connection",4,"CURSOR");
			sqlrcursor_svr	*curs=findAvailableCursor();
			if (!curs) {
				// FIXME: set error here
				return false;
			}
			curs->busy=true;
			bv->value.cursorid=curs->id;
		}

		// init the null indicator
		bv->isnull=nonNullBindValue();
	}

	dbgfile.debugPrint("connection",2,"done getting output binds");
	return true;
}

bool sqlrconnection_svr::getBindVarCount(uint16_t *count) {

	// get the number of input bind variable/values
	if (clientsock->read(count,idleclienttimeout,0)!=sizeof(uint16_t)) {
		dbgfile.debugPrint("connection",2,
			"getting binds failed: client sent bad bind count size");
		return false;
	}

	// bounds checking
	if (*count>MAXVAR) {
		dbgfile.debugPrint("connection",2,
			"getting binds failed: client tried to send too many binds:");
		dbgfile.debugPrint("connection",3,(int32_t)*count);
		return false;
	}

	return true;
}

bool sqlrconnection_svr::getBindVarName(bindvar_svr *bv) {

	uint16_t	bindnamesize;

	// get the variable name size
	if (clientsock->read(&bindnamesize,
				idleclienttimeout,0)!=sizeof(uint16_t)) {
		dbgfile.debugPrint("connection",2,
			"getting binds failed: bad variable name length size");
		return false;
	}

	// bounds checking
	if (bindnamesize>BINDVARLENGTH) {
		dbgfile.debugPrint("connection",2,
			"getting binds failed: bad variable name length");
		return false;
	}

	// get the variable name
	bv->variablesize=bindnamesize+1;
	bv->variable=(char *)bindmappingspool->malloc(bv->variablesize+2);
	bv->variable[0]=bindVariablePrefix();
	if (clientsock->read(bv->variable+1,bindnamesize,
					idleclienttimeout,0)!=bindnamesize) {
		dbgfile.debugPrint("connection",2,
			"getting binds failed: bad variable name");
		return false;
	}
	bv->variable[bindnamesize+1]='\0';

	dbgfile.debugPrint("connection",4,bv->variable);

	return true;
}

bool sqlrconnection_svr::getBindVarType(bindvar_svr *bv) {

	// get the type
        uint16_t type;
	if (clientsock->read(&type,idleclienttimeout,0)!=sizeof(uint16_t)) {
		dbgfile.debugPrint("connection",2,
				"getting binds failed: bad type size");
		return false;
	}
	bv->type=(bindtype)type;
	
	return true;
}

bool sqlrconnection_svr::getBindSize(bindvar_svr *bv, uint32_t maxsize) {

	// get the size of the value
	if (clientsock->read(&(bv->valuesize),
				idleclienttimeout,0)!=sizeof(uint32_t)) {
		dbgfile.debugPrint("connection",
			2,"getting binds failed: bad value length size");
		return false;
	}

	// bounds checking
	/*if (bv->valuesize>maxsize) {
		dbgfile.debugPrint("connection",2,
				"getting binds failed: bad value length");
		dbgfile.debugPrint("connection",3,bv->valuesize);
		return false;
	}*/

	return true;
}

void sqlrconnection_svr::getNullBind(bindvar_svr *bv) {

	dbgfile.debugPrint("connection",4,"NULL");

	bv->value.stringval=(char *)bindpool->malloc(1);
	bv->value.stringval[0]='\0';
	bv->valuesize=0;
	bv->isnull=nullBindValue();
}

bool sqlrconnection_svr::getStringBind(bindvar_svr *bv) {

	// get the size of the value
	if (!getBindSize(bv,maxstringbindvaluelength)) {
		return false;
	}

	// allocate space to store the value
	bv->value.stringval=(char *)bindpool->malloc(bv->valuesize+1);

	dbgfile.debugPrint("connection",4,"STRING");

	// get the bind value
	if ((uint32_t)(clientsock->read(bv->value.stringval,
					bv->valuesize,
					idleclienttimeout,0))!=
						(uint32_t)(bv->valuesize)) {
		dbgfile.debugPrint("connection",2,
					"getting binds failed: bad value");
		return false;
	}
	bv->value.stringval[bv->valuesize]='\0';
	bv->isnull=nonNullBindValue();

	dbgfile.debugPrint("connection",4,bv->value.stringval);

	return true;
}

bool sqlrconnection_svr::getIntegerBind(bindvar_svr *bv) {

	dbgfile.debugPrint("connection",4,"INTEGER");

	// get the value itself
	uint64_t	value;
	if (clientsock->read(&value,idleclienttimeout,0)!=sizeof(uint64_t)) {
		dbgfile.debugPrint("connection",2,
					"getting binds failed: bad value");
		return false;
	}

	// set the value
	bv->value.integerval=(int64_t)value;

	dbgfile.debugPrint("connection",4,(int32_t)bv->value.integerval);

	return true;
}

bool sqlrconnection_svr::getDoubleBind(bindvar_svr *bv) {

	dbgfile.debugPrint("connection",4,"DOUBLE");

	// get the value
	if (clientsock->read(&(bv->value.doubleval.value),
				idleclienttimeout,0)!=sizeof(double)) {
		dbgfile.debugPrint("connection",2,
					"getting binds failed: bad value");
		return false;
	}

	// get the precision
	if (clientsock->read(&(bv->value.doubleval.precision),
				idleclienttimeout,0)!=sizeof(uint32_t)) {
		dbgfile.debugPrint("connection",2,
					"getting binds failed: bad precision");
		return false;
	}

	// get the scale
	if (clientsock->read(&(bv->value.doubleval.scale),
				idleclienttimeout,0)!=sizeof(uint32_t)) {
		dbgfile.debugPrint("connection",2,
					"getting binds failed: bad scale");
		return false;
	}

	dbgfile.debugPrint("connection",4,bv->value.doubleval.value);

	return true;
}

bool sqlrconnection_svr::getDateBind(bindvar_svr *bv) {

	dbgfile.debugPrint("connection",4,"DATE");

	uint16_t	temp;

	// get the year
	if (clientsock->read(&temp,idleclienttimeout,0)!=sizeof(uint16_t)) {
		dbgfile.debugPrint("connection",2,
					"getting binds failed: bad year");
		return false;
	}
	bv->value.dateval.year=(int16_t)temp;

	// get the month
	if (clientsock->read(&temp,idleclienttimeout,0)!=sizeof(uint16_t)) {
		dbgfile.debugPrint("connection",2,
					"getting binds failed: bad month");
		return false;
	}
	bv->value.dateval.month=(int16_t)temp;

	// get the day
	if (clientsock->read(&temp,idleclienttimeout,0)!=sizeof(uint16_t)) {
		dbgfile.debugPrint("connection",2,
					"getting binds failed: bad day");
		return false;
	}
	bv->value.dateval.day=(int16_t)temp;

	// get the hour
	if (clientsock->read(&temp,idleclienttimeout,0)!=sizeof(uint16_t)) {
		dbgfile.debugPrint("connection",2,
					"getting binds failed: bad month");
		return false;
	}
	bv->value.dateval.hour=(int16_t)temp;

	// get the minute
	if (clientsock->read(&temp,idleclienttimeout,0)!=sizeof(uint16_t)) {
		dbgfile.debugPrint("connection",2,
					"getting binds failed: bad month");
		return false;
	}
	bv->value.dateval.minute=(int16_t)temp;

	// get the second
	if (clientsock->read(&temp,idleclienttimeout,0)!=sizeof(uint16_t)) {
		dbgfile.debugPrint("connection",2,
					"getting binds failed: bad month");
		return false;
	}
	bv->value.dateval.second=(int16_t)temp;

	// get the size of the time zone
	uint16_t	length;
	if (clientsock->read(&length,idleclienttimeout,0)!=sizeof(uint16_t)) {
		return false;
	}

	// allocate space to store the time zone
	bv->value.dateval.tz=(char *)bindpool->malloc(length+1);

	// get the time zone
	if ((uint16_t)(clientsock->read(bv->value.dateval.tz,length,
					idleclienttimeout,0))!=length) {
		dbgfile.debugPrint("connection",2,
					"getting binds failed: bad tz");
		return false;
	}
	bv->value.dateval.tz[length]='\0';

	// allocate enough space to store the date/time string
	// 64 bytes ought to be enough
	bv->value.dateval.buffersize=64;
	bv->value.dateval.buffer=(char *)bindpool->malloc(
					bv->value.dateval.buffersize);

	if (dbgfile.debugEnabled()) {
		stringbuffer	str;
		str.append(bv->value.dateval.year)->append("-");
		str.append(bv->value.dateval.month)->append("-");
		str.append(bv->value.dateval.day)->append(" ");
		str.append(bv->value.dateval.hour)->append(":");
		str.append(bv->value.dateval.minute)->append(":");
		str.append(bv->value.dateval.second)->append(" ");
		str.append(bv->value.dateval.tz);
		dbgfile.debugPrint("connection",4,str.getString());
	}

	return true;
}

bool sqlrconnection_svr::getLobBind(bindvar_svr *bv) {

	if (bv->type==BLOB_BIND) {
		dbgfile.debugPrint("connection",4,"BLOB");
	}
	if (bv->type==CLOB_BIND) {
		dbgfile.debugPrint("connection",4,"CLOB");
	}

	// get the size of the value
	if (!getBindSize(bv,maxlobbindvaluelength)) {
		return false;
	}

	// allocate space to store the value
	// (the +1 is to store the NULL-terminator for CLOBS)
	bv->value.stringval=(char *)bindpool->malloc(bv->valuesize+1);

	// get the bind value
	if ((uint32_t)(clientsock->read(bv->value.stringval,
					bv->valuesize,
					idleclienttimeout,0))!=
						(uint32_t)(bv->valuesize)) {
		dbgfile.debugPrint("connection",2,
				"getting binds failed: bad value");
		return false;
	}

	// It shouldn't hurt to NULL-terminate the lob because the actual size
	// (which doesn't include the NULL terminator) should be used when
	// binding.
	bv->value.stringval[bv->valuesize]='\0';
	bv->isnull=nonNullBindValue();

	/*if (bv->type==BLOB_BIND) {
		dbgfile.debugPrintBlob(bv->value.stringval,bv->valuesize);
	}
	if (bv->type==CLOB_BIND) {
		dbgfile.debugPrintClob(bv->value.stringval,bv->valuesize);
	}*/

	return true;
}
