// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection::sendField(const char *data, unsigned long size) {

	#ifdef SERVER_DEBUG
	debugstr->append("\"");
	for (unsigned long i=0; i<size; i++) {
		debugstr->append(data[i]);
	}
	debugstr->append("\",");
	#endif

	clientsock->write((unsigned short)NORMAL_DATA);
	clientsock->write(size);
	clientsock->write(data,size);
}

void sqlrconnection::sendNullField() {

	#ifdef SERVER_DEBUG
	debugstr->append("NULL");
	#endif

	clientsock->write((unsigned short)NULL_DATA);
}

void sqlrconnection::startSendingLong() {
	clientsock->write((unsigned short)START_LONG_DATA);
}

void sqlrconnection::sendLongSegment(const char *data, unsigned long size) {

	#ifdef SERVER_DEBUG
	for (unsigned long i=0; i<size; i++) {
		debugstr->append(data[i]);
	}
	#endif

	clientsock->write((unsigned short)NORMAL_DATA);
	clientsock->write(size);
	clientsock->write(data,size);
}

void sqlrconnection::endSendingLong() {

	#ifdef SERVER_DEBUG
	debugstr->append(",");
	#endif

	clientsock->write((unsigned short)END_LONG_DATA);
}
