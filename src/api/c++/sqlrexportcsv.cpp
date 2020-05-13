// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrexportcsv.h>

sqlrexportcsv::sqlrexportcsv() {
}

sqlrexportcsv::~sqlrexportcsv() {
}

bool sqlrexportcsv::exportToFileDescriptor(filedescriptor *fd,
						const char *filename,
						const char *table) {

	// print header
	uint32_t	cols=sqlrcur->colCount();
	if (!ignorecolumns) {
		for (uint32_t j=0; j<cols; j++) {
			if (j) {
				fd->printf(",");
			}
			const char	*name=sqlrcur->getColumnName(j);
			bool		isnumber=charstring::isNumber(name);
			if (!isnumber) {
				fd->write('"');
			}
			escapeField(fd,name,charstring::length(name));
			if (!isnumber) {
				fd->write('"');
			}
		}
		fd->printf("\n");
	}

	// print rows
	uint64_t	row=0;
	for (;;) {
		for (uint32_t col=0; col<cols; col++) {
			const char	*field=sqlrcur->getField(row,col);
			if (!field) {
				break;
			}
			if (col) {
				fd->write(',');
			}
			bool	isnumber=charstring::isNumber(field);
			if (!isnumber) {
				fd->write('"');
			}
			escapeField(fd,field,sqlrcur->getFieldLength(row,col));
			if (!isnumber) {
				fd->write('"');
			}
		}
		fd->write('\n');
		row++;
		if (sqlrcur->endOfResultSet() && row>=sqlrcur->rowCount()) {
			break;
		}
	}

	return true;
}

void sqlrexportcsv::escapeField(filedescriptor *fd,
					const char *field, uint32_t length) {
	for (uint32_t index=0; index<length; index++) {
		if (field[index]=='"') {
			fd->write("\"\"");
		} else if (field[index]<' ' || field[index]>'~' ||
				field[index]=='&' || field[index]=='<' ||
				field[index]=='>') {
			// FIXME: how should these be escaped in a CSV?
			fd->printf("&%d;",(uint8_t)field[index]);
		} else {
			fd->write(field[index]);
		}
	}
}
