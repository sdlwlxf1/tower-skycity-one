#ifndef __TOOLS_H__
#define __TOOLS_H__

#include "cocos2d.h"
USING_NS_CC;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
static inline long getTime()
{
	struct timeval now;
	gettimeofday(&now, NULL);
	return now.tv_sec;
}
#endif

#if ( CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 )
static inline time_t getTime()
{
	time_t timep;
	time(&timep);
	return timep;
}
#endif


static inline std::string getTimeStr()
{
	struct tm* time;
	auto timep = getTime();
	time = localtime(&timep);
	char date[32] = {0};
	sprintf(date, "%02d-%02d %02d:%02d",(int)time->tm_mon + 1,(int)time->tm_mday,(int)time->tm_hour,(int)time->tm_min);
	return StringUtils::format("%s",date);
}

#endif //__TOOLS_H__