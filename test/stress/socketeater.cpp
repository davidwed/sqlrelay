#include <rudiments/inetclientsocket.h>

#ifdef RUDIMENTS_NAMESPACE
using namespace rudiments;
#endif

int main() {
	for (int i=0; i<16384; i++) {
		inetclientsocket	cl;
		cl.connect("localhost",8009,-1,-1,0,0);
		cl.close();
		printf("%d\n",i);
	}
}
