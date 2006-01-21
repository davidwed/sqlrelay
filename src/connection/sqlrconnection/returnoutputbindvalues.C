// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection_svr::returnOutputBindValues(sqlrcursor_svr *cursor) {

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"returning output bind values");
	debugPrint("connection",3,(int32_t)cursor->outbindcount);
	#endif

	// run through the output bind values, sending them back
	for (uint16_t i=0; i<cursor->outbindcount; i++) {

		bindvar_svr	*bv=&(cursor->outbindvars[i]);

		#ifdef SERVER_DEBUG
		debugstr=new stringbuffer();
		debugstr->append(i);
		debugstr->append(":");
		#endif

		if (bindValueIsNull(bv->isnull)) {

			#ifdef SERVER_DEBUG
			debugstr->append("NULL");
			#endif

			clientsock->write((uint16_t)NULL_DATA);

		} else if (bv->type==BLOB_BIND) {

			#ifdef SERVER_DEBUG
			debugstr->append("BLOB:\n");
			#endif

			cursor->returnOutputBindBlob(i);

		} else if (bv->type==CLOB_BIND) {

			#ifdef SERVER_DEBUG
			debugstr->append("CLOB:\n");
			#endif

			cursor->returnOutputBindClob(i);

		} else if (bv->type==STRING_BIND) {

			#ifdef SERVER_DEBUG
			debugstr->append("STRING:\n");
			debugstr->append(bv->value.stringval);
			#endif

			clientsock->write((uint16_t)STRING_DATA);
			bv->valuesize=charstring::length(
						(char *)bv->value.stringval);
			clientsock->write(bv->valuesize);
			clientsock->write(bv->value.stringval,bv->valuesize);

		} else if (bv->type==INTEGER_BIND) {

			#ifdef SERVER_DEBUG
			debugstr->append("INTEGER:\n");
			debugstr->append(bv->value.integerval);
			#endif

			clientsock->write((uint16_t)INTEGER_DATA);
			clientsock->write((uint64_t)bv->value.integerval);

		} else if (bv->type==DOUBLE_BIND) {

			#ifdef SERVER_DEBUG
			debugstr->append("DOUBLE:\n");
			debugstr->append(bv->value.doubleval.value);
			debugstr->append("(");
			debugstr->append(bv->value.doubleval.precision);
			debugstr->append(",");
			debugstr->append(bv->value.doubleval.scale);
			debugstr->append(")");
			#endif

			clientsock->write((uint16_t)DOUBLE_DATA);
			clientsock->write(bv->value.doubleval.value);
			clientsock->write((uint32_t)bv->value.
						doubleval.precision);
			clientsock->write((uint32_t)bv->value.
						doubleval.scale);

		} else if (bv->type==CURSOR_BIND) {

			#ifdef SERVER_DEBUG
			debugstr->append("CURSOR:\n");
			debugstr->append(bv->value.cursorid);
			#endif

			clientsock->write((uint16_t)CURSOR_DATA);
			clientsock->write(bv->value.cursorid);
		}

		#ifdef SERVER_DEBUG
		debugPrint("connection",3,debugstr->getString());
		delete debugstr;
		#endif
	}

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"done returning output bind values");
	#endif
}
