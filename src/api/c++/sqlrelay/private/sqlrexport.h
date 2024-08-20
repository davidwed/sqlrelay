// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

		virtual void	clearFlagsAndCounts();

		virtual bool	startProcessingExport();
		virtual bool	sanityCheck();
		virtual bool	startProcessingColumns();
		virtual bool	excludeThisColumn();
		virtual bool	exportColumnName(bool first);
		virtual bool	startProcessingColumn();
		virtual bool	endProcessingColumn();
		virtual bool	endProcessingColumns();
		virtual bool	startProcessingRows();
		virtual bool	startProcessingRow();
		virtual bool	startProcessingField();
		virtual bool	excludeThisField();
		virtual bool	exportField(bool first);
		virtual bool	endProcessingField();
		virtual bool	endProcessingRow();
		virtual bool	endProcessingRows();
		virtual bool	endProcessingExport();

		virtual bool	flush();
		virtual bool	escapeValue(filedescriptor *fd,
						const char *value);
		virtual bool	systemError();

	private:

		sqlrconnection		*sqlrcon;
		sqlrcursor		*sqlrcur;

		const char		*table;

		bool			excludecolumns;
		const char * const	*columnstoexclude;

		logger			*lg;
		uint8_t			coarseloglevel;
		uint8_t			fineloglevel;
		uint32_t		logindent;
		bool			logerrors;

		bool			excluderow;
		uint64_t		currentrow;
		uint32_t		currentcol;
		const char		*currentcolname;
		const char		*currentfield;
		uint64_t		exportedrowcount;
		dynamicarray<bool>	numericcolumn;
