// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrexporttable.h>
#include <rudiments/dynamicarray.h>

sqlrexporttable::sqlrexporttable() : sqlrexport() {
	exportcon=NULL;
	exportcur=NULL;
	commitcount=0;
	bf='\0';
	bindindex=0;
	bindnames.setManageArrayValues(true);
	firstrow=true;
}

sqlrexporttable::~sqlrexporttable() {
}

void sqlrexporttable::setExportSqlrConnection(sqlrconnection *exportcon) {
	this->exportcon=exportcon;
}

sqlrconnection *sqlrexporttable::getExportSqlrConnection() {
	return exportcon;
}

void sqlrexporttable::setExportSqlrCursor(sqlrcursor *exportcur) {
	this->exportcur=exportcur;
}

sqlrcursor *sqlrexporttable::getExportSqlrCursor() {
	return exportcur;
}

stringbuffer *sqlrexporttable::getInsertQueryBuffer() {
	return &insertquery;
}

void sqlrexporttable::setCommitCount(uint64_t commitcount) {
	this->commitcount=commitcount;
}

uint64_t sqlrexporttable::getCommitCount() {
	return commitcount;
}

bool sqlrexporttable::exportData() {
	return sqlrexport::exportData();
}

void sqlrexporttable::clearFlagsAndCounts() {
	sqlrexport::clearFlagsAndCounts();
	insertquery.clear();
	bf='\0';
	bindindex=0;
	bindnames.clear();
	firstrow=true;
}

bool sqlrexporttable::sanityCheck() {

	if (!sqlrexport::sanityCheck()) {
		return false;
	}

	if (!getExportSqlrConnection()) {
		return error(
			0,"No connection set with setExportSqlrConnection()");
	}
	if (!getExportSqlrCursor()) {
		return error(
			0,"No connection set with setExportSqlrCursor()");
	}
	if (!getTable()) {
		return error(0,"No table set with setTable()");
	}

	return true;
}

bool sqlrexporttable::startProcessingColumns() {

	if (!sqlrexport::startProcessingColumns()) {
		return false;
	}

	// start building the insert query
	insertquery.append("insert into ");
	insertquery.append(getTable());
	insertquery.append(" values (");
	const char	*bindformat=getExportSqlrConnection()->bindFormat();
	bf=(!charstring::isNullOrEmpty(bindformat))?bindformat[0]:':';
	return true;
}

bool sqlrexporttable::excludeThisColumn() {
	// NOTE that we don't check getExcludeColumns() as that
	// doesn't make any sense when exporting to a table
	return charstring::isInSet(getCurrentField(),getColumnsToExclude());
}

bool sqlrexporttable::exportColumnName(bool first) {

	if (!sqlrexport::exportColumnName(first)) {
		return false;
	}

	// append the bind variable for this column to the insert query
	if (bindindex) {
		insertquery.append(',');
	}
	if (bf=='?') {
		insertquery.append('?');
	} else if (bf=='$') {
		insertquery.append('$')->append(bindindex+1);
	} else if (bf=='@' || bf==':') {
		insertquery.append(bf)->append(bindindex+1);
	}
	bindindex++;
	return true;
}

bool sqlrexporttable::endProcessingColumns() {

	if (!sqlrexport::endProcessingColumns()) {
		return false;
	}

	// close the values clause
	insertquery.append(')');
	return true;
}

bool sqlrexporttable::startProcessingRows() {

	if (!sqlrexport::startProcessingRows()) {
		return false;
	}

	// start a transaction, if necessary
	if (getCommitCount()) {
		if (!beginStart()) {
			return false;
		}
		sqlrconnection	*exportcon=getExportSqlrConnection();
		if (!exportcon->begin()) {
			if (!error(exportcon->errorNumber(),
					exportcon->errorMessage())) {
				return false;
			}
		}
		if (!beginEnd()) {
			return false;
		}
		
	}

	// prepare query
	getExportSqlrCursor()->prepareQuery(insertquery.getString(),
						insertquery.getSize());
	return true;
}

bool sqlrexporttable::startProcessingRow() {

	// commit/begin, if necessary
	if (getCommitCount() &&
		!((getCurrentRow()+1)%getCommitCount())) {
		if (!commitStart()) {
			return false;
		}
		sqlrconnection	*exportcon=getExportSqlrConnection();
		if (!exportcon->commit()) {
			if (!error(exportcon->errorNumber(),
					exportcon->errorMessage())) {
				return false;
			}
		}
		if (!commitEnd()) {
			return false;
		}
		if (!beginStart()) {
			return false;
		}
		if (!exportcon->begin()) {
			if (!error(exportcon->errorNumber(),
					exportcon->errorMessage())) {
				return false;
			}
		}
		if (!beginEnd()) {
			return false;
		}
	}

	// reset bind index
	bindindex=0;

	if (!sqlrexport::startProcessingRow()) {
		// even if this fails, we need to do a final commit
		finalCommit();
		return false;
	}
	return true;
}

bool sqlrexporttable::startProcessingField() {

	if (!sqlrexport::startProcessingField()) {
		// even if this fails, we need to do a final commit
		finalCommit();
		return false;
	}
	return true;
}

bool sqlrexporttable::endProcessingField() {

	if (!sqlrexport::endProcessingField()) {
		// even if this fails, we need to do a final commit
		finalCommit();
		return false;
	}
	return true;
}

bool sqlrexporttable::exportField(bool first) {

	if (!sqlrexport::exportField(first)) {
		return false;
	}

	// set the bind variable name for this position, if necessary
	if (firstrow) {
		bindnames[bindindex]=charstring::parseNumber(bindindex+1);
	}

	// bind the current field
	exportcur->inputBind(bindnames[bindindex],getCurrentField());

	// next...
	bindindex++;

	return true;
}

bool sqlrexporttable::endProcessingRow() {

	if (!getExcludeRow()) {
		// It's not impossible that there were 0 columns in
		// this result set.  If that was the case then
		// bindindex should still be 0 at this point, and no
		// values should be bound.  In that case, we don't want
		// to attempt to execute anything.
		sqlrcursor	*exportcur=getExportSqlrCursor();
		if (bindindex) {
			if (!exportcur->executeQuery()) {
				if (!error(exportcur->errorNumber(),
						exportcur->errorMessage())) {
					return false;
				}
			}
		}
		exportcur->clearBinds();
		firstrow=false;
	}

	if (!sqlrexport::endProcessingRow()) {
		// even if this fails, we need to do a final commit
		finalCommit();
		return false;
	}
	return true;
}

bool sqlrexporttable::endProcessingRows() {

	// final commit
	if (!finalCommit()) {
		return false;
	}

	return sqlrexport::endProcessingRows();
}

bool sqlrexporttable::finalCommit() {

	// do the final commit, if necessary
	if (getCommitCount()) {
		if (!commitStart()) {
			return false;
		}
		if (!exportcon->commit()) {
			if (!error(exportcon->errorNumber(),
					exportcon->errorMessage())) {
				return false;
			}
		}
		if (!commitEnd()) {
			return false;
		}
	}
	return true;
}
