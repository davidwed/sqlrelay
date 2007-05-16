// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <config.h>
#include <sqlrelay/sqlrclient.h>
#include <defines.h>

bool sqlrcursor::runQuery(const char *query) {

	// send the query
	if (sendQueryInternal(query)) {

		sendInputBinds();
		sendOutputBinds();
		sendGetColumnInfo();

		sqlrc->flushWriteBuffer();

		if (rsbuffersize) {
			if (processResultSet(false,rsbuffersize-1)) {
				return true;
			}
		} else {
			if (processResultSet(true,0)) {
				return true;
			}
		}
	}
	return false;
}

bool sqlrcursor::sendQueryInternal(const char *query) {

	// if the first 8 characters of the query are "-- debug" followed
	// by a return, then set debugging on
	if (!charstring::compare(query,"-- debug\n",9)) {
		sqlrc->debugOn();
	}

	if (!endofresultset) {
		abortResultSet();
	}
	clearResultSet();

	if (!sqlrc->openSession()) {
		return false;
	}

	cached=false;
	endofresultset=false;

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Sending Query:");
		sqlrc->debugPrint("\n");
		sqlrc->debugPrint(query);
		sqlrc->debugPrint("\n");
		sqlrc->debugPrint("Length: ");
		sqlrc->debugPrint((int64_t)querylen);
		sqlrc->debugPrint("\n");
		sqlrc->debugPreEnd();
	}

	// send the query to the server.
	if (!reexecute) {

		// tell the server we're sending a query
		sqlrc->cs->write((uint16_t)NEW_QUERY);

		if (havecursorid) {

			// tell the server we already have a cursor
			sqlrc->cs->write((uint16_t)DONT_NEED_NEW_CURSOR);

			// send the cursor id to the server
			sqlrc->cs->write(cursorid);

			if (sqlrc->debug) {
				sqlrc->debugPreStart();
				sqlrc->debugPrint("Requesting Cursor: ");
				sqlrc->debugPrint((int64_t)cursorid);
				sqlrc->debugPrint("\n");
				sqlrc->debugPreEnd();
			}

		} else {

			// tell the server we need a cursor
			sqlrc->cs->write((uint16_t)NEED_NEW_CURSOR);

			if (sqlrc->debug) {
				sqlrc->debugPreStart();
				sqlrc->debugPrint("Requesting a new cursor.\n");
				sqlrc->debugPreEnd();
			}
		}

		// send the query
		sqlrc->cs->write(querylen);
		sqlrc->cs->write(query,querylen);

	} else {

		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("Requesting re-execution of ");
			sqlrc->debugPrint("previous query.");
			sqlrc->debugPrint("\n");
			sqlrc->debugPrint("Requesting Cursor: ");
			sqlrc->debugPrint((int64_t)cursorid);
			sqlrc->debugPrint("\n");
			sqlrc->debugPreEnd();
		}

		// tell the server we're sending a query
		sqlrc->cs->write((uint16_t)REEXECUTE_QUERY);

		// send the cursor id to the server
		sqlrc->cs->write(cursorid);
	}

	return true;
}

