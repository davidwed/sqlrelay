// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection_svr::returnOutputBindValues(sqlrcursor_svr *cursor) {

	dbgfile.debugPrint("connection",2,"returning output bind values");
	dbgfile.debugPrint("connection",3,(int32_t)cursor->outbindcount);

	// run through the output bind values, sending them back
	for (uint16_t i=0; i<cursor->outbindcount; i++) {

		bindvar_svr	*bv=&(cursor->outbindvars[i]);

		if (dbgfile.debugEnabled()) {
			debugstr=new stringbuffer();
			debugstr->append(i);
			debugstr->append(":");
		}

		if (bindValueIsNull(bv->isnull)) {

			if (dbgfile.debugEnabled()) {
				debugstr->append("NULL");
			}

			clientsock->write((uint16_t)NULL_DATA);

		} else if (bv->type==BLOB_BIND) {

			if (dbgfile.debugEnabled()) {
				debugstr->append("BLOB:\n");
			}

			cursor->returnOutputBindBlob(i);

		} else if (bv->type==CLOB_BIND) {

			if (dbgfile.debugEnabled()) {
				debugstr->append("CLOB:\n");
			}

			cursor->returnOutputBindClob(i);

		} else if (bv->type==STRING_BIND) {

			if (dbgfile.debugEnabled()) {
				debugstr->append("STRING:\n");
				debugstr->append(bv->value.stringval);
			}

			clientsock->write((uint16_t)STRING_DATA);
			bv->valuesize=charstring::length(
						(char *)bv->value.stringval);
			clientsock->write(bv->valuesize);
			clientsock->write(bv->value.stringval,bv->valuesize);

		} else if (bv->type==INTEGER_BIND) {

			if (dbgfile.debugEnabled()) {
				debugstr->append("INTEGER:\n");
				debugstr->append(bv->value.integerval);
			}

			clientsock->write((uint16_t)INTEGER_DATA);
			clientsock->write((uint64_t)bv->value.integerval);

		} else if (bv->type==DOUBLE_BIND) {

			if (dbgfile.debugEnabled()) {
				debugstr->append("DOUBLE:\n");
				debugstr->append(bv->value.doubleval.value);
				debugstr->append("(");
				debugstr->append(bv->value.doubleval.precision);
				debugstr->append(",");
				debugstr->append(bv->value.doubleval.scale);
				debugstr->append(")");
			}

			clientsock->write((uint16_t)DOUBLE_DATA);
			clientsock->write(bv->value.doubleval.value);
			clientsock->write((uint32_t)bv->value.
						doubleval.precision);
			clientsock->write((uint32_t)bv->value.
						doubleval.scale);

		} else if (bv->type==DATE_BIND) {

			if (dbgfile.debugEnabled()) {
				debugstr->append("DATE:\n");
				debugstr->append(bv->value.dateval.year);
				debugstr->append("-");
				debugstr->append(bv->value.dateval.month);
				debugstr->append("-");
				debugstr->append(bv->value.dateval.day);
				debugstr->append(" ");
				debugstr->append(bv->value.dateval.hour);
				debugstr->append(":");
				debugstr->append(bv->value.dateval.minute);
				debugstr->append(":");
				debugstr->append(bv->value.dateval.second);
				debugstr->append(":");
				debugstr->append(bv->value.dateval.microsecond);
				debugstr->append(" ");
				debugstr->append(bv->value.dateval.tz);
			}

			clientsock->write((uint16_t)DATE_DATA);
			clientsock->write((uint16_t)bv->value.dateval.year);
			clientsock->write((uint16_t)bv->value.dateval.month);
			clientsock->write((uint16_t)bv->value.dateval.day);
			clientsock->write((uint16_t)bv->value.dateval.hour);
			clientsock->write((uint16_t)bv->value.dateval.minute);
			clientsock->write((uint16_t)bv->value.dateval.second);
			clientsock->write((uint32_t)bv->value.
							dateval.microsecond);
			uint16_t	length=charstring::length(
							bv->value.dateval.tz);
			clientsock->write(length);
			clientsock->write(bv->value.dateval.tz,length);

		} else if (bv->type==CURSOR_BIND) {

			if (dbgfile.debugEnabled()) {
				debugstr->append("CURSOR:\n");
				debugstr->append(bv->value.cursorid);
			}

			clientsock->write((uint16_t)CURSOR_DATA);
			clientsock->write(bv->value.cursorid);
		}

		if (dbgfile.debugEnabled()) {
			dbgfile.debugPrint("connection",3,
						debugstr->getString());
			delete debugstr;
		}
	}

	dbgfile.debugPrint("connection",2,"done returning output bind values");
}
