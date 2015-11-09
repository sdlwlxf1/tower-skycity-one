/*
 * Copyright (C) 2010-2013 The SINA WEIBO Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.com2us.sharecollection.wxapi;

import org.cocos2dx.cpp.AppActivity;

import IAP.IAPJni;
import android.app.Activity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.drawable.BitmapDrawable;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import com.lxf.game.tower.R;
import com.sina.weibo.sdk.api.ImageObject;
import com.sina.weibo.sdk.api.MusicObject;
import com.sina.weibo.sdk.api.TextObject;
import com.sina.weibo.sdk.api.VideoObject;
import com.sina.weibo.sdk.api.VoiceObject;
import com.sina.weibo.sdk.api.WebpageObject;
import com.sina.weibo.sdk.api.WeiboMessage;
import com.sina.weibo.sdk.api.WeiboMultiMessage;
import com.sina.weibo.sdk.api.share.BaseResponse;
import com.sina.weibo.sdk.api.share.IWeiboHandler;
import com.sina.weibo.sdk.api.share.IWeiboShareAPI;
import com.sina.weibo.sdk.api.share.SendMessageToWeiboRequest;
import com.sina.weibo.sdk.api.share.SendMultiMessageToWeiboRequest;
import com.sina.weibo.sdk.api.share.WeiboShareSDK;
import com.sina.weibo.sdk.auth.AuthInfo;
import com.sina.weibo.sdk.auth.Oauth2AccessToken;
import com.sina.weibo.sdk.auth.WeiboAuthListener;
import com.sina.weibo.sdk.auth.sso.SsoHandler;
import com.sina.weibo.sdk.constant.WBConstants;
import com.sina.weibo.sdk.exception.WeiboException;
import com.sina.weibo.sdk.utils.Utility;

/**
 * 璇ョ被婕旂ず浜嗙涓夋柟搴旂敤濡備綍閫氳繃寰崥瀹㈡埛绔垎浜枃瀛椼�佸浘鐗囥�佽棰戙�侀煶涔愮瓑銆�
 * 鎵ц娴佺▼锛� 浠庢湰搴旂敤->寰崥->鏈簲鐢�
 * 
 * @author SINA
 * @since 2013-10-22
 */
public class WBShareActivity extends Activity implements OnClickListener, IWeiboHandler.Response {
    @SuppressWarnings("unused")
    private static final String TAG = "WBShareActivity";

    public static final String KEY_SHARE_TYPE = "key_share_type";
    public static final int SHARE_CLIENT = 1;
    public static final int SHARE_ALL_IN_ONE = 2;
    
    /** 寰崥寰崥鍒嗕韩鎺ュ彛瀹炰緥 */
    private IWeiboShareAPI  mWeiboShareAPI = null;

    private int mShareType = SHARE_CLIENT;
    
    public String _text;
    
    /**
     * @see {@link Activity#onCreate}
     */
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        //setContentView(R.layout.activity_share);
        //initViews();

        
        
        mShareType = getIntent().getIntExtra(KEY_SHARE_TYPE, SHARE_CLIENT);
        
        _text = getIntent().getStringExtra("shareText");
        
        // 鍒涘缓寰崥鍒嗕韩鎺ュ彛瀹炰緥
        mWeiboShareAPI = WeiboShareSDK.createWeiboAPI(this, Constants_sina.APP_KEY);
        
        // 娉ㄥ唽绗笁鏂瑰簲鐢ㄥ埌寰崥瀹㈡埛绔腑锛屾敞鍐屾垚鍔熷悗璇ュ簲鐢ㄥ皢鏄剧ず鍦ㄥ井鍗氱殑搴旂敤鍒楄〃涓��
        // 浣嗚闄勪欢鏍忛泦鎴愬垎浜潈闄愰渶瑕佸悎浣滅敵璇凤紝璇︽儏璇锋煡鐪� Demo 鎻愮ず
        // NOTE锛氳鍔″繀鎻愬墠娉ㄥ唽锛屽嵆鐣岄潰鍒濆鍖栫殑鏃跺�欐垨鏄簲鐢ㄧ▼搴忓垵濮嬪寲鏃讹紝杩涜娉ㄥ唽
        mWeiboShareAPI.registerApp();
        
