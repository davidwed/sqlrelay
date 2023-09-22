#include <rudiments/inetsocketclient.h>
#include <rudiments/stdio.h>

int main() {

	const char	*host="sqlrelay";
	uint16_t	port=9000;

	inetsocketclient	cl;
	cl.setHost(host);
	cl.setPort(port);
	uint32_t i=0;
	while (true) {
		stdoutput.printf("%d: ",i);
		if (!cl.connect()) {
			stdoutput.printf("failed\n");
			break;
		}
		stdoutput.printf("success\n");
		cl.close();
		i++;
	}
}
