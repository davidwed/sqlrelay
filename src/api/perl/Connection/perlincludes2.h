#include "../../c++/include/sqlrelay/sqlrclient.h"
#include <EXTERN.h>
extern "C" {
	#undef __cplusplus
	#include <perl.h>
	#define __cplusplus
}
#include <XSUB.h>
