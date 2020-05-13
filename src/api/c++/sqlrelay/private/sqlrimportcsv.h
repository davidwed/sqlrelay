// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

	private:
		bool	column(const char *name, bool quoted);
		bool	headerEnd();
		bool	bodyStart();
		bool	rowStart();
		bool	field(const char *value, bool quoted);
		bool	rowEnd();
		bool	bodyEnd();

		void	escapeField(stringbuffer *strb, const char *field);

		stringbuffer	query;
		stringbuffer	columns;
		uint32_t	colcount;
		uint32_t	currentcol;
		bool		*numbercolumn;
		bool		foundfieldtext;
		uint32_t	fieldcount;
		uint64_t	rowcount;
		uint64_t	committedcount;
