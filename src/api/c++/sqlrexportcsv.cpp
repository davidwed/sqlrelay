// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrexportcsv.h>

sqlrexportcsv::sqlrexportcsv() {
}

sqlrexportcsv::~sqlrexportcsv() {
}

bool sqlrexportcsv::exportToFile(const char *filename, const char *table) {

	// FIXME: create/open file
	// FIXME: write to file rather than stdoutput

	// print header
	uint32_t	cols=sqlrcur->colCount();
	for (uint32_t j=0; j<cols; j++) {
		if (j) {
			stdoutput.printf(",");
		}
		const char	*name=sqlrcur->getColumnName(j);
		bool		isnumber=charstring::isNumber(name);
		if (!isnumber) {
			stdoutput.write('\'');
		}
		csvEscapeField(name,charstring::length(name));
		if (!isnumber) {
			stdoutput.write('\'');
		}
	}
	stdoutput.printf("\n");

	// print rows
	uint64_t	row=0;
	for (;;) {
		for (uint32_t col=0; col<cols; col++) {
			const char	*field=sqlrcur->getField(row,col);
			if (!field) {
				break;
			}
			if (col) {
				stdoutput.write(',');
			}
			bool	isnumber=charstring::isNumber(field);
			if (!isnumber) {
				stdoutput.write('\'');
			}
			csvEscapeField(field,sqlrcur->getFieldLength(row,col));
			if (!isnumber) {
				stdoutput.write('\'');
			}
		}
		stdoutput.write('\n');
		row++;
		if (sqlrcur->endOfResultSet() && row>=sqlrcur->rowCount()) {
			break;
		}
	}

	return true;
}

void sqlrexportcsv::csvEscapeField(const char *field, uint32_t length) {
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