		// 褰� Activity 琚噸鏂板垵濮嬪寲鏃讹紙璇� Activity 澶勪簬鍚庡彴鏃讹紝鍙兘浼氱敱浜庡唴瀛樹笉瓒宠鏉�鎺変簡锛夛紝
        // 闇�瑕佽皟鐢� {@link IWeiboShareAPI#handleWeiboResponse} 鏉ユ帴鏀跺井鍗氬鎴风杩斿洖鐨勬暟鎹��
        // 鎵ц鎴愬姛锛岃繑鍥� true锛屽苟璋冪敤 {@link IWeiboHandler.Response#onResponse}锛�
        // 澶辫触杩斿洖 false锛屼笉璋冪敤涓婅堪鍥炶皟
        if (savedInstanceState != null) {
            mWeiboShareAPI.handleWeiboResponse(getIntent(), this);
        }
        
        sendMessage(true, true);
    }
    
    /**
     * @see {@link Activity#onNewIntent}
     */	
    @Override
    protected void onNewIntent(Intent intent) {
        super.onNewIntent(intent);
        
        // 从当前应用唤起微博并进行分享后，返回到当前应用时，需要在此处调用该函数
        // 来接收微博客户端返回的数据；执行成功，返回 true，并调用
        // {@link IWeiboHandler.Response#onResponse}；失败返回 false，不调用上述回调
        mWeiboShareAPI.handleWeiboResponse(intent, this);
    }

    /**
     * 接收微客户端博请求的数据。
     * 当微博客户端唤起当前应用并进行分享时，该方法被调用。
     * 
     * @param baseRequest 微博请求数据对象
     * @see {@link IWeiboShareAPI#handleWeiboRequest}
     */
    @Override
    public void onResponse(BaseResponse baseResp) {
        switch (baseResp.errCode) {
        case WBConstants.ErrorCode.ERR_OK:
            Toast.makeText(this, R.string.weibosdk_demo_toast_share_success, Toast.LENGTH_LONG).show();
//            Intent aintent1 = new Intent(this, AppActivity.class);
//            setResult(1,aintent1);
        	IAPJni.shareSuccess(3);
            break;
        case WBConstants.ErrorCode.ERR_CANCEL:
            Toast.makeText(this, R.string.weibosdk_demo_toast_share_canceled, Toast.LENGTH_LONG).show();
//            Intent aintent2 = new Intent(this, AppActivity.class);
//            setResult(2,aintent2);
        	IAPJni.shareFailed(3);
            break;
        case WBConstants.ErrorCode.ERR_FAIL:
//            Toast.makeText(this, 
//                    getString(R.string.weibosdk_demo_toast_share_failed) + "Error Message: " + baseResp.errMsg + baseResp.errCode, 
//                    Toast.LENGTH_LONG).show();
//            Intent aintent3 = new Intent(this, AppActivity.class);
//            setResult(3,aintent3);
        	IAPJni.shareFailed(3);
        	IAPJni.authOrder(3);
            break;
        }
        finish();
    }
    
    /**
     * 第三方应用发送请求消息到微博，唤起微博分享界面。
     * @see {@link #sendMultiMessage} 或者 {@link #sendSingleMessage}
     */
    private void sendMessage(boolean hasText, boolean hasImage) {
        
        if (mShareType == SHARE_CLIENT) {
            if (mWeiboShareAPI.isWeiboAppSupportAPI()) {
                int supportApi = mWeiboShareAPI.getWeiboAppSupportAPI();
                if (supportApi >= 10351 /*ApiUtils.BUILD_INT_VER_2_2*/) {
                    sendMultiMessage(hasText, hasImage);
                } else {
                    sendSingleMessage(hasText, hasImage);
                }
            } else {
            	// 去授权
            	// 快速授权时，请不要传入 SCOPE，否则可能会授权不成功
            	IAPJni.authOrder(3);
                //Toast.makeText(this, R.string.weibosdk_demo_not_support_api_hint, Toast.LENGTH_SHORT).show();
            }
        }
        else if (mShareType == SHARE_ALL_IN_ONE) {
            sendMultiMessage(hasText, hasImage);
        }
    }

    /**
     * 第三方应用发送请求消息到微博，唤起微博分享界面。
     * 注意：当 {@link IWeiboShareAPI#getWeiboAppSupportAPI()} >= 10351 时，支持同时分享多条消息，
     * 同时可以分享文本、图片以及其它媒体资源（网页、音乐、视频、声音中的一种）。
     * 
     * @param hasText    分享的内容是否有文本
     * @param hasImage   分享的内容是否有图片
     * @param hasWebpage 分享的内容是否有网页
     * @param hasMusic   分享的内容是否有音乐
     * @param hasVideo   分享的内容是否有视频
     * @param hasVoice   分享的内容是否有声音
     */
    private void sendMultiMessage(boolean hasText, boolean hasImage) {
        
        // 1. 初始化微博的分享消息
        WeiboMultiMessage weiboMessage = new WeiboMultiMessage();
        if (hasText) {
            weiboMessage.textObject = getTextObj();
        }
        
        if (hasImage) {
            weiboMessage.imageObject = getImageObj();
        }
        
        // 2. 初始化从第三方到微博的消息请求
        SendMultiMessageToWeiboRequest request = new SendMultiMessageToWeiboRequest();
        // 用transaction唯一标识一个请求
        request.transaction = String.valueOf(System.currentTimeMillis());
        request.multiMessage = weiboMessage;
        
        // 3. 发送请求消息到微博，唤起微博分享界面
        if (mShareType == SHARE_CLIENT) {
            mWeiboShareAPI.sendRequest(this, request);
        }
        else if (mShareType == SHARE_ALL_IN_ONE) {
            AuthInfo authInfo = new AuthInfo(this, Constants_sina.APP_KEY, Constants_sina.REDIRECT_URL, Constants_sina.SCOPE);
            Oauth2AccessToken accessToken = AccessTokenKeeper.readAccessToken(getApplicationContext());
            String token = "";
            if (accessToken != null) {
                token = accessToken.getToken();
            }
            mWeiboShareAPI.sendRequest(this, request, authInfo, token, new WeiboAuthListener() {
                
                @Override
                public void onWeiboException( WeiboException arg0 ) {
                }
                
                @Override
                public void onComplete( Bundle bundle ) {
                    // TODO Auto-generated method stub
                    Oauth2AccessToken newToken = Oauth2AccessToken.parseAccessToken(bundle);
                    AccessTokenKeeper.writeAccessToken(getApplicationContext(), newToken);
                    //Toast.makeText(getApplicationContext(), "onAuthorizeComplete token = " + newToken.getToken(), 0).show();
                }
                
                @Override
                public void onCancel() {
                }
            });
        }
    }

    /**
     * 第三方应用发送请求消息到微博，唤起微博分享界面。
     * 当{@link IWeiboShareAPI#getWeiboAppSupportAPI()} < 10351 时，只支持分享单条消息，即
     * 文本、图片、网页、音乐、视频中的一种，不支持Voice消息。
     * 
     * @param hasText    分享的内容是否有文本
     * @param hasImage   分享的内容是否有图片
     * @param hasWebpage 分享的内容是否有网页
     * @param hasMusic   分享的内容是否有音乐
     * @param hasVideo   分享的内容是否有视频
     */
    private void sendSingleMessage(boolean hasText, boolean hasImage) {
        
        // 1. 初始化微博的分享消息
        // 用户可以分享文本、图片、网页、音乐、视频中的一种
        WeiboMessage weiboMessage = new WeiboMessage();
        if (hasText) {
            weiboMessage.mediaObject = getTextObj();
        }
        if (hasImage) {
            weiboMessage.mediaObject = getImageObj();
        }
        
        // 2. 初始化从第三方到微博的消息请求
        SendMessageToWeiboRequest request = new SendMessageToWeiboRequest();
        // 用transaction唯一标识一个请求
        request.transaction = String.valueOf(System.currentTimeMillis());
        request.message = weiboMessage;
        
        // 3. 发送请求消息到微博，唤起微博分享界面
        mWeiboShareAPI.sendRequest(this, request);
    }
    
    /**
     * 获取分享的文本模板。
     * 
     * @return 分享的文本模板
     */
    private String getSharedText() {
        return _text;
    }
    
    /**
     * 获取分享的文本模板。
     * 
     * @return 分享的文本模板
     */
    public void setSharedText(String text) {
        _text = text;
    }
    
    /**
     * 创建文本消息对象。
     * 
     * @return 文本消息对象。
     */
    private TextObject getTextObj() {
        TextObject textObject = new TextObject();
        textObject.text = getSharedText();
        return textObject;
    }

    /**
     * 创建图片消息对象。
     * 
     * @return 图片消息对象。
     */
    private ImageObject getImageObj() {
        ImageObject imageObject = new ImageObject();
        //BitmapDrawable bitmapDrawable = (BitmapDrawable) mImageView.getDrawable();
        Bitmap bmp = BitmapFactory.decodeResource(getResources(), R.drawable.icon80);
        imageObject.setImageObject(bmp);
        return imageObject;
    }


	@Override
	public void onClick(View arg0) {
		// TODO Auto-generated method stub
		
	}
    
}
