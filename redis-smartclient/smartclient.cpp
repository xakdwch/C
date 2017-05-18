#include <iostream>
#include <map>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <hiredis.h>

using std::string;
using std::map;

pthread_cond_t masterIsDown = PTHREAD_COND_INITIALIZER;
pthread_cond_t newMaster = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

const char *sentinels[3] = {"127.0.0.1:26379", "127.0.0.1:26380", "127.0.0.1:26381"};
const string masterName = "mymaster";
map<string, string> routeMap;

const int max_retries = 60;
bool masterIsRunning = true;

void *listenSentinel(void *args)
{
	const char *sentinel = (const char *)args;
    char sentinelIP[16] = "";
    int sentinelPort = 0;
	int retries = 0;
	pthread_t self = pthread_self();

    sscanf(sentinel, "%[0-9,.]:%d", sentinelIP, &sentinelPort);

	while (1) {
		pthread_mutex_lock(&lock);
		if (retries == 3) {
			retries = 0;
			masterIsRunning = true;
			pthread_cond_signal(&newMaster);
			printf("[%ld] %d: retry too many times!\n", self, __LINE__);
			pthread_mutex_unlock(&lock);
			sleep(3);
			pthread_mutex_lock(&lock);
		}

		while (masterIsRunning){
			pthread_cond_wait(&masterIsDown, &lock);
		}

    	struct timeval timeout = {1, 500000};
    	redisContext *ctx = redisConnectWithTimeout(sentinelIP, sentinelPort, timeout);
    	if (!ctx || ctx->err) {
        	if (ctx) {
            	printf("[%ld] %d: connection error: %s\n", self, __LINE__, ctx->errstr);
            	redisFree(ctx);
        	}
        	else {
            	printf("[%ld] %d: can't allocate redis context\n", self, __LINE__);
        	}
			retries++;
			pthread_mutex_unlock(&lock);
        	continue;
    	}

    	redisReply *reply = (redisReply*)redisCommand(ctx, "sentinel get-master-addr-by-name mymaster");
    	if (!reply || reply->type != REDIS_REPLY_ARRAY || reply->elements != 2) {
			printf("[%ld] %d: get master addr error!\n", self, __LINE__);
			if (reply)
				freeReplyObject(reply);
			redisFree(ctx);
			retries++;
			pthread_mutex_unlock(&lock);
			continue;
    	}

		string masterIP = reply->element[0]->str;
		string masterPort = reply->element[1]->str;
		string newMasterAddr =  masterIP + ":" + masterPort;
		if (routeMap[masterName].compare(newMasterAddr) != 0) {
			routeMap[masterName] = newMasterAddr;
		}

		masterIsRunning = true;
		pthread_cond_signal(&newMaster);
		printf("[%ld] %d: new master is selected!\n", self, __LINE__);
		pthread_mutex_unlock(&lock);
		redisFree(ctx);
	}

	return (void *)0;
}

int initRouteMap(void)
{
	int i = 0;
    struct timeval timeout = {0, 500000};
	redisContext *ctx = NULL;

	for (i = 0; i < 3; i++) {
        char sentinelIP[16] = "";
        int sentinelPort = 0;

        sscanf(sentinels[i], "%[0-9,.]:%d", sentinelIP, &sentinelPort);
        ctx = redisConnectWithTimeout(sentinelIP, sentinelPort, timeout);
        if (!ctx || ctx->err) {
			printf("%d: connect %s failed!\n", __LINE__, sentinels[i]);
			continue;
		}
		break;
    }

	if (i == 3) {
		printf("%d: no sentinel is running, exiting!\n", __LINE__);
		return -1;
	}

	redisReply *reply = (redisReply *)redisCommand(ctx, "sentinel get-master-addr-by-name mymaster");
	if (!reply || reply->type != REDIS_REPLY_ARRAY || reply->elements != 2) {
		printf("%d: get master addr error!\n", __LINE__);
		return -1;
	}

	string masterIP = reply->element[0]->str;
	string masterPort = reply->element[1]->str;
	routeMap[masterName] =  masterIP + ":" + masterPort;

	return 0;
}

redisContext *redisConnectNewMaster(void)
{
	char masterIP[16] = "";
	int masterPort = 0;
	redisContext *ctx = NULL;
	struct timeval timeout = {0, 500000};
	int retries = 0;

	while (retries < max_retries) {
		pthread_mutex_lock(&lock);
		masterIsRunning = false;
		pthread_cond_signal(&masterIsDown);
		printf("%d: master is down!\n", __LINE__);

		while (!masterIsRunning) {
			pthread_cond_wait(&newMaster, &lock);
		}

		sscanf(routeMap[masterName].c_str(), "%[0-9,.]:%d", masterIP, &masterPort);
		ctx = redisConnectWithTimeout(masterIP, masterPort, timeout);
		if (!ctx || ctx->err) {
			if (ctx) {
				redisFree(ctx);
			}
			printf("%d: %d --> connect %s failed!\n", __LINE__, retries, routeMap[masterName].c_str());
			retries++;
			pthread_mutex_unlock(&lock);
			sleep(1);
			continue;
		}
		else {
			printf("%d: new master %s\n", __LINE__, routeMap[masterName].c_str());
			pthread_mutex_unlock(&lock);
			return ctx;
		}
	}

	return NULL;
}

void freeRes(pthread_t tids[])
{
	int i;

	for (i = 0; i < 3; i++) {
		pthread_cancel(tids[i]);
	}

	pthread_mutex_destroy(&lock);
	pthread_cond_destroy(&newMaster);
	pthread_cond_destroy(&masterIsDown);
}

int main(int args, char **argv)
{
	unsigned int i = 0;
	pthread_t tid[3];
	struct timeval timeout = {0, 500000};
	char masterIP[16] = "";
	int masterPort = 0;
	redisContext *ctx = NULL;

	if (initRouteMap() < 0) {
		return -1;
	}

	for (i = 0; i < 3; i++) {
		pthread_create(&tid[i], NULL, listenSentinel, (void *)sentinels[i]);
	}

	sscanf(routeMap[masterName].c_str(), "%[0-9,.]:%d", masterIP, &masterPort);
	ctx = redisConnectWithTimeout(masterIP, masterPort, timeout);
	if (!ctx || ctx->err) {
		printf("%d: connect %s failed!\n", __LINE__, routeMap[masterName].c_str());
		if (ctx)
			redisFree(ctx);

		if ((ctx = redisConnectNewMaster()) == NULL) {
			return -1;
		}
	}

	while (1) {
		redisReply *reply = (redisReply *)redisCommand(ctx, "INFO Replication");
		if (reply == NULL) {
			if (ctx)
				redisFree(ctx);

			if ((ctx = redisConnectNewMaster()) == NULL) {
				break;
			}
		}
		else {
			//printf("[INFO]: %s\n", reply->str);
			freeReplyObject(reply);
		}
		sleep(1);
	}

	freeRes(tid);
	return 0;
}
