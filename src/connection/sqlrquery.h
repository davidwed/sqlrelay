// Copyright (c) 2012  David Muse
// See the file COPYING for more information

#ifndef SQLRQUERY_H
#define SQLRQUERY_H

#include <rudiments/xmldom.h>
#include <rudiments/xmldomnode.h>
#include <rudiments/stringbuffer.h>

// for return values of columnTypeFormat
#include <datatypes.h>

class sqlrconnection_svr;
class sqlrcursor_svr;

class sqlrquery {
	public:
			sqlrquery(rudiments::xmldomnode *parameters);
		virtual	~sqlrquery();

		virtual bool	init(sqlrconnection_svr *sqlrcon);
		virtual bool	match(sqlrconnection_svr *sqlrcon,
						sqlrcursor_svr *sqlrcur,
						const char *querystring,
						uint32_t querylength);

		virtual bool	prepareQuery(const char *query,
						uint32_t length);
		virtual bool	supportsNativeBinds();
		virtual	bool	inputBindString(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint32_t valuesize,
						int16_t *isnull);
		virtual	bool	inputBindInteger(const char *variable, 
						uint16_t variablesize,
						int64_t *value);
		virtual	bool	inputBindDouble(const char *variable, 
						uint16_t variablesize,
						double *value,
						uint32_t precision,
						uint32_t scale);
		virtual bool	inputBindDate(const char *variable,
						uint16_t variablesize,
						int64_t year,
						int16_t month,
						int16_t day,
						int16_t hour,
						int16_t minute,
						int16_t second,
						int32_t microsecond,
						const char *tz,
						char *buffer,
						uint16_t buffersize,
						int16_t *isnull);
		virtual	bool	inputBindBlob(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint32_t valuesize,
						int16_t *isnull);
		virtual	bool	inputBindClob(const char *variable, 
						uint16_t variablesize,
						const char *value, 
						uint32_t valuesize,
						int16_t *isnull);
		virtual	bool	outputBindString(const char *variable, 
						uint16_t variablesize,
						char *value,
						uint16_t valuesize,
						int16_t *isnull);
		virtual	bool	outputBindInteger(const char *variable, 
						uint16_t variablesize,
						int64_t *value,
						int16_t *isnull);
		virtual	bool	outputBindDouble(const char *variable, 
						uint16_t variablesize,
						double *value,
						uint32_t *precision,
						uint32_t *scale,
						int16_t *isnull);
		virtual bool	outputBindDate(const char *variable,
						uint16_t variablesize,
						int16_t *year,
						int16_t *month,
						int16_t *day,
						int16_t *hour,
						int16_t *minute,
						int16_t *second,
						int32_t *microsecond,
						const char **tz,
						char *buffer,
						uint16_t buffersize,
						int16_t *isnull);
		virtual	bool	outputBindBlob(const char *variable, 
						uint16_t variablesize,
						uint16_t index,
						int16_t *isnull);
		virtual	bool	outputBindClob(const char *variable, 
						uint16_t variablesize,
						uint16_t index,
						int16_t *isnull);
		virtual	bool	outputBindCursor(const char *variable,
						uint16_t variablesize,
						sqlrcursor_svr *cursor);
		virtual	void	returnOutputBindCursor(uint16_t index);
		virtual bool	getLobOutputBindLength(uint16_t index,
							uint64_t *length);
		virtual bool	getLobOutputBindSegment(uint16_t index,
							char *buffer,
							uint64_t buffersize,
							uint64_t offset,
							uint64_t charstoread,
							uint64_t *charsread);
		virtual bool	executeQuery(const char *query,
						uint32_t length)=0;
		virtual bool	fetchFromBindCursor();
		virtual	bool	queryIsNotSelect();
		virtual	bool	queryIsCommitOrRollback();
		virtual bool	errorMessage(char *errorbuffer,
						uint32_t errorbuffersize,
						uint32_t *errorlength,
						int64_t *errorcode,
						bool *liveconnection);
		virtual bool		knowsRowCount();
		virtual uint64_t	rowCount();
		virtual uint64_t	affectedRows();
		virtual uint32_t	colCount();
		virtual const char * const *	columnNames();
		virtual uint16_t	columnTypeFormat();
		virtual void		returnColumnInfo();
		virtual bool		noRowsToReturn();
		virtual bool		skipRow();
		virtual bool		fetchRow();
		virtual bool		returnRow();
		virtual bool		nextRow();
		virtual void		getField(uint32_t col,
							const char **field,
							uint64_t *fieldlength,
							bool *blob,
							bool *null);
		virtual void		sendLobField(uint32_t col);
		virtual bool		getLobFieldLength(uint32_t col,
							uint64_t *length);
		virtual bool		getLobFieldSegment(uint32_t col,
							char *buffer,
							uint64_t buffersize,
							uint64_t offset,
							uint64_t charstoread,
							uint64_t *charsread);
		virtual void		cleanUpLobField(uint32_t col);
		virtual	void		cleanUpData(bool freeresult,
							bool freebinds);
		virtual bool		getColumnNameList(
					rudiments::stringbuffer *output);
	protected:
		rudiments::xmldomnode	*parameters;
};

#endif
