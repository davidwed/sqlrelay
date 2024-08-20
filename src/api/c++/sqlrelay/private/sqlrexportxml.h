// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

	private:
		bool	startProcessingExport();
		bool	startProcessingColumns();
		bool	exportColumnName(bool first);
		bool	endProcessingColumns();
		bool	startProcessingRows();
		bool	startProcessingRow();
		bool	exportField(bool first);
		bool	endProcessingRow();
		bool	endProcessingRows();
		bool	endProcessingExport();

		bool	escapeValue(filedescriptor *fd, const char *field);
