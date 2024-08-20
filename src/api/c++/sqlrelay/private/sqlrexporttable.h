// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

		virtual void	clearFlagsAndCounts();

		virtual bool	sanityCheck();
		virtual bool	startProcessingColumns();
		virtual bool	excludeThisColumn();
		virtual bool	exportColumnName(bool first);
		virtual bool	endProcessingColumns();
		virtual bool	startProcessingRows();
		virtual bool	startProcessingRow();
		virtual bool	startProcessingField();
		virtual bool	exportField(bool first);
		virtual bool	endProcessingField();
		virtual bool	endProcessingRow();
		virtual bool	endProcessingRows();

	private:
		bool	finalCommit();

		sqlrconnection		*exportcon;
		sqlrcursor		*exportcur;
		stringbuffer		insertquery;
		uint64_t		commitcount;
		char			bf;
		uint32_t		bindindex;
		dynamicarray<char *>	bindnames;
		bool			firstrow;
