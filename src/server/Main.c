#include "Common.h"
#include "DbMysql.h"
#include "Service.h"
#include "Log.h"
#include "ProcessFuncs.h"

FILE * fp_log;

int 
main 
(int argc,char *argv[])
{
	fp_log = fopen(LOG_PATH, "a+");
	
	if (argc < 3)
	{
		printf("Usage ./xxx ip port\n");
		exit(1);
	}

	// fork a new process, and make it to be a daemon process
	setDaemon(fp_log);
	// fork a new process, and make it to run main service
	createMainProcess(fp_log, mainService, argv[1], argv[2]);
	// fork a new process, and make it to run heartbeat client
	createHeartbeatProcess(fp_log, heartbeatService, argv[1]);

	// daemon process, do something like watchdog
	for (;;)
	{
		sleep(3600);
	}
	
	return 0;
}
