#include <pqdefinitions.h>

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
	return NULL;
}

void PQconninfoFree(PQconninfoOption *connOptions) {
}

}
