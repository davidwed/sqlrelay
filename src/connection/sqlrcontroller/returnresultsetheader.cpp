// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrcontroller.h>

#include <datatypes.h>

void sqlrcontroller_svr::returnResultSetHeader(sqlrcursor_svr *cursor) {

	dbgfile.debugPrint("connection",2,"returning result set header...");

	// return the row counts
	dbgfile.debugPrint("connection",3,"returning row counts...");
	sendRowCounts(cursor->knowsRowCount(),cursor->rowCount(),
			cursor->knowsAffectedRows(),cursor->affectedRows());
	dbgfile.debugPrint("connection",3,"done returning row counts");


	// write a flag to the client indicating whether 
	// or not the column information will be sent
	clientsock->write(sendcolumninfo);

	if (sendcolumninfo==SEND_COLUMN_INFO) {
		dbgfile.debugPrint("connection",3,
					"column info will be sent");
	} else {
		dbgfile.debugPrint("connection",3,
					"column info will not be sent");
	}


	// return the column count
	dbgfile.debugPrint("connection",3,"returning column counts...");
	clientsock->write(cursor->colCount());
	dbgfile.debugPrint("connection",3,"done returning column counts");


	if (sendcolumninfo==SEND_COLUMN_INFO) {

		// return the column type format
		dbgfile.debugPrint("connection",2,
					"sending column type format...");
		uint16_t	format=cursor->columnTypeFormat();
		if (format==COLUMN_TYPE_IDS) {
			dbgfile.debugPrint("connection",3,"id's");
		} else {
			dbgfile.debugPrint("connection",3,"names");
		}
		clientsock->write(format);
		dbgfile.debugPrint("connection",2,
					"done sending column type format");

		// return the column info
		dbgfile.debugPrint("connection",3,"returning column info...");
		returnColumnInfo(cursor,format);
		dbgfile.debugPrint("connection",3,"done returning column info");
	}


	// return the output bind vars
	returnOutputBindValues(cursor);


	// terminate the bind vars
	clientsock->write((uint16_t)END_BIND_VARS);

	flushWriteBuffer();

	dbgfile.debugPrint("connection",2,"done returning result set header");
}

void sqlrcontroller_svr::returnColumnInfo(sqlrcursor_svr *cursor,
							uint16_t format) {

	for (uint32_t i=0; i<cursor->colCount(); i++) {

		const char	*name=cursor->getColumnName(i);
		uint16_t	namelen=cursor->getColumnNameLength(i);
		uint32_t	length=cursor->getColumnLength(i);
		uint32_t	precision=cursor->getColumnPrecision(i);
		uint32_t	scale=cursor->getColumnScale(i);
		uint16_t	nullable=cursor->getColumnIsNullable(i);
		uint16_t	primarykey=cursor->getColumnIsPrimaryKey(i);
		uint16_t	unique=cursor->getColumnIsUnique(i);
		uint16_t	partofkey=cursor->getColumnIsPartOfKey(i);
		uint16_t	unsignednumber=cursor->getColumnIsUnsigned(i);
		uint16_t	zerofill=cursor->getColumnIsZeroFilled(i);
		uint16_t	binary=cursor->getColumnIsBinary(i);
		uint16_t	autoincrement=
					cursor->getColumnIsAutoIncrement(i);

		if (format==COLUMN_TYPE_IDS) {
			sendColumnDefinition(name,namelen,
					cursor->getColumnType(i),
					length,precision,scale,
					nullable,primarykey,unique,partofkey,
					unsignednumber,zerofill,binary,
					autoincrement);
		} else {
			sendColumnDefinitionString(name,namelen,
					cursor->getColumnTypeName(i),
					cursor->getColumnTypeNameLength(i),
					length,precision,scale,
					nullable,primarykey,unique,partofkey,
					unsignednumber,zerofill,binary,
					autoincrement);
		}
	}
}
