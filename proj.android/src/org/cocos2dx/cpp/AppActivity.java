/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2014 Chukong Technologies Inc.
 
http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
package org.cocos2dx.cpp;

import IAP.IAPJni;

import org.cocos2dx.lib.Cocos2dxActivity;
import org.cocos2dx.lib.Cocos2dxGLSurfaceView;

import com.lxf.game.tower.R;
import com.sina.weibo.sdk.constant.WBConstants;
import com.tencent.mm.sdk.openapi.BaseReq;
import com.tencent.mm.sdk.openapi.BaseResp;
import com.tencent.mm.sdk.openapi.ConstantsAPI;
import com.tencent.mm.sdk.openapi.IWXAPI;
import com.tencent.mm.sdk.openapi.IWXAPIEventHandler;
import com.tencent.mm.sdk.openapi.WXAppExtendObject;

import android.content.Intent;
import android.os.Bundle;
import android.widget.Toast;

public class AppActivity extends Cocos2dxActivity{
	
	public Cocos2dxGLSurfaceView onCreateView() {
        Cocos2dxGLSurfaceView glSurfaceView = new Cocos2dxGLSurfaceView(this);
        // TestCpp should create stencil buffer
        glSurfaceView.setEGLConfigChooser(5, 6, 5, 0, 16, 8);
        
        return glSurfaceView;
    }

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		IAPJni jni = new IAPJni();
		jni.init(this);
		IAPJni.instance = jni;
	}
	
	@Override
	public void onActivityResult(int requestCode, int resultCode, Intent intent)
	{
		super.onActivityResult(requestCode,resultCode,intent);
		
//		switch (resultCode) {
//        case 1:
//            Toast.makeText(this, R.string.weibosdk_demo_toast_share_success, Toast.LENGTH_LONG).show();
//            IAPJni.shareSuccess(4);
//            break;
//        case 2:
//            Toast.makeText(this, R.string.weibosdk_demo_toast_share_canceled, Toast.LENGTH_LONG).show();
//            IAPJni.shareFailed(4);
//            break;
//        case 3:
//            Toast.makeText(this, 
//                    getString(R.string.weibosdk_demo_toast_share_failed) + "Error Message: ", 
//                    Toast.LENGTH_LONG).show();
//            IAPJni.shareFailed(4);
//            break;
//        }
		
		// SSO 授权回调
        // 重要：发起 SSO 登陆的 Activity 必须重写 onActivityResult
        if (IAPJni.mSsoHandler != null) {
        	IAPJni.mSsoHandler.authorizeCallBack(requestCode, resultCode, intent);
        }
	}
}
