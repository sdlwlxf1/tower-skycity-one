#include "cocos2d.h"
#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#include "test.h"
#include "JniTest.h"

#define CLASS_NAME "org/cocos2dx/cpp/AppActivity"

using namespace cocos2d;

extern "C"
{
void showTipDialog(const char *title, const char *msg)
{
	JniMethodInfo t;
	if(JniHelper::getStaticMethodInfo(t, CLASS_NAME, "showTipDialog", "(Ljava/lang/String;Ljava/lang/String;)V"))
	{
		jstring jTitle = t.env->NewStringUTF(title);
		jstring jMsg = t.env->NewStringUTF(msg);
		t.env->CallStaticVoidMethod(t.classID, t.methodID, jTitle, jMsg);
		t.env->DeleteLocalRef(jTitle);
		t.env->DeleteLocalRef(jMsg);
	}
}

void Java_IAP_IAPJni_setPackageName(JNIEnv *env, jobject thiz, jstring packageName)
{
	const char *pkgName = env->GetStringUTFChars(packageName, NULL);
	JniTest::getInstance()->setPackageName(pkgName);
	env->ReleaseStringUTFChars(packageName, pkgName);
}

void Java_IAP_IAPJni_shareSuccess(JNIEnv *env, jobject thiz, jint shareId)
{
	JniTest::getInstance()->shareSuccess(shareId);
}

void Java_IAP_IAPJni_shareFailed(JNIEnv *env, jobject thiz, jint shareId)
{
	JniTest::getInstance()->shareFailed(shareId);
}

void Java_IAP_IAPJni_exitApp(JNIEnv *env, jobject thiz)
{
	JniTest::getInstance()->exitApp();
}

void Java_IAP_IAPJni_orderSuccess(JNIEnv *env, jobject thiz, jint goodId)
{
	JniTest::getInstance()->orderSuccess(goodId);
}

void Java_IAP_IAPJni_orderFailed(JNIEnv *env, jobject thiz, jstring info)
{
	const char *infoC = env->GetStringUTFChars(info, NULL);
	JniTest::getInstance()->orderFailed(infoC);
}

}
