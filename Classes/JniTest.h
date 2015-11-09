#ifndef __JNI_TEST_H__
#define __JNI_TEST_H__

#include "cocos2d.h"
using namespace cocos2d;

class JniTest :public Ref
{
public:
	static JniTest* getInstance();

	void setPackageName(const char *packageName);
	void exitApp();
	void shareSuccess(int index);
	void shareFailed(int index);
	void orderSuccess(int goodId);
	void orderFailed(const char* info);
};

#endif //__JNI_TEST_H__
