package IAP;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Arrays;

import org.cocos2dx.cpp.DialogMessage;
import org.cocos2dx.cpp.AppActivity;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.text.TextUtils;
import android.view.Menu;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.Toast;

import org.cocos2dx.cpp.IAPHandler;
import org.cocos2dx.cpp.IAPListener;
import org.cocos2dx.cpp.AppActivity;


import com.com2us.sharecollection.wxapi.AccessTokenKeeper;
import com.com2us.sharecollection.wxapi.BaseUIListener;
import com.com2us.sharecollection.wxapi.Constants_Qzone;
import com.com2us.sharecollection.wxapi.Constants_sina;
import com.com2us.sharecollection.wxapi.Constants_tencent;
import com.com2us.sharecollection.wxapi.Constants_wechat;
//import com.com2us.sharecollection.wxapi.Util;
import com.com2us.sharecollection.wxapi.WBShareActivity;
import com.com2us.sharecollection.wxapi.map;
import com.lxf.game.tower.R;
import com.lxf.game.tower.wxapi.WXEntryActivity;
import com.sina.weibo.sdk.api.ImageObject;
import com.sina.weibo.sdk.api.TextObject;
import com.sina.weibo.sdk.api.WeiboMessage;
import com.sina.weibo.sdk.api.WeiboMultiMessage;
import com.sina.weibo.sdk.api.share.IWeiboShareAPI;
import com.sina.weibo.sdk.api.share.SendMessageToWeiboRequest;
import com.sina.weibo.sdk.api.share.SendMultiMessageToWeiboRequest;
import com.sina.weibo.sdk.api.share.WeiboShareSDK;
import com.sina.weibo.sdk.auth.AuthInfo;
import com.sina.weibo.sdk.auth.Oauth2AccessToken;
import com.sina.weibo.sdk.auth.WeiboAuthListener;
import com.sina.weibo.sdk.auth.sso.SsoHandler;
import com.sina.weibo.sdk.exception.WeiboException;
import com.tencent.connect.share.QzoneShare;
import com.tencent.mm.sdk.openapi.IWXAPI;
import com.tencent.mm.sdk.openapi.SendMessageToWX;
import com.tencent.mm.sdk.openapi.WXAPIFactory;
import com.tencent.mm.sdk.openapi.WXMediaMessage;
import com.tencent.mm.sdk.openapi.WXTextObject;
import com.tencent.tauth.Tencent;
import com.tencent.weibo.sdk.android.api.util.Util;
import com.tencent.weibo.sdk.android.component.Authorize;
import com.tencent.weibo.sdk.android.component.ReAddActivity;
import com.tencent.weibo.sdk.android.component.sso.AuthHelper;
import com.tencent.weibo.sdk.android.component.sso.OnAuthListener;
import com.tencent.weibo.sdk.android.component.sso.WeiboToken;

import android.app.ProgressDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.drawable.BitmapDrawable;
import mm.purchasesdk.OnPurchaseListener;
import mm.purchasesdk.Purchase;

public class IAPJni
{
	static {
	    System.loadLibrary("identifyapp");
	    System.loadLibrary("casdkjni");
	}
	
	/** Called when the activity is first created. */
	public static ArrayList<String> payCodes = new ArrayList<String>(Arrays.asList("30000879204805","30000879204804","30000879204801","30000879204802","30000879204803"));
	
	private static final int PRODUCT_NUM = 5;
	private int mProductNum = PRODUCT_NUM;
	
	private static final String APPID = "300008792048";
	private static final String APPKEY = "CC806B36AB6DBB0001E0C1C4B3392E04";
	// 鐠伮ゅ瀭閻愰�涗繆閹拷
	private static final String LEASE_PAYCODE = "娴ｇ姷娈憄aycode";
    
	private static String mPaycode;
	
    public static Purchase purchase;
    private static IAPListener mListener;
    private static Context mcontext;
    private static AppActivity mactivity;
    private static IAPHandler mHandler;
    
    public static IAPJni instance;
    
    private ProgressDialog mProgressDialog;
    
