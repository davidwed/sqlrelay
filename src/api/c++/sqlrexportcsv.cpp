// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrexportcsv.h>
#include <rudiments/filedescriptor.h>
#include <rudiments/file.h>
#include <rudiments/permissions.h>

sqlrexportcsv::sqlrexportcsv() {
}

sqlrexportcsv::~sqlrexportcsv() {
}

bool sqlrexportcsv::exportToFile(const char *filename, const char *table) {

	// reset flags
	exportrow=true;
	currentrow=0;
	currentcol=0;
	currentfield=NULL;

	// output to stdoutput or create/open file
	filedescriptor	*fd=&stdoutput;
	file		f;
	if (!charstring::isNullOrEmpty(filename)) {
		if (!f.create(filename,
				permissions::evalPermString("rw-r--r--"))) {
			// FIXME: report error
			return false;
		}
		fd=&f;
	}

	// export header
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
			escapeField(fd,name);
			if (!isnumber) {
				fd->write('"');
			}
		}
		fd->printf("\n");
	}

	// call the pre-rows event
	if (!rowsStart()) {
		return false;
	}

	// export rows...
	do {

		// reset export-row flag
		exportrow=true;

		// call the pre-row event
		if (!rowStart()) {
			return false;
		}

		// if rowStart() didn't disable export of this row...
		if (exportrow) {
			for (currentcol=0; currentcol<cols; currentcol++) {

				// get the field
				currentfield=sqlrcur->getField(
							currentrow,currentcol);
				if (!currentfield) {
					break;
				}

				// prepend a comma if necessary
				if (currentcol) {
					fd->write(',');
				}

				// call the pre-column event
				if (!colStart()) {
					return false;
				}

				// export the field
				bool	isnumber=
					charstring::isNumber(currentfield);
				if (!isnumber) {
					fd->write('"');
				}
				escapeField(fd,currentfield);
				if (!isnumber) {
					fd->write('"');
				}

				// call the post-column event
				if (!colEnd()) {
					return false;
				}
			}
			fd->write('\n');
		}

		// call the post-row event
		if (!rowEnd()) {
			return false;
		}

		currentrow++;

	} while  (!sqlrcur->endOfResultSet() || currentrow<sqlrcur->rowCount());

	// call the post-rows event
	if (!rowsEnd()) {
		return false;
	}

	return true;
}

void sqlrexportcsv::escapeField(filedescriptor *fd, const char *field) {
	for (const char *f=field; *f; f++) {
		if (*f=='"') {
			fd->write("\"\"");
		} else {
			fd->write(*f);
		}
	}
}
