// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

#include <datatypes.h>

bool sqlrconnection_svr::sendColumnInfo() {
	if (sendcolumninfo==SEND_COLUMN_INFO) {
		return true;
	}
	return false;
}

void sqlrconnection_svr::sendColumnDefinition(const char *name,
						uint16_t namelen,
						uint16_t type, 
						uint32_t size,
						uint32_t precision,
						uint32_t scale,
						uint16_t nullable,
						uint16_t primarykey,
						uint16_t unique,
						uint16_t partofkey,
						uint16_t unsignednumber,
						uint16_t zerofill,
						uint16_t binary,
						uint16_t autoincrement) {

	debugstr=new stringbuffer();
	for (uint16_t i=0; i<namelen; i++) {
		debugstr->append(name[i]);
	}
	debugstr->append(":");
	debugstr->append(type);
	debugstr->append(":");
	debugstr->append(size);
	debugstr->append(" (");
	debugstr->append(precision);
	debugstr->append(",");
	debugstr->append(scale);
	debugstr->append(") ");
	if (!nullable) {
		debugstr->append("NOT NULL ");
	}
	if (primarykey) {
		debugstr->append("Primary key ");
	}
	if (unique) {
		debugstr->append("Unique");
	}
	dbgfile.debugPrint("connection",3,debugstr->getString());
	delete debugstr;

	clientsock->write(namelen);
	clientsock->write(name,namelen);
	clientsock->write(type);
	clientsock->write(size);
	clientsock->write(precision);
	clientsock->write(scale);
	clientsock->write(nullable);
	clientsock->write(primarykey);
	clientsock->write(unique);
	clientsock->write(partofkey);
	clientsock->write(unsignednumber);
	clientsock->write(zerofill);
	clientsock->write(binary);
	clientsock->write(autoincrement);
}

void sqlrconnection_svr::sendColumnDefinitionString(const char *name,
						uint16_t namelen,
						const char *type, 
						uint16_t typelen,
						uint32_t size,
						uint32_t precision,
						uint32_t scale,
						uint16_t nullable,
						uint16_t primarykey,
						uint16_t unique,
						uint16_t partofkey,
						uint16_t unsignednumber,
						uint16_t zerofill,
						uint16_t binary,
						uint16_t autoincrement) {

	debugstr=new stringbuffer();
	for (uint16_t i=0; i<namelen; i++) {
		debugstr->append(name[i]);
	}
	debugstr->append(":");
	for (uint16_t i=0; i<typelen; i++) {
		debugstr->append(type[i]);
	}
	debugstr->append(":");
	debugstr->append(size);
	debugstr->append(" (");
	debugstr->append(precision);
	debugstr->append(",");
	debugstr->append(scale);
	debugstr->append(") ");
	if (!nullable) {
		debugstr->append("NOT NULL ");
	}
	if (primarykey) {
		debugstr->append("Primary key ");
	}
	if (unique) {
		debugstr->append("Unique");
	}
	dbgfile.debugPrint("connection",3,debugstr->getString());
	delete debugstr;

	clientsock->write(namelen);
	clientsock->write(name,namelen);
	clientsock->write(typelen);
	clientsock->write(type,typelen);
	clientsock->write(size);
	clientsock->write(precision);
	clientsock->write(scale);
	clientsock->write(nullable);
	clientsock->write(primarykey);
	clientsock->write(unique);
	clientsock->write(partofkey);
	clientsock->write(unsignednumber);
	clientsock->write(zerofill);
	clientsock->write(binary);
	clientsock->write(autoincrement);
}