    public static IWXAPI wxApi;
//	private IWeiboShareAPI  mWeiboShareAPI = null;
//	private SsoHandler mSsoHandler;
//	private WeiboAuth mWeiboAuth;
//	private Oauth2AccessToken mAccessToken;
	private static Tencent mTencent;
	
	private static AuthInfo mAuthInfo;
	
	/** 封装了 "access_token"，"expires_in"，"refresh_token"，并提供了他们的管理功能  */
    private static Oauth2AccessToken mAccessToken;
	 /** 注意：SsoHandler 仅当 SDK 支持 SSO 时有效 */
    public static SsoHandler mSsoHandler;
    
    public static final String KEY_SHARE_TYPE = "key_share_type";
    public static final int SHARE_CLIENT = 1;
    public static final int SHARE_ALL_IN_ONE = 2;
    
    /** 微博微博分享接口实例 */
    private IWeiboShareAPI  mWeiboShareAPI = null;
    
    private int mShareType = SHARE_CLIENT;
	
	public void init(AppActivity activity)
	{
//		mProgressDialog = new ProgressDialog(activity);
//		mProgressDialog.setIndeterminate(true);
//		mProgressDialog.setMessage("");
		
		
		mactivity = activity;
		
		/**
		 * 支付初始化
		 */
		
		mHandler = new IAPHandler(activity);
		/**
		 * IAP缂備礁瀚▎銏ゅ礆濠靛棭娼楅柛鏍垫嫹.闁告牕鎳忕�氼厽绋夌�ｎ喗妗�3婵縿鍎敓鏂ゆ嫹
		 */
		/**
		 * step1.閻庡湱鍋樼欢銉╁礌閺堢垜rchaseListener闁靛棗鍊搁悿鍕瑹鐎ｎ亜顕уù鑲╁Т閸欏棝鎯冮崟顐㈡闁轰線顣︾粭宀勫箖閵娿儳鏉介柣婊呮珒urchaseListener闁规亽鍎辫ぐ娑㈡儍閸曨偒鍤犻悹鐐╁墲濠�渚�宕楅悙鏂ゆ嫹閿燂拷
		 * 濞撴艾顑呴々褔鏁嶇仦缁㈠妰Demo濞寸媴绲块悥婊勭▔椤撴繂鈻忛柣鈶╁晱APListener缂備綀鍕棡PurchaseListener闁挎稑鑻崣楣冨几閸曨垽鎷烽悩鎻掓瘣闁轰椒鍗冲〒鍓佹啺娑撳攷ntext閻庡湱鍋樼欢銉╁Υ閿燂拷
		 */
		mListener = new IAPListener(activity, mHandler);
		/**
		 * step2.闁兼儳鍢茶ぐ鍢漸rchase閻庡湱鍋樼欢銉╁Υ閿燂拷
		 */
		purchase = Purchase.getInstance();
		/**
		 * step3.闁告碍鍘紆rhase濞磋偐濮撮崣鍡樻償閺冨倹鏆忓ǎ鍥ｅ墲娴煎懘濡存笟鐜働ID闁挎稑鐡慞PKEY闁靛棴鎷� 闂傚浄鎷烽悷鏇氭缁卞爼宕楅妷銉ユ闁轰胶妾癙PID闁挎稑鐡慞PKEY闁靛棴鎷� APPID闁挎稑鐭侀～鍡楊嚕閿熶粙宕ｉ幋锝忔嫹閸涱喗鐎俊妤嬫嫹
		 * APPKEY闁挎稑鐭侀～鍡楊嚕閿熶粙宕ｉ幋锝忔嫹閸涱喗鐎俊妤嬫嫹
		 */
		
		//IAPJni.setPackageName(this.getPackageName());
		
		try {
			purchase.setAppInfo(APPID, APPKEY);

		} catch (Exception e1) {
			e1.printStackTrace();
		}
		/**
		 * step4. IAP缂備礁瀚▎銏ゅ礆濠靛棭娼楅柛鏍ㄧ墪缁辨垶鎱ㄧ�ｅ墎绀� 闁告瑥鍊归弳鐑秛rchaseListener闁挎稑鑻崹鍨叏鐎ｎ亜顕ч柛鎴ｅГ閺嗙喖妫侀敓鑺ュ閻樻彃寮硈tep1闁哄啳娉涢悿鍕瑹鐎ｎ亜顕ч柣顭掓嫹
		 * PurchaseListener闁靛棴鎷�
		 */
		try {
			purchase.init(activity, mListener);

		} catch (Exception e) {
			e.printStackTrace();
			return;
		}
		
		
		/**
		 * 分享初始化
		 */
		// 通过WXAPIFactory工厂，获取IWXAPI的实例
		wxApi = WXAPIFactory.createWXAPI(mactivity, Constants_wechat.APP_ID, false);
		wxApi.registerApp(Constants_wechat.APP_ID);
		
		//sina weibo initialize
		
		
		//Qzone initialize
		mTencent = Tencent.createInstance(Constants_Qzone.APP_ID, mactivity);
	}
    
	
	/**
     * 微博认证授权回调类。
     * 1. SSO 授权时，需要在 {@link #onActivityResult} 中调用 {@link SsoHandler#authorizeCallBack} 后，
     *    该回调才会被执行。
     * 2. 非 SSO 授权时，当授权结束后，该回调就会被执行。
     * 当授权成功后，请保存该 access_token、expires_in、uid 等信息到 SharedPreferences 中。
     */
    static class AuthListener implements WeiboAuthListener {
        
