#ifndef SRS_CORE_HPP
#define SRS_CORE_HPP

/*
#include <srs_core.hpp>
*/

/**
* the core provides the common defined macros, utilities,
* user must include the srs_core.hpp before any header, or maybe 
* build failed.
*/

// for int64_t print using PRId64 format.
#ifndef __STDC_FORMAT_MACROS
    #define __STDC_FORMAT_MACROS
#endif
#include <inttypes.h>

#include <assert.h>
#define srs_assert(expression) assert(expression)

#include <stddef.h>
#include <sys/types.h>

// free the p and set to NULL.
// p must be a T*.
#define srs_freep(p) \
	if (p) { \
		delete p; \
		p = NULL; \
	} \
	(void)0
// free the p which represents a array
#define srs_freepa(p) \
	if (p) { \
		delete[] p; \
		p = NULL; \
	} \
	(void)0

// server info.
#define RTMP_SIG_SRS_KEY "srs"
#define RTMP_SIG_SRS_NAME RTMP_SIG_SRS_KEY"(simple rtmp server)"
#define RTMP_SIG_SRS_URL "https://github.com/winlinvip/simple-rtmp-server"
#define RTMP_SIG_SRS_WEB "http://blog.csdn.net/win_lin"
#define RTMP_SIG_SRS_EMAIL "winterserver@126.com"
#define RTMP_SIG_SRS_VERSION "0.1"

// compare
#define srs_min(a, b) ((a < b)? a : b)

#endif