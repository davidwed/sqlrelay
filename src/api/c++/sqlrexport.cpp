// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#include <sqlrelay/sqlrexport.h>

#include <rudiments/error.h>

#define NEED_IS_NUMBER_TYPE_CHAR
#include <datatypes.h>

sqlrexport::sqlrexport() {

	sqlrcon=NULL;
	sqlrcur=NULL;

	table=NULL;

	excludecolumns=false;
	columnstoexclude=NULL;

	lg=NULL;
	coarseloglevel=0;
	fineloglevel=9;
	logindent=0;
	logerrors=true;

	clearFlagsAndCounts();
}

sqlrexport::~sqlrexport() {
}

void sqlrexport::setSqlrConnection(sqlrconnection *sqlrcon) {
	this->sqlrcon=sqlrcon;
}

sqlrconnection *sqlrexport::getSqlrConnection() {
	return sqlrcon;
}

void sqlrexport::setSqlrCursor(sqlrcursor *sqlrcur) {
	this->sqlrcur=sqlrcur;
}

sqlrcursor *sqlrexport::getSqlrCursor() {
	return sqlrcur;
}

void sqlrexport::setTable(const char *table) {
	this->table=table;
}

const char *sqlrexport::getTable() {
	return table;
}

void sqlrexport::setExcludeColumns(bool excludecolumns) {
	this->excludecolumns=excludecolumns;
}

bool sqlrexport::getExcludeColumns() {
	return excludecolumns;
}

void sqlrexport::setColumnsToExclude(const char * const *columnstoexclude) {
	this->columnstoexclude=columnstoexclude;
}

const char * const *sqlrexport::getColumnsToExclude() {
	return columnstoexclude;
}

void sqlrexport::setLogger(logger *lg) {
	this->lg=lg;
}

logger *sqlrexport::getLogger() {
	return lg;
}

void sqlrexport::setCoarseLogLevel(uint8_t coarseloglevel) {
	this->coarseloglevel=coarseloglevel;
}

void sqlrexport::setFineLogLevel(uint8_t fineloglevel) {
	this->fineloglevel=fineloglevel;
}

void sqlrexport::setLogIndent(uint32_t logindent) {
	this->logindent=logindent;
}

uint8_t sqlrexport::getCoarseLogLevel() {
	return coarseloglevel;
}

uint8_t sqlrexport::getFineLogLevel() {
	return fineloglevel;
}

uint32_t sqlrexport::getLogIndent() {
	return logindent;
}

void sqlrexport::setLogErrors(bool logerrors) {
	this->logerrors=logerrors;
}

bool sqlrexport::getLogErrors() {
	return logerrors;
}

bool sqlrexport::exportStart() {
	// by default, just return success
	return true;
}

bool sqlrexport::columnsStart() {
	// by default, just return success
	return true;
}

bool sqlrexport::columnStart() {
	// by default, just return success
	return true;
}

bool sqlrexport::columnEnd() {
	// by default, just return success
	return true;
}

bool sqlrexport::columnsEnd() {
	// by default, just return success
	return true;
}

bool sqlrexport::rowsStart() {
	// by default, just return success
	return true;
}

bool sqlrexport::rowStart() {
	// by default, just return success
	return true;
}

bool sqlrexport::fieldStart() {
	// by default, just return success
	return true;
}

bool sqlrexport::fieldEnd() {
	// by default, just return success
	return true;
}

bool sqlrexport::rowEnd() {
	// by default, just return success
	return true;
}

bool sqlrexport::rowsEnd() {
	// by default, just return success
	return true;
}

bool sqlrexport::beginStart() {
	// by default, just return success
	return true;
}

bool sqlrexport::beginEnd() {
	// by default, just return success
	return true;
}

bool sqlrexport::commitStart() {
	// by default, just return success
	return true;
}

bool sqlrexport::commitEnd() {
	// by default, just return success
	return true;
}

bool sqlrexport::error(int64_t errornumber, const char *errormessage) {
	// by default, just return error
	return false;
}

bool sqlrexport::exportEnd() {
	// by default, just return success
	return true;
}

void sqlrexport::setExportedRowCount(uint64_t exportedrowcount) {
	this->exportedrowcount=exportedrowcount;
}