void sqlrcursor::sendInputBinds() {

	// adjust inbindcount
	uint16_t	count=inbindcount;
	for (uint16_t i=0; i<count; i++) {
		if (!inbindvars[i].send) {
			inbindcount--;
		}
	}

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Sending ");
		sqlrc->debugPrint((int64_t)inbindcount);
		sqlrc->debugPrint(" Input Bind Variables:\n");
		sqlrc->debugPreEnd();
	}

	// write the input bind variables/values to the server.
	sqlrc->cs->write(inbindcount);
	count=inbindcount;
	uint16_t	size;
	for (uint16_t i=0; i<count; i++) {

		// don't send anything if the send flag is turned off
		if (!inbindvars[i].send) {
			count++;
			continue;
		}

		// send the variable
		size=charstring::length(inbindvars[i].variable);
		sqlrc->cs->write(size);
		sqlrc->cs->write(inbindvars[i].variable,(size_t)size);
		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint(inbindvars[i].variable);
			sqlrc->debugPrint("(");
			sqlrc->debugPrint((int64_t)size);
		}

		// send the type
		sqlrc->cs->write((uint16_t)inbindvars[i].type);

		// send the value
		if (inbindvars[i].type==NULL_BIND) {

			if (sqlrc->debug) {
				sqlrc->debugPrint(":NULL)\n");
				sqlrc->debugPreEnd();
			}

		} else if (inbindvars[i].type==STRING_BIND) {

			sqlrc->cs->write(inbindvars[i].valuesize);
			if (inbindvars[i].valuesize>0) {
				sqlrc->cs->write(inbindvars[i].value.stringval,
					(size_t)inbindvars[i].valuesize);
			}

			if (sqlrc->debug) {
				sqlrc->debugPrint(":STRING)=");
				sqlrc->debugPrint(inbindvars[i].
							value.stringval);
				sqlrc->debugPrint("(");
				sqlrc->debugPrint((int64_t)inbindvars[i].
								valuesize);
				sqlrc->debugPrint(")");
				sqlrc->debugPrint("\n");
				sqlrc->debugPreEnd();
			}

		} else if (inbindvars[i].type==INTEGER_BIND) {

			sqlrc->cs->write((uint64_t)inbindvars[i].
							value.integerval);

			if (sqlrc->debug) {
				sqlrc->debugPrint(":LONG)=");
				sqlrc->debugPrint((int64_t)inbindvars[i].
							value.integerval);
				sqlrc->debugPrint("\n");
				sqlrc->debugPreEnd();
			}

		} else if (inbindvars[i].type==DOUBLE_BIND) {

			sqlrc->cs->write((double)inbindvars[i].value.
							doubleval.value);
			sqlrc->cs->write(inbindvars[i].value.
							doubleval.precision);
			sqlrc->cs->write(inbindvars[i].value.
							doubleval.scale);

			if (sqlrc->debug) {
				sqlrc->debugPrint(":DOUBLE)=");
				sqlrc->debugPrint(inbindvars[i].value.
							doubleval.value);
				sqlrc->debugPrint(":");
				sqlrc->debugPrint((int64_t)inbindvars[i].value.
							doubleval.precision);
				sqlrc->debugPrint(",");
				sqlrc->debugPrint((int64_t)inbindvars[i].value.
							doubleval.scale);
				sqlrc->debugPrint("\n");
				sqlrc->debugPreEnd();
			}

		} else if (inbindvars[i].type==BLOB_BIND ||
				inbindvars[i].type==CLOB_BIND) {

			sqlrc->cs->write(inbindvars[i].valuesize);
			if (inbindvars[i].valuesize>0) {
				sqlrc->cs->write(inbindvars[i].
					value.lobval,
					(size_t)inbindvars[i].valuesize);
			}

			if (sqlrc->debug) {
				if (inbindvars[i].type==BLOB_BIND) {
					sqlrc->debugPrint(":BLOB)=");
					sqlrc->debugPrintBlob(
						inbindvars[i].value.lobval,
						inbindvars[i].valuesize);
				} else if (inbindvars[i].type==CLOB_BIND) {
					sqlrc->debugPrint(":CLOB)=");
					sqlrc->debugPrintClob(
						inbindvars[i].value.lobval,
						inbindvars[i].valuesize);
				}
				sqlrc->debugPrint("(");
				sqlrc->debugPrint((int64_t)inbindvars[i].
								valuesize);
				sqlrc->debugPrint(")");
				sqlrc->debugPrint("\n");
				sqlrc->debugPreEnd();
			}
		}
	}
}

void sqlrcursor::sendOutputBinds() {

	// adjust outbindcount
	uint16_t	count=outbindcount;
	for (uint16_t i=0; i<count; i++) {
		if (!outbindvars[i].send) {
			outbindcount--;
		}
	}

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Sending Output Bind Variables:\n");
		sqlrc->debugPreEnd();
	}

	// write the output bind variables to the server.
	sqlrc->cs->write(outbindcount);
	uint16_t	size;
	count=outbindcount;
	for (uint16_t i=0; i<count; i++) {

		// don't send anything if the send flag is turned off
		if (!outbindvars[i].send) {
			count++;
			continue;
		}

		// send the variable, type and size that the buffer needs to be
		size=charstring::length(outbindvars[i].variable);
		sqlrc->cs->write(size);
		sqlrc->cs->write(outbindvars[i].variable,(size_t)size);
		sqlrc->cs->write((uint16_t)outbindvars[i].type);
		if (outbindvars[i].type==STRING_BIND ||
			outbindvars[i].type==BLOB_BIND ||
			outbindvars[i].type==CLOB_BIND ||
			outbindvars[i].type==NULL_BIND) {
			sqlrc->cs->write(outbindvars[i].valuesize);
		}

		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint(outbindvars[i].variable);
			if (outbindvars[i].type==STRING_BIND ||
				outbindvars[i].type==BLOB_BIND ||
				outbindvars[i].type==CLOB_BIND ||
				outbindvars[i].type==NULL_BIND) {
				sqlrc->debugPrint("(");
				sqlrc->debugPrint((int64_t)outbindvars[i].
								valuesize);
				sqlrc->debugPrint(")");
			}
			sqlrc->debugPrint("\n");
			sqlrc->debugPreEnd();
		}
	}
}

void sqlrcursor::sendGetColumnInfo() {

	if (sendcolumninfo==SEND_COLUMN_INFO) {
		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("Send Column Info: yes\n");
			sqlrc->debugPreEnd();
		}
		sqlrc->cs->write((uint16_t)SEND_COLUMN_INFO);
	} else {
		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("Send Column Info: no\n");
			sqlrc->debugPreEnd();
		}
		sqlrc->cs->write((uint16_t)DONT_SEND_COLUMN_INFO);
	}
}
