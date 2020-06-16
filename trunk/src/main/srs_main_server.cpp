#include <srs_core_log.hpp>
#include <srs_core_error.hpp>
#include <pthread.h>
#include <srs_core_server.hpp>
// #include <srs_core_test.hpp>

#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv){
    // pthread_t thread;
    int ret = ERROR_SUCCESS;

    if (argc <= 1) {
		printf(RTMP_SIG_SRS_NAME " " RTMP_SIG_SRS_VERSION
			" Copyright (c) 2013 winlin\n"
			"Usage: %s <listen_port>\n" 
			"\n"
			RTMP_SIG_SRS_WEB"\n"
			RTMP_SIG_SRS_URL"\n"
			"Email: " RTMP_SIG_SRS_EMAIL "\n",
			argv[0]);
		exit(1);
	}

	int listen_port = ::atoi(argv[1]);
	srs_trace("listen_port=%d", listen_port);
	
	SrsServer server;

	ret = server.initialize();
	if (ret != ERROR_SUCCESS) {
		return ret;
	}

	if ((ret = server.listen(listen_port)) != ERROR_SUCCESS) {
		return ret;
	}

	if ((ret = server.cycle()) != ERROR_SUCCESS) {
		return ret;
	}
	
	return 0;
}