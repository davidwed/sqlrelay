extern "C" {

typedef struct _PQconninfoOption {
	char	*keyword;
	char	*envvar;
	char	*compiled;
	char	*val;
	char	*label;
	char	*dispchar;
	int	dispsize;
} PQconninfoOption;

PQconninfoOption *PQconndefaults(void) {
}

void PQconninfoFree(PQconninfoOption *connOptions) {
}

}
