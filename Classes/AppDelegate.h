#ifndef  _APP_DELEGATE_H_
#define  _APP_DELEGATE_H_

#include "Global.h"
#include "cocos2d.h"

/**
@brief    The cocos2d Application.

The reason for implement as private inheritance is to hide some interface call by Director.
*/
class  AppDelegate : private cocos2d::Application
{
public:
    AppDelegate();
    virtual ~AppDelegate();

    /**
    @brief    Implement Director and Scene init code here.
    @return true    Initialize success, app continue.
    @return false   Initialize failed, app terminate.
    */
    virtual bool applicationDidFinishLaunching();

    /**
    @brief  The function be called when the application enter background
    @param  the Vec2er of the application
    */
    virtual void applicationDidEnterBackground();

    /**
    @brief  The function be called when the application enter foreground
    @param  the Vec2er of the application
    */
    virtual void applicationWillEnterForeground();

	virtual void preHandle();
};

#endif // _APP_DELEGATE_H_

