// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>
#include <string.h>

void sqlrconnection::returnOutputBindValues(sqlrcursor *cursor) {

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"returning output bind values");
	debugPrint("connection",3,(long)cursor->outbindcount);
	#endif

	// run through the output bind values, sending them back
	for (int i=0; i<cursor->outbindcount; i++) {

		bindvar	*bv=&(cursor->outbindvars[i]);

		#ifdef SERVER_DEBUG
		debugstr=new stringbuffer();
		debugstr->append((long)i);
		debugstr->append(":");
		#endif

		if (bindValueIsNull(bv->isnull)) {

			#ifdef SERVER_DEBUG
			debugstr->append("NULL");
			#endif

			clientsock->write((unsigned short)NULL_DATA);

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

			clientsock->write((unsigned short)NORMAL_DATA);
			bv->valuesize=strlen((char *)bv->value.stringval);
			clientsock->write(bv->valuesize);
			clientsock->write(bv->value.stringval,bv->valuesize);

		} else if (bv->type==CURSOR_BIND) {

			#ifdef SERVER_DEBUG
			debugstr->append("CURSOR:\n");
			debugstr->append((long)bv->value.cursorid);
			#endif

			clientsock->write((unsigned short)CURSOR_DATA);
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
