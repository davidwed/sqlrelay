// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrexportxml.h>

sqlrexportxml::sqlrexportxml() {
}

sqlrexportxml::~sqlrexportxml() {
}

bool sqlrexportxml::exportToFileDescriptor(filedescriptor *fd,
						const char *filename,
						const char *table) {

	// print header
	fd->printf("<?xml version=\"1.0\"?>\n");

	// print table name
	if (!charstring::isNullOrEmpty(table)) {
		fd->printf("<table name=\"%s\">\n",table);
	}

	// print columns
	uint32_t	cols=sqlrcur->colCount();
	if (!ignorecolumns) {
		fd->printf("<columns count=\"%d\">\n",cols);
		for (uint32_t j=0; j<cols; j++) {
			fd->printf("	<column name=\"%s\" type=\"%s\"/>\n",
						sqlrcur->getColumnName(j),
						sqlrcur->getColumnType(j));
		}
		fd->printf("</columns>\n");
	}

	// print rows
	fd->printf("<rows>\n");
	uint64_t	row=0;
	do {
		fd->printf("	<row>\n");
		for (uint32_t col=0; col<cols; col++) {
			const char	*field=sqlrcur->getField(row,col);
			if (!field) {
				break;
			}
			fd->printf("	<field>");
			escapeField(fd,field,sqlrcur->getFieldLength(row,col));
			fd->printf("</field>\n");
		}
		fd->printf("	</row>\n");
		row++;
	} while (!sqlrcur->endOfResultSet() || row<sqlrcur->rowCount());
	fd->printf("</rows>\n");
	fd->printf("</table>\n");

	return true;
}

void sqlrexportxml::escapeField(filedescriptor *fd,
					const char *field, uint32_t length) {
	for (uint32_t index=0; index<length; index++) {
		if (field[index]=='"') {
			fd->write("\"\"");
		} else if (field[index]<' ' || field[index]>'~' ||
				field[index]=='&' || field[index]=='<' ||
				field[index]=='>') {
			fd->printf("&%d;",(uint8_t)field[index]);
		} else {
			fd->write(field[index]);
		}
	}
}