uint64_t sqlrexport::getExportedRowCount() {
	return exportedrowcount;
}

void sqlrexport::setExcludeRow(bool excluderow) {
	this->excluderow=excluderow;
}

bool sqlrexport::getExcludeRow() {
	return excluderow;
}

void sqlrexport::setCurrentRow(uint64_t currentrow) {
	this->currentrow=currentrow;
}

uint64_t sqlrexport::getCurrentRow() {
	return currentrow;
}

void sqlrexport::setCurrentColumn(uint32_t currentcol) {
	this->currentcol=currentcol;
}

uint32_t sqlrexport::getCurrentColumn() {
	return currentcol;
}

void sqlrexport::setCurrentColumnName(const char *currentcolname) {
	this->currentcolname=currentcolname;
}

const char *sqlrexport::getCurrentColumnName() {
	return currentcolname;
}

void sqlrexport::setCurrentField(const char *currentfield) {
	this->currentfield=currentfield;
}

const char *sqlrexport::getCurrentField() {
	return currentfield;
}

void sqlrexport::setIsNumericColumn(uint64_t index, bool value) {
	numericcolumn[index]=value;
}

bool sqlrexport::getIsNumericColumn(uint64_t index) {
	return (index<numericcolumn.getCount())?numericcolumn[index]:false;
}

void sqlrexport::clearAreNumericColumns() {
	numericcolumn.clear();
}

bool sqlrexport::exportData() {

	// start processing the export
	if (!startProcessingExport()) {
		return false;
	}

	// start processing columns
	if (!startProcessingColumns()) {
		return false;
	}

	// export columns...
	bool	first=true;
	for (setCurrentColumn(0);
			getCurrentColumn()<getSqlrCursor()->colCount();
			setCurrentColumn(getCurrentColumn()+1)) {

		// start processing column
		if (!startProcessingColumn()) {
			return false;
		}

		// if we're not excluding this column...
		if (!excludeThisColumn()) {
			if (!exportColumnName(first)) {
				return false;
			}
			first=false;
		}

		// end processing column
		if (!endProcessingColumn()) {
			return false;
		}
	}

	// end processing columns
	if (!endProcessingColumns()) {
		return false;
	}

	// start processing rows
	if (!startProcessingRows()) {
		return false;
	}

	// export rows...
	while (!getSqlrCursor()->endOfResultSet() ||
			getCurrentRow()<getSqlrCursor()->rowCount()) {

		// start processing row
		if (!startProcessingRow()) {
			return false;
		}

		bool	first=true;
		for (setCurrentColumn(0);
				getCurrentColumn()<getSqlrCursor()->colCount();
				setCurrentColumn(getCurrentColumn()+1)) {

			// start processing field
			if (!startProcessingField()) {
				return false;
			}

			// if we're not excluding this field...
			if (!excludeThisField()) {
				if (!exportField(first)) {
					return false;
				}
				first=false;
			}

			// end processing field
			if (!endProcessingField()) {
				return false;
			}
		}

		// end processing row
		if (!endProcessingRow()) {
			return false;
		}
	}

	// end processing rows
	if (!endProcessingRows()) {
		return false;
	}

	// end processing the export
	return endProcessingExport();
}

void sqlrexport::clearFlagsAndCounts() {
	excluderow=false;
	currentrow=0;
	currentcol=0;
	currentcolname=NULL;
	currentfield=NULL;
	exportedrowcount=0;
	numericcolumn.clear();
}

bool sqlrexport::startProcessingExport() {

	// clear flags and counts
	clearFlagsAndCounts();

	// sanity check
	if (!sanityCheck()) {
		return false;
	}

	// set initial column/field
	setCurrentColumnName(getSqlrCursor()->getColumnName(0));
	setCurrentField(getCurrentColumnName());

	// determine numeric columns
	uint32_t	cols=sqlrcur->colCount();
	for (uint32_t i=0; i<cols; i++) {
		setIsNumericColumn(
			i,isNumberTypeChar(getSqlrCursor()->getColumnType(i)));
	}

	// call the export-start event
	if (!exportStart()) {
		flush();
		return false;
	}
	return true;
}

bool sqlrexport::sanityCheck() {
	// by default, just return success
	return true;
}