        @Override
        public void onComplete(Bundle values) {
            // 从 Bundle 中解析 Token
            mAccessToken = Oauth2AccessToken.parseAccessToken(values);
            if (mAccessToken.isSessionValid()) {
                // 显示 Token
                updateTokenView(false);
                
                // 保存 Token 到 SharedPreferences
                AccessTokenKeeper.writeAccessToken(mactivity, mAccessToken);
                Toast.makeText(mactivity, 
                        R.string.weibosdk_demo_toast_auth_success, Toast.LENGTH_SHORT).show();
            } else {
                // 以下几种情况，您会收到 Code：
                // 1. 当您未在平台上注册的应用程序的包名与签名时；
                // 2. 当您注册的应用程序包名与签名不正确时；
                // 3. 当您在平台上注册的包名和签名与您当前测试的应用的包名和签名不匹配时。
                String code = values.getString("code");
                String message = mactivity.getString(R.string.weibosdk_demo_toast_auth_failed);
                if (!TextUtils.isEmpty(code)) {
                    message = message + "\nObtained the code: " + code;
                }
                Toast.makeText(mactivity, message, Toast.LENGTH_LONG).show();
            }
        }

        @Override
        public void onCancel() {
//            Toast.makeText(mactivity, 
//                    R.string.weibosdk_demo_toast_auth_canceled, Toast.LENGTH_LONG).show();
        }

        @Override
        public void onWeiboException(WeiboException e) {
            Toast.makeText(mactivity, 
                    "Auth exception : " + e.getMessage(), Toast.LENGTH_LONG).show();
        }
    }
    
    /**
     * 显示当前 Token 信息。
     * 
     * @param hasExisted 配置文件中是否已存在 token 信息并且合法
     */
    private static void updateTokenView(boolean hasExisted) {
//        String date = new SimpleDateFormat("yyyy/MM/dd HH:mm:ss").format(
//                new java.util.Date(mAccessToken.getExpiresTime()));
//        String format = getString(R.string.weibosdk_demo_token_to_string_format_1);
//        mTokenText.setText(String.format(format, mAccessToken.getToken(), date));
//        
//        String message = String.format(format, mAccessToken.getToken(), date);
//        if (hasExisted) {
//            message = getString(R.string.weibosdk_demo_token_has_existed) + "\n" + message;
//        }
//        mTokenText.setText(message);
    }
    /*
	 * 授权
	 */
    public static void authOrder(int index)
    {
    	switch(index)
        {
        // 微信
        case 1:
        	// 将该app注册到微信
		    wxApi.registerApp(Constants_wechat.APP_ID);
        	break;
    	// 腾讯微博
		case 2: 			
			long app_key = Long.valueOf(Constants_tencent.APP_KEY);
			String app_secret = Constants_tencent.APP_SECRET;
			auth(app_key, app_secret);
        	break;
    	// 新浪微博
		case 3:			
			// 快速授权时，请不要传入 SCOPE，否则可能会授权不成功
	        mAuthInfo = new AuthInfo(mactivity, Constants_sina.APP_KEY, Constants_sina.REDIRECT_URL, Constants_sina.SCOPE);//
	        mSsoHandler = new SsoHandler(mactivity, mAuthInfo);
	        mSsoHandler.authorize(new AuthListener());
			break;
        }
    }
    
