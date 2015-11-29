#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <time.h>

#define LOG_PATH "chat4lan.log"

#define STR_CAT(x, y) (x y)
#define LOG_PREFIX_PATTERN "%02d-%02d-%02d %02d:%02d:%02d.%03lu|%s|%lu|"
#define LOGGER(fp, timeval, tm, format, level, ...){\
	gettimeofday(&timeval, NULL);		\
	localtime_r(&(timeval.tv_sec), &tm);	\
	fprintf (fp, STR_CAT(LOG_PREFIX_PATTERN, format), \
		tm.tm_year+1900,		\
		tm.tm_mon+1,			\
		tm.tm_mday,			\
		tm.tm_hour,			\
		tm.tm_min,			\
		tm.tm_sec,			\
		timeval.tv_usec/1000,	\
		level,				\
		pthread_self(),			\
		##__VA_ARGS__);		\
		fflush(fp);				\
}

#define DEBUG_LOG(fp, timeval, tm, format, ...)\
	LOGGER(fp, timeval, tm, format, "DEBUG", ##__VA_ARGS__)

#define INFO_LOG(fp, timeval, tm, format, ...)\
	LOGGER(fp, timeval, tm, format, "INFO", ##__VA_ARGS__)

#define WARN_LOG(fp, timeval, tm, format, ...)\
	LOGGER(fp, timeval, tm, format, "WARN", ##__VA_ARGS__)

#define ERROR_LOG(fp, timeval, tm, format, ...)\
	LOGGER(fp, timeval, tm, format, "ERROR", ##__VA_ARGS__)

