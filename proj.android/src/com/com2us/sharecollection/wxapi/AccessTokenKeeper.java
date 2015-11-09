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

import com.sina.weibo.sdk.auth.Oauth2AccessToken;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;

//import com.sina.weibo.sdk.auth.Oauth2AccessToken;

/**
 * 璇ョ被瀹氫箟浜嗗井鍗氭巿鏉冩椂鎵�闇�瑕佺殑鍙傛暟銆�
 * 
 * @author SINA
 * @since 2013-10-07
 */
public class AccessTokenKeeper {
    private static final String PREFERENCES_NAME = "com_weibo_sdk_android";

    private static final String KEY_UID           = "uid";
    private static final String KEY_ACCESS_TOKEN  = "access_token";
    private static final String KEY_EXPIRES_IN    = "expires_in";
    
    /**
     * 淇濆瓨 Token 瀵硅薄鍒� SharedPreferences銆�
     * 
     * @param context 搴旂敤绋嬪簭涓婁笅鏂囩幆澧�
     * @param token   Token 瀵硅薄
     */
    public static void writeAccessToken(Context context, Oauth2AccessToken token) {
        if (null == context || null == token) {
            return;
        }
        
        SharedPreferences pref = context.getSharedPreferences(PREFERENCES_NAME, Context.MODE_APPEND);
        Editor editor = pref.edit();
        editor.putString(KEY_UID, token.getUid());
        editor.putString(KEY_ACCESS_TOKEN, token.getToken());
        editor.putLong(KEY_EXPIRES_IN, token.getExpiresTime());
        editor.commit();
    }

    /**
     * 浠� SharedPreferences 璇诲彇 Token 淇℃伅銆�
     * 
     * @param context 搴旂敤绋嬪簭涓婁笅鏂囩幆澧�
     * 
     * @return 杩斿洖 Token 瀵硅薄
     */
    public static Oauth2AccessToken readAccessToken(Context context) {
        if (null == context) {
            return null;
        }
        
        Oauth2AccessToken token = new Oauth2AccessToken();
        SharedPreferences pref = context.getSharedPreferences(PREFERENCES_NAME, Context.MODE_APPEND);
        token.setUid(pref.getString(KEY_UID, ""));
        token.setToken(pref.getString(KEY_ACCESS_TOKEN, ""));
        token.setExpiresTime(pref.getLong(KEY_EXPIRES_IN, 0));
        return token;
    }

    /**
     * 娓呯┖ SharedPreferences 涓� Token淇℃伅銆�
     * 
     * @param context 搴旂敤绋嬪簭涓婁笅鏂囩幆澧�
     */
    public static void clear(Context context) {
        if (null == context) {
            return;
        }
        
        SharedPreferences pref = context.getSharedPreferences(PREFERENCES_NAME, Context.MODE_APPEND);
        Editor editor = pref.edit();
        editor.clear();
        editor.commit();
    }
}