    public static Object getJavaActivity()
    {
    	return instance;
    }
    
    private static String buildTransaction(final String type) {
		return (type == null) ? String.valueOf(System.currentTimeMillis()) : type + System.currentTimeMillis();
	}
    
	/*
	 * 分享方式
	 */
    public static void shareOrder(int index, String a)
    {
        switch(index)
        {
        // 微信
        case 1:
        	int wxSdkVersion = wxApi.getWXAppSupportAPI();
    		if (wxSdkVersion >= 0x21020001) {
    	        // 初始化一个WXTextObject对象
    			WXTextObject textObj = new WXTextObject();
    			textObj.text = a + "http://dwz.cn/zS2fe";
    	
    			// 用WXTextObject对象初始化一个WXMediaMessage对象
    			WXMediaMessage msg = new WXMediaMessage();
    			msg.mediaObject = textObj;
    			// 发送文本类型的消息时，title字段不起作用
    			// msg.title = "Will be ignored";
    			msg.description = a + "http://dwz.cn/zS2fe";
    	
    			// 构造一个Req
    			SendMessageToWX.Req req = new SendMessageToWX.Req();
    			req.transaction = buildTransaction("text"); // transaction字段用于唯一标识一个请求
    			req.message = msg;
    			req.scene = true ? SendMessageToWX.Req.WXSceneTimeline : SendMessageToWX.Req.WXSceneSession;
    			
    			// 调用api接口发送数据到微信
    			wxApi.sendReq(req);
    		} else {
    			Toast.makeText(mactivity, "Please install the latest version of wechat", Toast.LENGTH_LONG).show();
    		}
        	break;
//    	// qq空间
//        case 2:
//        	final Bundle params = new Bundle();
//        	
//        	params.putInt(QzoneShare.SHARE_TO_QZONE_KEY_TYPE, QzoneShare.SHARE_TO_QZONE_TYPE_IMAGE_TEXT);
//            params.putString(QzoneShare.SHARE_TO_QQ_TITLE, "title");
//            params.putString(QzoneShare.SHARE_TO_QQ_IMAGE_URL, "@drawable/icon80");
//            params.putString(QzoneShare.SHARE_TO_QQ_SUMMARY, "快来玩魔塔天城吧");
//            params.putString(QzoneShare.SHARE_TO_QQ_TARGET_URL, "http://www.withhive.com");
//            ArrayList<String> imageUrls = new ArrayList<String>();
//            params.putStringArrayList(QzoneShare.SHARE_TO_QQ_IMAGE_URL, imageUrls);
//          
//            mTencent.shareToQzone(mactivity, params, new BaseUIListener(mactivity));
//        	break;
    	// 腾讯微博
		case 2: 			
  			Intent i = new Intent(mactivity,ReAddActivity.class);
			Bundle bundle = new Bundle();
			bundle.putString("content", a + "http://dwz.cn/zRDFD");
			//bundle.putString("video_url", "http://www.tudou.com/programs/view/b-4VQLxwoX4/");
			bundle.putString("pic_url", "@drawable/icon80");
			i.putExtras(bundle);
			mactivity.startActivity(i);
        	break;
    	// 新浪微博
		case 3:			
			Intent j = new Intent(mactivity, WBShareActivity.class);
            j.putExtra(WBShareActivity.KEY_SHARE_TYPE, WBShareActivity.SHARE_ALL_IN_ONE);
            j.putExtra("shareText", a + "http://t.cn/RZx3AOz");
            //mactivity.startActivity(j);
            mactivity.startActivityForResult(j, -1);
			break;
        }
    }

