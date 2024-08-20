// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrexportfile.h>

#include <rudiments/filesystem.h>
#include <rudiments/permissions.h>

sqlrexportfile::sqlrexportfile() : sqlrexport() {
	filename=NULL;
	fd=NULL;
}

sqlrexportfile::~sqlrexportfile() {
}

void sqlrexportfile::setFileName(const char *filename) {
	this->filename=filename;
}

const char *sqlrexportfile::getFileName() {
	return filename;
}

void sqlrexportfile::setFileDescriptor(filedescriptor *fd) {
	this->fd=fd;
}

filedescriptor *sqlrexportfile::getFileDescriptor() {
	return fd;
}

bool sqlrexportfile::exportData() {
	return sqlrexport::exportData();
}

void sqlrexportfile::clearFlagsAndCounts() {
	sqlrexport::clearFlagsAndCounts();
	fd=NULL;
}

bool sqlrexportfile::sanityCheck() {

	if (!sqlrexport::sanityCheck()) {
		return false;
	}

	// sanity check on file name
	if (!charstring::isNullOrEmpty(getFileName())) {
		if (!f.create(getFileName(),
			permissions::parsePermString("rw-r--r--"))) {
			return systemError();
		}
		f.setWriteBufferSize(
			filesystem::getOptimumTransferBlockSize(getFileName()));
		setFileDescriptor(&f);
		return true;
	}

	// if no file name was set then fall back to standard output
	setFileDescriptor(&stdoutput);
	return true;
}

bool sqlrexportfile::flush() {
	getFileDescriptor()->flushWriteBuffer(-1,-1);
	return true;
}
