// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information

#ifndef SQLRIMPORTCSV_H
#define SQLRIMPORTCSV_H

#include <sqlrelay/private/sqlrimportcsvincludes.h>

class SQLRCLIENT_DLLSPEC sqlrimportcsv : public sqlrimport, public csvsax {
	public:
			sqlrimportcsv();
			~sqlrimportcsv();

		void	insertPrimaryKey(const char *primarykeycolumnname,
						uint32_t primarykeycolumnindex,
						const char *primarykeysequence);
		void	insertStaticValue(const char *columnname,
						uint32_t columnindex,
						const char *value);

		void	setIgnoreColumnsWithEmptyNames(
					bool ignorecolumnswithemptynames);
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
