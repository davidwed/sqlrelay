// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

	private:

		bool	exportColumnName(bool first);
		bool	endProcessingColumns();
		bool	exportField(bool first);
		bool	endProcessingRow();

		bool	escapeValue(filedescriptor *fd, const char *field);
