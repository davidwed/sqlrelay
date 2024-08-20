// Copyright (c) 1999-2018 David Muse
// See the file COPYING for more information.

	private:
		bool	tagStart(const char *ns, const char *name);
		bool	attributeName(const char *name);
		bool	attributeValue(const char *value);
		bool	text(const char *string);
		char	*unescapeValue(const char *value);
		bool	tagEnd(const char *ns, const char *name);
		bool	resetSequence();

		unsigned short	currenttag;
		char		*currentattribute;
		bool		infield;
		char		*sequencevalue;

		static const unsigned short	NULLTAG;
		static const unsigned short	TABLETAG;
		static const unsigned short	COLUMNSTAG;
		static const unsigned short	COLUMNTAG;
		static const unsigned short	ROWSTAG;
		static const unsigned short	ROWTAG;
		static const unsigned short	FIELDTAG;
		static const unsigned short	SEQUENCETAG;
