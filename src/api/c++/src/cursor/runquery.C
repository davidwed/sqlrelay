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

		if (processResultSet(rsbuffersize-1)) {
			return true;
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
		sqlrc->debugPrint((long)querylen);
		sqlrc->debugPrint("\n");
		sqlrc->debugPreEnd();
	}

	// send the query to the server.
	if (!reexecute) {

		// tell the server we're sending a query
		sqlrc->cs->write((unsigned short)NEW_QUERY);

		if (havecursorid) {

			// tell the server we already have a cursor
			sqlrc->cs->write((unsigned short)DONT_NEED_NEW_CURSOR);

			// send the cursor id to the server
			sqlrc->cs->write((unsigned short)cursorid);

			if (sqlrc->debug) {
				sqlrc->debugPreStart();
				sqlrc->debugPrint("Requesting Cursor: ");
				sqlrc->debugPrint((long)cursorid);
				sqlrc->debugPrint("\n");
				sqlrc->debugPreEnd();
			}

		} else {

			// tell the server we need a cursor
			sqlrc->cs->write((unsigned short)NEED_NEW_CURSOR);

			if (sqlrc->debug) {
				sqlrc->debugPreStart();
				sqlrc->debugPrint("Requesting a new cursor.\n");
				sqlrc->debugPreEnd();
			}
		}

		// send the query
		sqlrc->cs->write((unsigned long)querylen);
		sqlrc->cs->write(query,querylen);

	} else {

		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("Requesting re-execution of ");
			sqlrc->debugPrint("previous query.");
			sqlrc->debugPrint("\n");
			sqlrc->debugPrint("Requesting Cursor: ");
			sqlrc->debugPrint((long)cursorid);
			sqlrc->debugPrint("\n");
			sqlrc->debugPreEnd();
		}

		// tell the server we're sending a query
		sqlrc->cs->write((unsigned short)REEXECUTE_QUERY);

		// send the cursor id to the server
		sqlrc->cs->write((unsigned short)cursorid);
	}

	return true;
}

void sqlrcursor::sendInputBinds() {

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Sending ");
		sqlrc->debugPrint((long)inbindcount);
		sqlrc->debugPrint(" Input Bind Variables:\n");
		sqlrc->debugPreEnd();
	}

	// write the input bind variables/values to the server.
	sqlrc->cs->write(inbindcount);
	unsigned long	size;
	unsigned short	count=inbindcount;
	for (unsigned short i=0; i<count; i++) {

		// don't send anything if the send flag is turned off
		if (!inbindvars[i].send) {
			count++;
			continue;
		}

		// send the variable
		size=(unsigned long)charstring::length(
					inbindvars[i].variable);
		sqlrc->cs->write((unsigned short)size);
		sqlrc->cs->write(inbindvars[i].variable,(size_t)size);
		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint(inbindvars[i].variable);
			sqlrc->debugPrint("(");
			sqlrc->debugPrint((long)size);
		}

		// send the type
		sqlrc->cs->write((unsigned short)inbindvars[i].type);

		// send the value
		if (inbindvars[i].type==NULL_BIND) {

			if (sqlrc->debug) {
				sqlrc->debugPrint(":NULL)\n");
				sqlrc->debugPreEnd();
			}

		} else if (inbindvars[i].type==STRING_BIND) {

			sqlrc->cs->write((unsigned long)
						inbindvars[i].valuesize);
			if (inbindvars[i].valuesize>0) {
				sqlrc->cs->write(inbindvars[i].
					value.stringval,
					(size_t)inbindvars[i].valuesize);
			}

			if (sqlrc->debug) {
				sqlrc->debugPrint(":STRING)=");
				sqlrc->debugPrint(inbindvars[i].
							value.stringval);
				sqlrc->debugPrint("(");
				sqlrc->debugPrint((long)inbindvars[i].
							valuesize);
				sqlrc->debugPrint(")");
				sqlrc->debugPrint("\n");
				sqlrc->debugPreEnd();
			}

		} else if (inbindvars[i].type==LONG_BIND) {

			char	negative=inbindvars[i].value.longval<0?1:0;
			sqlrc->cs->write(negative);
			sqlrc->cs->write((unsigned long)
					(inbindvars[i].value.longval*
					 		((negative)?-1:1)));

			if (sqlrc->debug) {
				sqlrc->debugPrint(":LONG)=");
				sqlrc->debugPrint((long)inbindvars[i].
							value.longval);
				sqlrc->debugPrint("\n");
				sqlrc->debugPreEnd();
			}

		} else if (inbindvars[i].type==DOUBLE_BIND) {

			sqlrc->cs->write((double)inbindvars[i].value.
							doubleval.value);
			sqlrc->cs->write((unsigned short)inbindvars[i].value.
							doubleval.precision);
			sqlrc->cs->write((unsigned short)inbindvars[i].value.
							doubleval.scale);

			if (sqlrc->debug) {
				sqlrc->debugPrint(":DOUBLE)=");
				sqlrc->debugPrint(inbindvars[i].value.
							doubleval.value);
				sqlrc->debugPrint(":");
				sqlrc->debugPrint((long)inbindvars[i].value.
							doubleval.precision);
				sqlrc->debugPrint(",");
				sqlrc->debugPrint((long)inbindvars[i].value.
							doubleval.scale);
				sqlrc->debugPrint("\n");
				sqlrc->debugPreEnd();
			}

		} else if (inbindvars[i].type==BLOB_BIND ||
				inbindvars[i].type==CLOB_BIND) {

			sqlrc->cs->write((unsigned long)
						inbindvars[i].valuesize);
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
				sqlrc->debugPrint((long)inbindvars[i].
							valuesize);
				sqlrc->debugPrint(")");
				sqlrc->debugPrint("\n");
				sqlrc->debugPreEnd();
			}
		}
	}
}

void sqlrcursor::sendOutputBinds() {

	if (sqlrc->debug) {
		sqlrc->debugPreStart();
		sqlrc->debugPrint("Sending Output Bind Variables:\n");
		sqlrc->debugPreEnd();
	}

	// write the output bind variables to the server.
	sqlrc->cs->write(outbindcount);
	unsigned short	size;
	unsigned short	count=outbindcount;
	for (unsigned short i=0; i<count; i++) {

		// don't send anything if the send flag is turned off
		if (!outbindvars[i].send) {
			count++;
			continue;
		}

		// send the variable, type and size that the buffer needs to be
		size=(unsigned short)charstring::length(
					outbindvars[i].variable);
		sqlrc->cs->write((unsigned short)size);
		sqlrc->cs->write(outbindvars[i].variable,(size_t)size);
		sqlrc->cs->write((unsigned short)outbindvars[i].type);
		if (outbindvars[i].type!=CURSOR_BIND) {
			sqlrc->cs->write((unsigned long)
					outbindvars[i].valuesize);
		}

		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint(outbindvars[i].variable);
			if (outbindvars[i].type!=CURSOR_BIND) {
				sqlrc->debugPrint("(");
				sqlrc->debugPrint((long)outbindvars[i].
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
		sqlrc->cs->write((unsigned short)SEND_COLUMN_INFO);
	} else {
		if (sqlrc->debug) {
			sqlrc->debugPreStart();
			sqlrc->debugPrint("Send Column Info: no\n");
			sqlrc->debugPreEnd();
		}
		sqlrc->cs->write((unsigned short)DONT_SEND_COLUMN_INFO);
	}
}
