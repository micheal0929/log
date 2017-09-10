/*
 * log.h
 *
 *  Created on: 2017年9月10日
 *      Author: michael
 */

#ifndef LOG_H_
#define LOG_H_

#include <stdint.h>
#include <string>
#include <fstream>
#include <concurrentqueue.h>

const static uint32_t LEN = 1024;
enum LogLevel{
	INFO = 0,
	DEBUG,
	ERROR,
	FATAL
};
const static std::string Info[5] = {"[INFO] ", "[DEBUG] ", "[ERROR] ", "[FATAL] "};

struct LogInfo{
	std::string sLogInfo;
	uint8_t nLogLevel;
	std::string sCodeFile;
	uint16_t nFileLine;
	LogInfo(const std::string& loginfo={}, uint8_t level=0,const std::string& codefile={}, uint16_t line=0):
		sLogInfo(loginfo)
		,nLogLevel(level)
		,sCodeFile(codefile)
		,nFileLine(line)
	{

	}
};
class CLog{
public:

	CLog(const std::string& slogname);
	~CLog();

public:
	//记录日志信息到无锁队列
	void log(const std::string& sLogInfo, uint8_t loglevel, const std::string& sCodeFile = {}, uint16_t nline = 0);

	//将无锁队列中的信息写入到文件中
	void flush();

	void Stop(void);

private:
	uint16_t nIntervalTime;//间隔写入时间
	uint32_t nCacheLength;//最大写入大小

	std::string sPrefix;//日志文件名前缀
	std::string sSuffix;//日志文件名后缀

	std::string sLogName;//日志文件名称

	std::ofstream fLogFile;//日志文件操作对象

	uint8_t nCurLogLevel;

	bool bRun;

	moodycamel::ConcurrentQueue<LogInfo> mQueueLog;

	std::thread mThread;
private:
	std::string GetDayTime();

	std::string GetFullName();

	void FlushAll(void);

	void WriteFile(const LogInfo& info);




};




#endif /* LOG_H_ */
