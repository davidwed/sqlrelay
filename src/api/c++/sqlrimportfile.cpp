// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrimportfile.h>

sqlrimportfile::sqlrimportfile() : sqlrimport() {
	filename=NULL;
	fd=NULL;
	extension=NULL;
}

sqlrimportfile::~sqlrimportfile() {
}

void sqlrimportfile::setFileName(const char *filename) {
	this->filename=filename;
}

const char *sqlrimportfile::getFileName() {
	return filename;
}

void sqlrimportfile::setFileDescriptor(filedescriptor *fd) {
	this->fd=fd;
}

filedescriptor *sqlrimportfile::getFileDescriptor() {
	return fd;
}

void sqlrimportfile::setExtension(const char *extension) {
	this->extension=extension;
}

const char *sqlrimportfile::getExtension() {
	return extension;
}

bool sqlrimportfile::importData() {

	fd=NULL;

	// set the table name from the file name, if it wasn't already set
	if (!getObjectName()) {
		setObjectName(file::getBaseName(getFileName(),extension));
	}

	return true;
}