  	private static void auth(long appid, String app_secket) {
  		final Context context = mactivity.getApplicationContext();
  		//注册当前应用的appid和appkeysec，并指定一个OnAuthListener
		//OnAuthListener在授权过程中实施监听
  		AuthHelper.register(context, appid, app_secket, new OnAuthListener() {
  		//如果当前设备没有安装腾讯微博客户端，走这里
  			@Override
  			public void onWeiBoNotInstalled() {
  				//Toast.makeText(mactivity, "onWeiBoNotInstalled", 1000).show();
  				AuthHelper.unregister(context);
  				Intent i = new Intent(context,Authorize.class);
  				mactivity.startActivity(i);
  			}
  			//如果当前设备没安装指定版本的微博客户端，走这里
  			@Override
  			public void onWeiboVersionMisMatch() {
  				//Toast.makeText(context, "onWeiboVersionMisMatch",1000).show();
  				AuthHelper.unregister(context);
  				Intent i = new Intent(context,Authorize.class);
  				mactivity.startActivity(i);
  			}
  			//如果授权失败，走这里
  			@Override
  			public void onAuthFail(int result, String err) {
  				//Toast.makeText(context, "result : " + result, 1000).show();
  				AuthHelper.unregister(context);
  			}
  			//授权成功，走这里
			//授权成功后，所有的授权信息是存放在WeiboToken对象里面的，可以根据具体的使用场景，将授权信息存放到自己期望的位置，
			//在这里，存放到了applicationcontext中
  			@Override
  			public void onAuthPassed(String name, WeiboToken token) {
  				//Toast.makeText(context, "passed", 1000).show();
  				//
  				Util.saveSharePersistent(context, "ACCESS_TOKEN", token.accessToken);
  				Util.saveSharePersistent(context, "EXPIRES_IN", String.valueOf(token.expiresIn));
  				Util.saveSharePersistent(context, "OPEN_ID", token.openID);
//  				Util.saveSharePersistent(context, "OPEN_KEY", token.omasKey);
  				Util.saveSharePersistent(context, "REFRESH_TOKEN", "");
//  				Util.saveSharePersistent(context, "NAME", name);
//  				Util.saveSharePersistent(context, "NICK", name);
  				Util.saveSharePersistent(context, "CLIENT_ID", Util.getConfig().getProperty("APP_KEY"));
  				Util.saveSharePersistent(context, "AUTHORIZETIME",
  						String.valueOf(System.currentTimeMillis() / 1000l));
  				AuthHelper.unregister(context);
  			}
  		});

  		AuthHelper.auth(mactivity, "");
  	}
	
    public static void orderGood(int itemId)
    {
        mPaycode = payCodes.get(itemId);
        purchase.order(mactivity,mPaycode,1,false,mListener);//MMSDK
    }
    
    private static void showTipDialog(final String title, final String text)
    {
    	Message msg = mHandler.obtainMessage();
    	msg.what = IAPHandler.SHOW_DIALOG;
    	DialogMessage dm = new DialogMessage();
    	dm.title = title;
    	dm.msg = text;
    	msg.obj = dm;
    	msg.sendToTarget();
    }
    
    private void showProgressDialog() {
		if (mProgressDialog == null) {
			mProgressDialog = new ProgressDialog(mactivity);
			mProgressDialog.setIndeterminate(true);
			mProgressDialog.setMessage("閻犲洭顥撻埣銏ゅ磹閿燂拷.....");
		}
		if (!mProgressDialog.isShowing()) {
			mProgressDialog.show();
		}
	}
    
    public static native void setPackageName(String packageName);
    public static native void exitApp();
    public native static void orderSuccess(int goodId);//璋冪敤C++
    public native static void orderFailed(String info);//璋冪敤C++
    public native static void shareSuccess(int index);//璋冪敤C++
    public native static void shareFailed(int index);//璋冪敤C++
}