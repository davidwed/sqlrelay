#include <rudiments/inetsocketclient.h>
#include <rudiments/stdio.h>

int main() {
	for (int i=0; i<16384; i++) {
		inetsocketclient	cl;
		cl.connect("localhost",8009,-1,-1,0,0);
		cl.close();
		stdoutput.printf("%d\n",i);
	}
}
