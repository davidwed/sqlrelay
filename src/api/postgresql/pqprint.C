/*
Portions Copyright (c) 1996-2002, The PostgreSQL Global Development Group
 
Portions Copyright (c) 1994, The Regents of the University of California

Portions Copyright (c) 2003-2004, David Muse

Permission to use, copy, modify, and distribute this software and its documentation for any purpose, without fee, and without a written agreement is hereby granted, provided that the above copyright notice and this paragraph and the following two paragraphs appear in all copies.
 
IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 
THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATIONS TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
*/

#include <pqdefinitions.h>
#include <rudiments/charstring.h>
#include <rudiments/process.h>

#ifdef WIN32
#include "win32.h"
#else
// for isatty()
#include <unistd.h>
#endif

extern "C" {

typedef struct _PQprintOpt {
	char	header;	
	char	align;
	char	standard;
	char	html3;
	char	expanded;
	char	pager;
	char	*fieldSep;
	char	*tableOpt;
	char	*caption;
	char	**fieldName;
} PQprintOpt;

// Haha!  I stole these straight out of the postgresql source

/*
PostgreSQL is Copyright © 1996-2002 by the PostgreSQL Global Development Group
and is distributed under the terms of the license of the University of
California below.

Postgres95 is Copyright © 1994-5 by the Regents of the University of California.

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose, without fee, and without a written agreement is
hereby granted, provided that the above copyright notice and this paragraph and
the following two paragraphs appear in all copies.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST
PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE. THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS-IS" BASIS, AND
THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATIONS TO PROVIDE MAINTENANCE,
SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS. 
*/

static void do_field(const PQprintOpt *po, const PGresult *res,
		 const int i, const int j, const int fs_len,
		 char **fields,
		 const int nFields, const char **fieldNames,
		 unsigned char *fieldNotNum, int *fieldMax,
		 const int fieldMaxLen, FILE *fout);
static char *do_header(FILE *fout, const PQprintOpt *po, const int nFields,
	  int *fieldMax, const char **fieldNames, unsigned char *fieldNotNum,
		  const int fs_len, const PGresult *res);
static void output_row(FILE *fout, const PQprintOpt *po, const int nFields, char **fields,
		   unsigned char *fieldNotNum, int *fieldMax, char *border,
		   const int row_index);
static void fill(int length, int max, char filler, FILE *fp);

/*
 * PQprint()
 *
 * Format results of a query for printing.
 *
 * PQprintOpt is a typedef (structure) that containes
 * various flags and options. consult libpq-fe.h for
 * details
 *
 * This function should probably be removed sometime since psql
 * doesn't use it anymore. It is unclear to what extend this is used
 * by external clients, however.
 */

void
PQprint(FILE *fout,
		const PGresult *res,
		const PQprintOpt *po)
{
	debugFunction();
	int			nFields;

	nFields = PQnfields(res);

	if (nFields > 0)
	{							/* only print rows with at least 1 field.  */
		int			i,
					j;
		int			nTups;
		int		   *fieldMax = NULL;	/* in case we don't use them */
		unsigned char *fieldNotNum = NULL;
		char	   *border = NULL;
		char	  **fields = NULL;
		const char **fieldNames;
		int			fieldMaxLen = 0;
		int			numFieldName;
		int			fs_len = charstring::length(po->fieldSep);
		int			total_line_length = 0;
		int			usePipe = 0;
		//pqsigfunc	oldsigpipehandler = NULL;
		char	   *pagerenv;

#ifdef TIOCGWINSZ
		struct winsize screen_size;

#else
		struct winsize
		{
			int			ws_row;
			int			ws_col;
		}			screen_size;
#endif

		nTups = PQntuples(res);
		if (!(fieldNames = (const char **) calloc(nFields, sizeof(char *))))
		{
			perror("calloc");
			process::exit(1);
		}
		if (!(fieldNotNum = (unsigned char *) calloc(nFields, 1)))
		{
			perror("calloc");
			process::exit(1);
		}
		if (!(fieldMax = (int *) calloc(nFields, sizeof(int))))
		{
			perror("calloc");
			process::exit(1);
		}
		for (numFieldName = 0;
			 po->fieldName && po->fieldName[numFieldName];
			 numFieldName++)
			;
		for (j = 0; j < nFields; j++)
		{
			int			len;
			const char *s = (j < numFieldName && po->fieldName[j][0]) ?
			po->fieldName[j] : PQfname(res, j);

			fieldNames[j] = s;
			len = s ? charstring::length(s) : 0;
			fieldMax[j] = len;
			len += fs_len;
			if (len > fieldMaxLen)
				fieldMaxLen = len;
			total_line_length += len;
		}

		total_line_length += nFields * charstring::length(po->fieldSep) + 1;

		if (fout == NULL)
			fout = stdout;
		if (po->pager && fout == stdout
#ifndef WIN32
			&&
			isatty(fileno(stdin)) &&
			isatty(fileno(stdout))
#endif
			)
		{
			/*
			 * If we think there'll be more than one screen of output, try
			 * to pipe to the pager program.
			 */
/*#ifdef TIOCGWINSZ
			if (ioctl(fileno(stdout), TIOCGWINSZ, &screen_size) == -1 ||
				screen_size.ws_col == 0 ||
				screen_size.ws_row == 0)
			{
				screen_size.ws_row = 24;
				screen_size.ws_col = 80;
			}
#else*/
			screen_size.ws_row = 24;
			screen_size.ws_col = 80;
//#endif
			pagerenv = getenv("PAGER");
			if (pagerenv != NULL &&
				pagerenv[0] != '\0' &&
				!po->html3 &&
				((po->expanded &&
				  nTups * (nFields + 1) >= screen_size.ws_row) ||
				 (!po->expanded &&
				  nTups * (total_line_length / screen_size.ws_col + 1) *
				  (1 + (po->standard != 0)) >= screen_size.ws_row -
				  (po->header != 0) *
				  (total_line_length / screen_size.ws_col + 1) * 2
				  - (po->header != 0) * 2		/* row count and newline */
				  )))
			{
#ifdef WIN32
				fout = _popen(pagerenv, "w");
#else
				fout = popen(pagerenv, "w");
#endif
				if (fout)
				{
					usePipe = 1;
#ifndef WIN32
					//oldsigpipehandler = pqsignal(SIGPIPE, SIG_IGN);
#endif
				}
				else
					fout = stdout;
			}
		}

		if (!po->expanded && (po->align || po->html3))
		{
			if (!(fields = (char **) calloc(nFields * (nTups + 1), sizeof(char *))))
			{
				perror("calloc");
				process::exit(1);
			}
		}
		else if (po->header && !po->html3)
		{
			if (po->expanded)
			{
				if (po->align)
					fprintf(fout, "%-*s%s Value\n",
							fieldMaxLen - fs_len, "Field", po->fieldSep);
				else
					fprintf(fout, "%s%sValue\n", "Field", po->fieldSep);
			}
			else
			{
				int			len = 0;

				for (j = 0; j < nFields; j++)
				{
					const char *s = fieldNames[j];

					fputs(s, fout);
					len += charstring::length(s) + fs_len;
					if ((j + 1) < nFields)
						fputs(po->fieldSep, fout);
				}
				fputc('\n', fout);
				for (len -= fs_len; len--; fputc('-', fout));
				fputc('\n', fout);
			}
		}
		if (po->expanded && po->html3)
		{
			if (po->caption)
				fprintf(fout, "<centre><h2>%s</h2></centre>\n", po->caption);
			else
				fprintf(fout,
						"<centre><h2>"
						"Query retrieved %d rows * %d fields"
						"</h2></centre>\n",
						nTups, nFields);
		}
		for (i = 0; i < nTups; i++)
		{
			if (po->expanded)
			{
				if (po->html3)
					fprintf(fout,
						  "<table %s><caption align=high>%d</caption>\n",
							po->tableOpt ? po->tableOpt : "", i);
				else
					fprintf(fout, "-- RECORD %d --\n", i);
			}
			for (j = 0; j < nFields; j++)
				do_field(po, res, i, j, fs_len, fields, nFields,
						 fieldNames, fieldNotNum,
						 fieldMax, fieldMaxLen, fout);
			if (po->html3 && po->expanded)
				fputs("</table>\n", fout);
		}
		if (!po->expanded && (po->align || po->html3))
		{
			if (po->html3)
			{
				if (po->header)
				{
					if (po->caption)
						fprintf(fout,
						  "<table %s><caption align=high>%s</caption>\n",
								po->tableOpt ? po->tableOpt : "",
								po->caption);
					else
						fprintf(fout,
								"<table %s><caption align=high>"
								"Retrieved %d rows * %d fields"
								"</caption>\n",
						po->tableOpt ? po->tableOpt : "", nTups, nFields);
				}
				else
					fprintf(fout, "<table %s>", po->tableOpt ? po->tableOpt : "");
			}
			if (po->header)
				border = do_header(fout, po, nFields, fieldMax, fieldNames,
								   fieldNotNum, fs_len, res);
			for (i = 0; i < nTups; i++)
				output_row(fout, po, nFields, fields,
						   fieldNotNum, fieldMax, border, i);
			free(fields);
			if (border)
				free(border);
		}
		if (po->header && !po->html3)
			fprintf(fout, "(%d row%s)\n\n", PQntuples(res),
					(PQntuples(res) == 1) ? "" : "s");
		free(fieldMax);
		free(fieldNotNum);
		free((void *) fieldNames);
		if (usePipe)
		{
#ifdef WIN32
			_pclose(fout);
#else
			pclose(fout);
			//pqsignal(SIGPIPE, oldsigpipehandler);
#endif
		}
		if (po->html3 && !po->expanded)
			fputs("</table>\n", fout);
	}
}


static void
do_field(const PQprintOpt *po, const PGresult *res,
		 const int i, const int j, const int fs_len,
		 char **fields,
		 const int nFields, char const ** fieldNames,
		 unsigned char *fieldNotNum, int *fieldMax,
		 const int fieldMaxLen, FILE *fout)
{
	debugFunction();

	const char *pval,
			   *p;
	int			plen;
	bool		skipit;

	plen = PQgetlength(res, i, j);
	pval = PQgetvalue(res, i, j);

	if (plen < 1 || !pval || !*pval)
	{
		if (po->align || po->expanded)
			skipit = true;
		else
		{
			skipit = false;
			goto efield;
		}
	}
	else
		skipit = false;

	if (!skipit)
	{
		if (po->align && !fieldNotNum[j])
		{
			/* Detect whether field contains non-numeric data */
			char		ch = '0';

			for (p = pval; *p; p += PQmblen((unsigned char *)p, PQclientEncoding(res->parent)))
			{
				ch = *p;
				if (!((ch >= '0' && ch <= '9') ||
					  ch == '.' ||
					  ch == 'E' ||
					  ch == 'e' ||
					  ch == ' ' ||
					  ch == '-'))
				{
					fieldNotNum[j] = 1;
					break;
				}
			}

			/*
			 * Above loop will believe E in first column is numeric; also,
			 * we insist on a digit in the last column for a numeric. This
			 * test is still not bulletproof but it handles most cases.
			 */
			if (*pval == 'E' || *pval == 'e' ||
				!(ch >= '0' && ch <= '9'))
				fieldNotNum[j] = 1;
		}

		if (!po->expanded && (po->align || po->html3))
		{
			if (plen > fieldMax[j])
				fieldMax[j] = plen;
			if (!(fields[i * nFields + j] = (char *) malloc(plen + 1)))
			{
				perror("malloc");
				process::exit(1);
			}
			charstring::copy(fields[i * nFields + j], pval);
		}
		else
		{
			if (po->expanded)
			{
				if (po->html3)
					fprintf(fout,
							"<tr><td align=left><b>%s</b></td>"
							"<td align=%s>%s</td></tr>\n",
							fieldNames[j],
							fieldNotNum[j] ? "left" : "right",
							pval);
				else
				{
					if (po->align)
						fprintf(fout,
								"%-*s%s %s\n",
								fieldMaxLen - fs_len, fieldNames[j],
								po->fieldSep,
								pval);
					else
						fprintf(fout,
								"%s%s%s\n",
								fieldNames[j], po->fieldSep, pval);
				}
			}
			else
			{
				if (!po->html3)
				{
					fputs(pval, fout);
			efield:
					if ((j + 1) < nFields)
						fputs(po->fieldSep, fout);
					else
						fputc('\n', fout);
				}
			}
		}
	}
}


static char *
do_header(FILE *fout, const PQprintOpt *po, const int nFields, int *fieldMax,
		  const char **fieldNames, unsigned char *fieldNotNum,
		  const int fs_len, const PGresult *res)
{
	debugFunction();

	int			j;				/* for loop index */
	char	   *border = NULL;

	if (po->html3)
		fputs("<tr>", fout);
	else
	{
		int			tot = 0;
		int			n = 0;
		char	   *p = NULL;

		for (; n < nFields; n++)
			tot += fieldMax[n] + fs_len + (po->standard ? 2 : 0);
		if (po->standard)
			tot += fs_len * 2 + 2;
		border = (char *)malloc(tot + 1);
		if (!border)
		{
			perror("malloc");
			process::exit(1);
		}
		p = border;
		if (po->standard)
		{
			char	   *fs = po->fieldSep;

			while (*fs++)
				*p++ = '+';
		}
		for (j = 0; j < nFields; j++)
		{
			int			len;

			for (len = fieldMax[j] + (po->standard ? 2 : 0); len--; *p++ = '-');
			if (po->standard || (j + 1) < nFields)
			{
				char	   *fs = po->fieldSep;

				while (*fs++)
					*p++ = '+';
			}
		}
		*p = '\0';
		if (po->standard)
			fprintf(fout, "%s\n", border);
	}
	if (po->standard)
		fputs(po->fieldSep, fout);
	for (j = 0; j < nFields; j++)
	{
		const char *s = PQfname(res, j);

		if (po->html3)
		{
			fprintf(fout, "<th align=%s>%s</th>",
					fieldNotNum[j] ? "left" : "right", fieldNames[j]);
		}
		else
		{
			int			n = charstring::length(s);

			if (n > fieldMax[j])
				fieldMax[j] = n;
			if (po->standard)
				fprintf(fout,
						fieldNotNum[j] ? " %-*s " : " %*s ",
						fieldMax[j], s);
			else
				fprintf(fout, fieldNotNum[j] ? "%-*s" : "%*s", fieldMax[j], s);
			if (po->standard || (j + 1) < nFields)
				fputs(po->fieldSep, fout);
		}
	}
	if (po->html3)
		fputs("</tr>\n", fout);
	else
		fprintf(fout, "\n%s\n", border);
	return border;
}


static void
output_row(FILE *fout, const PQprintOpt *po, const int nFields, char **fields,
		   unsigned char *fieldNotNum, int *fieldMax, char *border,
		   const int row_index)
{
	debugFunction();

	int			field_index;	/* for loop index */

	if (po->html3)
		fputs("<tr>", fout);
	else if (po->standard)
		fputs(po->fieldSep, fout);
	for (field_index = 0; field_index < nFields; field_index++)
	{
		char	   *p = fields[row_index * nFields + field_index];

		if (po->html3)
			fprintf(fout, "<td align=%s>%s</td>",
				fieldNotNum[field_index] ? "left" : "right", p ? p : "");
		else
		{
			fprintf(fout,
					fieldNotNum[field_index] ?
					(po->standard ? " %-*s " : "%-*s") :
					(po->standard ? " %*s " : "%*s"),
					fieldMax[field_index],
					p ? p : "");
			if (po->standard || field_index + 1 < nFields)
				fputs(po->fieldSep, fout);
		}
		if (p)
			free(p);
	}
	if (po->html3)
		fputs("</tr>", fout);
	else if (po->standard)
		fprintf(fout, "\n%s", border);
	fputc('\n', fout);
}



/*
 * really old printing routines
 */

void
PQdisplayTuples(const PGresult *res,
				FILE *fp,		/* where to send the output */
				int fillAlign,	/* pad the fields with spaces */
				const char *fieldSep,	/* field separator */
				int printHeader,	/* display headers? */
				int quiet
)
{
	debugFunction();
#define DEFAULT_FIELD_SEP " "

	int			i,
				j;
	int			nFields;
	int			nTuples;
	int		   *fLength = NULL;

	if (fieldSep == NULL)
		fieldSep = DEFAULT_FIELD_SEP;

	/* Get some useful info about the results */
	nFields = PQnfields(res);
	nTuples = PQntuples(res);

	if (fp == NULL)
		fp = stdout;

	/* Figure the field lengths to align to */
	/* will be somewhat time consuming for very large results */
	if (fillAlign)
	{
		fLength = (int *) malloc(nFields * sizeof(int));
		for (j = 0; j < nFields; j++)
		{
			fLength[j] = charstring::length(PQfname(res, j));
			for (i = 0; i < nTuples; i++)
			{
				int			flen = PQgetlength(res, i, j);

				if (flen > fLength[j])
					fLength[j] = flen;
			}
		}
	}

	if (printHeader)
	{
		/* first, print out the attribute names */
		for (i = 0; i < nFields; i++)
		{
			fputs(PQfname(res, i), fp);
			if (fillAlign)
				fill(charstring::length(PQfname(res, i)), fLength[i], ' ', fp);
			fputs(fieldSep, fp);
		}
		fprintf(fp, "\n");

		/* Underline the attribute names */
		for (i = 0; i < nFields; i++)
		{
			if (fillAlign)
				fill(0, fLength[i], '-', fp);
			fputs(fieldSep, fp);
		}
		fprintf(fp, "\n");
	}

	/* next, print out the instances */
	for (i = 0; i < nTuples; i++)
	{
		for (j = 0; j < nFields; j++)
		{
			fprintf(fp, "%s", PQgetvalue(res, i, j));
			if (fillAlign)
				fill(charstring::length(PQgetvalue(res, i, j)), fLength[j], ' ', fp);
			fputs(fieldSep, fp);
		}
		fprintf(fp, "\n");
	}

	if (!quiet)
		fprintf(fp, "\nQuery returned %d row%s.\n", PQntuples(res),
				(PQntuples(res) == 1) ? "" : "s");

	fflush(fp);

	if (fLength)
		free(fLength);
}



void
PQprintTuples(const PGresult *res,
			  FILE *fout,		/* output stream */
			  int PrintAttNames,	/* print attribute names or not */
			  int TerseOutput,	/* delimiter bars or not? */
			  int colWidth		/* width of column, if 0, use variable
								 * width */
)
{
	debugFunction();

	int			nFields;
	int			nTups;
	int			i,
				j;
	char		formatString[80];

	char	   *tborder = NULL;

	nFields = PQnfields(res);
	nTups = PQntuples(res);

	if (colWidth > 0)
		sprintf(formatString, "%%s %%-%ds", colWidth);
	else
		sprintf(formatString, "%%s %%s");

	if (nFields > 0)
	{							/* only print rows with at least 1 field.  */

		if (!TerseOutput)
		{
			int			width;

			width = nFields * 14;
			tborder = (char *)malloc(width + 1);
			for (i = 0; i <= width; i++)
				tborder[i] = '-';
			tborder[i] = '\0';
			fprintf(fout, "%s\n", tborder);
		}

		for (i = 0; i < nFields; i++)
		{
			if (PrintAttNames)
			{
				fprintf(fout, formatString,
						TerseOutput ? "" : "|",
						PQfname(res, i));
			}
		}

		if (PrintAttNames)
		{
			if (TerseOutput)
				fprintf(fout, "\n");
			else
				fprintf(fout, "|\n%s\n", tborder);
		}

		for (i = 0; i < nTups; i++)
		{
			for (j = 0; j < nFields; j++)
			{
				const char *pval = PQgetvalue(res, i, j);

				fprintf(fout, formatString,
						TerseOutput ? "" : "|",
						pval ? pval : "");
			}
			if (TerseOutput)
				fprintf(fout, "\n");
			else
				fprintf(fout, "|\n%s\n", tborder);
		}
	}
}



/* simply send out max-length number of filler characters to fp */

static void
fill(int length, int max, char filler, FILE *fp)
{
	debugFunction();

	int			count;

	count = max - length;
	while (count-- >= 0)
		putc(filler, fp);
}

}
