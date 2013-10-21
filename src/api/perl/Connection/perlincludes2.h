/*#include "../../c++/include/sqlrelay/sqlrclient.h"
#include <EXTERN.h>
extern "C" {
	#undef __cplusplus
	#include <perl.h>
	#define __cplusplus
}
#include <XSUB.h>*/
#include "../../c++/include/sqlrelay/sqlrclient.h"
#include <EXTERN.h>
extern "C" {
	/*#ifdef __cplusplus
		#define __cplusplus_was_set
		#undef __cplusplus
	#endif*/
	#include <perl.h>
	/*#ifdef __cplusplus_was_set
		#define __cplusplus
		#undef __cplusplus_was_set
	#endif*/
}
#include <XSUB.h>
#ifdef CLASS
	#undef CLASS
#endif
