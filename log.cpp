
/*
 * log.cpp
 *
 *  Created on: 2017年9月10日
 *      Author: michael
 */

#include "log.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <thread>
#include <iostream>

CLog::CLog(const std::string& slogname):
nIntervalTime(5)
,nCacheLength(LEN)
,sLogName(slogname)
,sSuffix({".log"})
,sPrefix(GetDayTime())
,nCurLogLevel(0)
,bRun(1)
{

	fLogFile.open(GetFullName().c_str());
	log({"log init over\n"}, INFO, __FILE__, __LINE__);
	std::thread t1(&CLog::flush, this);
	mThread.swap(t1);

}

CLog::~CLog()
{
	log({"log end\n"}, INFO, __FILE__, __LINE__);
	fLogFile.close();
	mThread.join();
}

void CLog::log(const std::string& sLogInfo, uint8_t loglevel,
		const std::string& sCodeFile, uint16_t nline)
{

	mQueueLog.enqueue(LogInfo(sLogInfo, loglevel, sCodeFile, nline));

}


void CLog::flush()
{
	LogInfo tmp;
	uint16_t start = time(NULL);
	uint16_t end = time(NULL);

	while(bRun)
	{
		usleep(100);
		end = time(NULL);
		if(nIntervalTime > (end - start))
			continue;
		while(mQueueLog.try_dequeue(tmp))
		{
			WriteFile(tmp);
		}
		end = start = time(NULL);
	}
}

std::string CLog::GetDayTime()
{
	time_t now = time(NULL);
	struct tm tt = {};
	localtime_r(&now, &tt);
	char * retstr = new char[9];
	strftime(retstr, 8, "%Y%m%d", &tt);
	retstr[8] = '\0';
	return {retstr};

}

std::string CLog::GetFullName()
{
	return {sPrefix + sLogName + sSuffix};
}

void CLog::FlushAll(void)
{
	LogInfo tmp;
	while(mQueueLog.try_dequeue(tmp))
	{
		WriteFile(tmp);
	}
}

void CLog::WriteFile(const LogInfo& info)
{
	if(nCurLogLevel <= info.nLogLevel)
	{
		fLogFile.write(Info[info.nLogLevel].c_str(), Info[info.nLogLevel].length());
		fLogFile.write(info.sLogInfo.c_str(), info.sLogInfo.length());
		fLogFile.write(info.sCodeFile.c_str(), info.sCodeFile.length());
		fLogFile.write(":",1);
		char number[10];
		sprintf(number, "%d", info.nFileLine);
		fLogFile.write(number, strlen(number));
		fLogFile.write("\n",1);
	}
}

void CLog::Stop(void)
{
	bRun = false;
}
