// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrexportxml.h>

sqlrexportxml::sqlrexportxml() {
}

sqlrexportxml::~sqlrexportxml() {
}

bool sqlrexportxml::exportToFile(const char *filename, const char *table) {

	// FIXME: create/open file
	// FIXME: write to file rather than stdoutput

	// print header
	stdoutput.printf("<?xml version=\"1.0\"?>\n");

	// print table name
	if (!charstring::isNullOrEmpty(table)) {
		stdoutput.printf("<table name=\"%s\">\n",table);
	}

	// print columns
	uint32_t	cols=sqlrcur->colCount();
	stdoutput.printf("<columns count=\"%d\">\n",cols);
	for (uint32_t j=0; j<cols; j++) {
		stdoutput.printf("	<column name=\"%s\" type=\"%s\"/>\n",
			sqlrcur->getColumnName(j),sqlrcur->getColumnType(j));
	}
	stdoutput.printf("</columns>\n");

	// print rows
	stdoutput.printf("<rows>\n");
	uint64_t	row=0;
	do {
		stdoutput.printf("	<row>\n");
		for (uint32_t col=0; col<cols; col++) {
			const char	*field=sqlrcur->getField(row,col);
			if (!field) {
				break;
			}
			stdoutput.printf("	<field>");
			xmlEscapeField(field,sqlrcur->getFieldLength(row,col));
			stdoutput.printf("</field>\n");
		}
		stdoutput.printf("	</row>\n");
		row++;
	} while (!sqlrcur->endOfResultSet() || row<sqlrcur->rowCount());
	stdoutput.printf("</rows>\n");
	stdoutput.printf("</table>\n");

	return true;
}

void sqlrexportxml::xmlEscapeField(const char *field, uint32_t length) {
	for (uint32_t index=0; index<length; index++) {
		if (field[index]=='"') {
			stdoutput.write("\"\"");
		} else if (field[index]<' ' || field[index]>'~' ||
				field[index]=='&' || field[index]=='<' ||
				field[index]=='>') {
			stdoutput.printf("&%d;",(uint8_t)field[index]);
		} else {
			stdoutput.printf("%c",field[index]);
		}
	}
}
