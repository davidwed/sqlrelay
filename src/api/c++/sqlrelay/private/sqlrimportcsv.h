// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

	private:
		void	appendField(stringbuffer *query,
					const char *value,
					uint32_t currentcol,
					bool overrideisstring);
		void	escapeField(stringbuffer *strb, const char *field);

		bool		insertprimarykey;
		char		*primarykeycolumnname;
		uint32_t	primarykeycolumnindex;
		char		*primarykeysequence;

		bool		ignorecolumnswithemptynames;
		bool		ignoreemptyrows;

		stringbuffer	query;
		uint32_t	colcount;
		uint32_t	currenttablecol;
		uint32_t	currentcol;
		bool		*numbercolumn;
		bool		*datecolumn;
		bool		foundfieldtext;
		uint32_t	fieldcount;
		uint64_t	rowcount;
		uint64_t	committedcount;

		linkedlist<uint32_t>		columnswithemptynames;
		listnode<uint32_t>		*columnswithemptynamesnode;

		dictionary<uint32_t, char *>	staticvaluecolumnnames;
		dictionary<uint32_t, char *>	staticvaluecolumnvalues;

		dynamicarray<char *>		columns;
		dynamicarray<char *>		fields;
