// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

#include <datatypes.h>

int	sqlrconnection::sendColumnInfo() {
	if (sendcolumninfo==SEND_COLUMN_INFO) {
		return 1;
	}
	return 0;
}

void	sqlrconnection::sendColumnCount(unsigned long ncols) {

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"sending column count...");
	#endif

	#ifdef SERVER_DEBUG
	debugPrint("connection",3,(long)ncols);
	#endif
	clientsock->write(ncols);

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"done sending column count");
	#endif
}

void	sqlrconnection::sendColumnTypeFormat(unsigned short format) {

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"sending column type format...");
	#endif

	#ifdef SERVER_DEBUG
	if (format==COLUMN_TYPE_IDS) {
		debugPrint("connection",3,"id's");
	} else {
		debugPrint("connection",3,"names");
	}
	#endif

	clientsock->write(format);

	#ifdef SERVER_DEBUG
	debugPrint("connection",2,"done sending column type format");
	#endif
}

void	sqlrconnection::sendColumnDefinition(const char *name,
						unsigned short namelen,
						unsigned short type, 
						unsigned long size,
						unsigned long precision,
						unsigned long scale,
						unsigned short nullable,
						unsigned short primarykey) {

	#ifdef SERVER_DEBUG
	debugstr=new stringbuffer();
	for (int i=0; i<namelen; i++) {
		debugstr->append(name[i]);
	}
	debugstr->append(":");
	debugstr->append((long)type);
	debugstr->append(":");
	debugstr->append((long)size);
	debugstr->append(" (");
	debugstr->append((long)precision);
	debugstr->append(",");
	debugstr->append((long)scale);
	debugstr->append(") ");
	if (!nullable) {
		debugstr->append("NOT NULL ");
	}
	if (primarykey) {
		debugstr->append("Primary key");
	}
	debugPrint("connection",3,debugstr->getString());
	delete debugstr;
	#endif

	clientsock->write(namelen);
	clientsock->write(name,namelen);
	clientsock->write(type);
	clientsock->write(size);
	clientsock->write(precision);
	clientsock->write(scale);
	clientsock->write(nullable);
	clientsock->write(primarykey);
}

void	sqlrconnection::sendColumnDefinitionString(const char *name,
						unsigned short namelen,
						const char *type, 
						unsigned short typelen,
						unsigned long size,
						unsigned long precision,
						unsigned long scale,
						unsigned short nullable,
						unsigned short primarykey) {

	#ifdef SERVER_DEBUG
	debugstr=new stringbuffer();
	for (int i=0; i<namelen; i++) {
		debugstr->append(name[i]);
	}
	debugstr->append(":");
	for (int i=0; i<typelen; i++) {
		debugstr->append(type[i]);
	}
	debugstr->append(":");
	debugstr->append((long)size);
	debugstr->append(" (");
	debugstr->append((long)precision);
	debugstr->append(",");
	debugstr->append((long)scale);
	debugstr->append(") ");
	if (!nullable) {
		debugstr->append("NOT NULL ");
	}
	if (primarykey) {
		debugstr->append("Primary key");
	}
	debugPrint("connection",3,debugstr->getString());
	delete debugstr;
	#endif

	clientsock->write(namelen);
	clientsock->write(name,namelen);
	clientsock->write(typelen);
	clientsock->write(type,typelen);
	clientsock->write(size);
	clientsock->write(precision);
	clientsock->write(scale);
	clientsock->write(nullable);
	clientsock->write(primarykey);
}
