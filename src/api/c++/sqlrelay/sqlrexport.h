// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#ifndef SQLREXPORT_H
#define SQLREXPORT_H

#include <sqlrelay/private/sqlrexportincludes.h>

class sqlrexport {
	public:
			sqlrexport();
		virtual	~sqlrexport();

		void	setSqlrConnection(sqlrconnection *sqlrconnection);
		void	setSqlrCursor(sqlrcursor *sqlrcur);

		sqlrconnection	*getSqlrConnection();
		sqlrcursor	*getSqlrCursor();

		void	setIgnoreColumns(bool ignorecolumns);
		bool	getIgnoreColumns();

		void	setFieldsToIgnore(const char * const *fieldstoignore);
		const char * const *getFieldsToIgnore();

		void	setLogger(logger *lg);
		void	setCoarseLogLevel(uint8_t coarseloglevel);
		void	setFineLogLevel(uint8_t fineloglevel);
		void	setLogIndent(uint32_t logindent);

		logger		*getLogger();
		uint8_t		getCoarseLogLevel();
		uint8_t		getFineLogLevel();
		uint32_t	getLogIndent();

		void	setShutdownFlag(bool *shuttingdown);

		virtual	bool	exportToFile(const char *filename,
						const char *table)=0;

		virtual	bool	headerStart();
		virtual	bool	columnStart();
		virtual	bool	columnEnd();
		virtual	bool	headerEnd();
		virtual	bool	rowsStart();
		virtual	bool	rowStart();
		virtual	bool	fieldStart();
		virtual	bool	fieldEnd();
		virtual	bool	rowEnd();
		virtual	bool	rowsEnd();

		virtual uint64_t	getExportedRowCount();

	protected:
		void	setExportRow(bool exportrow);
		bool	getExportRow();

		void		setCurrentRow(uint64_t currentrow);
		uint64_t	getCurrentRow();

		void		setCurrentColumn(uint32_t currentcol);
		uint32_t	getCurrentColumn();

		void		setCurrentField(const char *currentfield);
		const char	*getCurrentField();

		void	setNumberColumn(uint64_t index, bool value);
		bool	getNumberColumn(uint64_t index);
		void	clearNumberColumns();

		void		setFileDescriptor(filedescriptor *fd);
		filedescriptor	*getFileDescriptor();

	#include <sqlrelay/private/sqlrexport.h>
};

#endif
