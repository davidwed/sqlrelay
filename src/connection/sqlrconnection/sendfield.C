// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection::sendField(const char *data, uint32_t size) {

	#ifdef SERVER_DEBUG
	debugstr->append("\"");
	debugstr->append(data,size);
	debugstr->append("\",");
	#endif

	clientsock->write((uint16_t)STRING_DATA);
	clientsock->write(size);
	clientsock->write(data,size);
}

void sqlrconnection::sendNullField() {

	#ifdef SERVER_DEBUG
	debugstr->append("NULL");
	#endif

	clientsock->write((uint16_t)NULL_DATA);
}

void sqlrconnection::startSendingLong(uint64_t longlength) {
	clientsock->write((uint16_t)START_LONG_DATA);
	clientsock->write(longlength);
}

void sqlrconnection::sendLongSegment(const char *data, uint32_t size) {

	#ifdef SERVER_DEBUG
	debugstr->append(data,size);
	#endif

	clientsock->write((uint16_t)STRING_DATA);
	clientsock->write(size);
	clientsock->write(data,size);
}

void sqlrconnection::endSendingLong() {

	#ifdef SERVER_DEBUG
	debugstr->append(",");
	#endif

	clientsock->write((uint16_t)END_LONG_DATA);
}
