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

void PQprint(FILE *fout, const PGresult *res, const PQprintOpt *ps) {
}

void PQdisplayTuples(const PGresult *res, FILE *fp, int fillAlign,
				const char *fieldSep, int printHeader,
				int quiet) {
}

void PQprintTuples(const PGresult *res, FILE *fout, int printAttName,
			  int terseOutput, int width) {
}

}
