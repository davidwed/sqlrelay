extern "C" {
	#ifdef __cplusplus
		#undef __cplusplus
		#define cpluspluswasdefined
	#endif
	#include "php.h"
	#ifdef cpluspluswasdefined
		#define __cplusplus
	#endif
}