bool sqlrexport::startProcessingColumns() {

	// call the columns-start event
	if (!columnsStart()) {
		flush();
		return false;
	}
	return true;
}

bool sqlrexport::startProcessingColumn() {

	// set the current column name (and field)
	setCurrentColumnName(
		getSqlrCursor()->getColumnName(getCurrentColumn()));
	setCurrentField(getCurrentColumnName());

	// call the column-start event
	if (!columnStart()) {
		flush();
		return false;
	}

	// reset the current field to the current column name too
	// (in case columnStart overrode the columm name)
	setCurrentField(getCurrentColumnName());

	return true;
}

bool sqlrexport::excludeThisColumn() {
	return getExcludeColumns() ||
			charstring::isInSet(getCurrentField(),
						getColumnsToExclude());
}

bool sqlrexport::exportColumnName(bool first) {
	// by default, just return success
	return true;
}

bool sqlrexport::endProcessingColumn() {

	// call the column-end event
	if (!columnEnd()) {
		flush();
		return false;
	}
	return true;
}

bool sqlrexport::endProcessingColumns() {

	// set the current column and field to NULL
	setCurrentColumnName(NULL);
	setCurrentField(NULL);

	// call the columns-end event
	if (!columnsEnd()) {
		flush();
		return false;
	}
	return true;
}

bool sqlrexport::startProcessingRows() {

	// reset current column/field
	setCurrentColumn(0);
	setCurrentColumnName(sqlrcur->getColumnName(0));
	setCurrentField(sqlrcur->getField(0,(uint32_t)0));

	// call the rows-start event
	if (!rowsStart()) {
		flush();
		return false;
	}
	return true;
}

bool sqlrexport::startProcessingRow() {

	// reset export-row flag and current column/field
	setExcludeRow(false);
	setCurrentColumn(0);
	setCurrentColumnName(sqlrcur->getColumnName(0));
	setCurrentField(sqlrcur->getField(getCurrentRow(),(uint32_t)0));

	// call the row-start event
	if (!rowStart()) {
		flush();
		return false;
	}
	return true;
}

bool sqlrexport::startProcessingField() {

	// set the current column and field
	setCurrentColumnName(sqlrcur->getColumnName(getCurrentColumn()));
	setCurrentField(sqlrcur->getField(getCurrentRow(),getCurrentColumn()));

	// FIXME: I'm not sure why we're doing this,
	// I don't think it's possible for this to happen
	if (!getCurrentField()) {
		// FIXME: set error
		return false;
	}

	// call the field-start event
	if (!fieldStart()) {
		flush();
		return false;
	}
	return true;
}

bool sqlrexport::excludeThisField() {
	return getExcludeRow() ||
		charstring::isInSet(
			getSqlrCursor()->getColumnName(getCurrentColumn()),
			getColumnsToExclude());
}

bool sqlrexport::exportField(bool first) {
	// by default, just return success
	return true;
}

bool sqlrexport::endProcessingField() {

	// call the field-end event
	if (!fieldEnd()) {
		flush();
		return false;
	}
	return true;
}

bool sqlrexport::endProcessingRow() {

	// set the current column and field to NULL
	setCurrentColumnName(NULL);
	setCurrentField(NULL);

	// call the row-end event
	if (!rowEnd()) {
		flush();
		return false;
	}

	// update exported row count
	if (!getExcludeRow()) {
		setExportedRowCount(getExportedRowCount()+1);
	}

	// update current row
	setCurrentRow(getCurrentRow()+1);
	return true;
}

bool sqlrexport::endProcessingRows() {

	// call the rows-end event
	if (!rowsEnd()) {
		flush();
		return false;
	}
	return true;
}

bool sqlrexport::endProcessingExport() {

	// call the export-end event
	if (!exportEnd()) {
		flush();
		return false;
	}
	flush();
	return true;
}

bool sqlrexport::flush() {
	return true;
}

bool sqlrexport::escapeValue(filedescriptor *fd, const char *value) {
	return true;
}

bool sqlrexport::systemError() {
	char	*err=error::getErrorString();
	bool	retval=error(error::getErrorNumber(),err);
	delete[] err;
	return retval;
}
