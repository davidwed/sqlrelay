// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

	private:
		void	escapeField(stringbuffer *strb, const char *field);

		uint32_t	primarykeyposition;
		char		*primarykeyname;
		char		*primarykeysequence;

		bool		ignorecolumnswithemptynames;
		bool		ignoreemptyrows;

		stringbuffer	query;
		stringbuffer	columns;
		uint32_t	colcount;
		uint32_t	currentcol;
		bool		*numbercolumn;
		bool		*datecolumn;
		bool		foundfieldtext;
		uint32_t	fieldcount;
		uint64_t	rowcount;
		uint64_t	committedcount;
		bool		needcomma;

		linkedlist<uint32_t>		columnswithemptynames;
		linkedlistnode<uint32_t>	*columnswithemptynamesnode;
