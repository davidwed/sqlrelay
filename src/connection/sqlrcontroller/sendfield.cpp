// Copyright (c) 1999-2001  David Muse
// See the file COPYING for more information

#include <sqlrconnection.h>

void sqlrconnection_svr::sendField(const char *data, uint32_t size) {

	if (dbgfile.debugEnabled()) {
		debugstr->append("\"");
		debugstr->append(data,size);
		debugstr->append("\",");
	}

	clientsock->write((uint16_t)STRING_DATA);
	clientsock->write(size);
	clientsock->write(data,size);
}

void sqlrconnection_svr::sendNullField() {

	if (dbgfile.debugEnabled()) {
		debugstr->append("NULL");
	}

	clientsock->write((uint16_t)NULL_DATA);
}

void sqlrconnection_svr::startSendingLong(uint64_t longlength) {
	clientsock->write((uint16_t)START_LONG_DATA);
	clientsock->write(longlength);
}

void sqlrconnection_svr::sendLongSegment(const char *data, uint32_t size) {

	if (dbgfile.debugEnabled()) {
		debugstr->append(data,size);
	}

	clientsock->write((uint16_t)STRING_DATA);
	clientsock->write(size);
	clientsock->write(data,size);
}

void sqlrconnection_svr::endSendingLong() {

	if (dbgfile.debugEnabled()) {
		debugstr->append(",");
	}

	clientsock->write((uint16_t)END_LONG_DATA);
}
