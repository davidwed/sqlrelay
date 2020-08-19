// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#ifndef SQLRIMPORTCSV_H
#define SQLRIMPORTCSV_H

#include <sqlrelay/private/sqlrimportcsvincludes.h>

class sqlrimportcsv : public sqlrimport, public csvsax {
	public:
			sqlrimportcsv();
			~sqlrimportcsv();

		void	setPrimaryKeyName(const char *primarykeyname);
		void	setPrimaryKeyPosition(uint32_t primarykeyposition);
		void	setPrimaryKeySequence(const char *primarykeysequence);

		void	setIgnoreColumnsWithEmptyNames(bool ignorecolumnswithemptynames);
		void	setIgnoreEmptyRows(bool ignoreemptyrows);

		bool	importFromFile(const char *filename);

	protected:
		virtual	bool	column(const char *name, bool quoted);
		virtual	bool	headerEnd();
		virtual	bool	bodyStart();
		virtual	bool	rowStart();
		virtual	bool	field(const char *value, bool quoted);
		virtual	bool	rowEnd();
		virtual	bool	bodyEnd();

	#include <sqlrelay/private/sqlrimportcsv.h>
};

#endif
