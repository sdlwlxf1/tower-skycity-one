package org.cocos2dx.cpp;

import java.util.HashMap;

import mm.purchasesdk.OnPurchaseListener;
import mm.purchasesdk.Purchase;
import mm.purchasesdk.core.PurchaseCode;
import IAP.IAPJni;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.os.Message;
import android.util.Log;

public class IAPListener implements OnPurchaseListener {
	private final String TAG = "IAPListener";
	private AppActivity context;
	private IAPHandler iapHandler;

	public IAPListener(Context context, IAPHandler iapHandler) {
		this.context = (AppActivity) context;
		this.iapHandler = iapHandler;
	}

	@Override
	public void onAfterApply() {

	}

	@Override
	public void onAfterDownload() {

	}

	@Override
	public void onBeforeApply() {

	}

	@Override
	public void onBeforeDownload() {

	}

	@Override
	public void onInitFinish(int code) {
		Log.d(TAG, "Init finish, status code = " + code);
//		Message message = iapHandler.obtainMessage(IAPHandler.INIT_FINISH);
//		String result = "支付初始化成功" + Purchase.getReason(code);
//		message.obj = result;
//		message.sendToTarget();
	}

	@Override
    public void onBillingFinish(int code, HashMap arg1) {
        Log.d(TAG, "billing finish, status code = " + code);
        String result = "";
        Message message = iapHandler.obtainMessage(IAPHandler.BILL_FINISH);
        // 此次订购的orderID
        String orderID = null;
        // 商品的paycode
        String paycode = null;
        // 商品的有效期(仅租赁类型商品有�?
        String leftday = null;
        // 商品的交�?ID，用户可以根据这个交易ID，查询商品是否已经交�?
        String tradeID = null;
        
        String ordertype = null;
        if (code == PurchaseCode.ORDER_OK || (code == PurchaseCode.AUTH_OK) || (code == PurchaseCode.WEAK_ORDER_OK)) {
            /**
             * 商品购买成功或�?已经购买�?此时会返回商品的paycode，orderID,以及剩余时间(租赁类型商品)
             */
            if (arg1 != null) {
                leftday = (String) arg1.get(OnPurchaseListener.LEFTDAY);
                if (leftday != null && leftday.trim().length() != 0) {
                    result = result + ",剩余时间 �?" + leftday;
                }
                orderID = (String) arg1.get(OnPurchaseListener.ORDERID);
                if (orderID != null && orderID.trim().length() != 0) {
                    result = result + ",OrderID �?" + orderID;
                }
                paycode = (String) arg1.get(OnPurchaseListener.PAYCODE);
                if (paycode != null && paycode.trim().length() != 0) {
                    result = result + ",Paycode:" + paycode;
                }
                tradeID = (String) arg1.get(OnPurchaseListener.TRADEID);
                if (tradeID != null && tradeID.trim().length() != 0) {
                    result = result + ",tradeID:" + tradeID;
                }
                ordertype = (String) arg1.get(OnPurchaseListener.ORDERTYPE);
                if (tradeID != null && tradeID.trim().length() != 0) {
                    result = result + ",ORDERTYPE:" + ordertype;
                }
//_________修改___________________________ 
                
            	IAPJni.orderSuccess(IAPJni.payCodes.indexOf(paycode));//返回结果调用
            	result = "订购结果：订购成功";
            }
        } else {
            /**
             * 表示订购失败�?
             */
            result = Purchase.getReason(code);
            IAPJni.orderFailed(result);
        }
		message.obj = result;
		message.sendToTarget();
        System.out.println(result);
        
        new AlertDialog.Builder(context)
        .setTitle("订购结果")
        .setMessage(result).setNegativeButton("确定", new DialogInterface.OnClickListener() {
        
        @Override
        public void onClick(DialogInterface dialog, int which) {
            }
        })
        .create().show();
    }

	@Override
	public void onQueryFinish(int code, HashMap arg1) {
		Log.d(TAG, "license finish, status code = " + code);
		Message message = iapHandler.obtainMessage(IAPHandler.QUERY_FINISH);
		String result = "鏌ヨ鎴愬姛,璇ュ晢鍝佸凡璐拱";
		// 姝ゆ璁㈣喘鐨刼rderID
		String orderID = null;
		// 鍟嗗搧鐨刾aycode
		String paycode = null;
		// 鍟嗗搧鐨勬湁鏁堟湡(浠呯璧佺被鍨嬪晢鍝佹湁鏁�)
		String leftday = null;
		if (code != PurchaseCode.QUERY_OK) {
			/**
			 * 鏌ヨ涓嶅埌鍟嗗搧璐拱鐨勭浉鍏充俊鎭�
			 */
			result = "鏌ヨ缁撴灉锛�" + Purchase.getReason(code);
		} else {
			/**
			 * 鏌ヨ鍒板晢鍝佺殑鐩稿叧淇℃伅銆�
			 * 姝ゆ椂浣犲彲浠ヨ幏寰楀晢鍝佺殑paycode锛宱rderid锛屼互鍙婂晢鍝佺殑鏈夋晥鏈焞eftday锛堜粎绉熻祦绫诲瀷鍟嗗搧鍙互杩斿洖锛�
			 */
			leftday = (String) arg1.get(OnPurchaseListener.LEFTDAY);
			if (leftday != null && leftday.trim().length() != 0) {
				result = result + ",鍓╀綑鏃堕棿 锛� " + leftday;
			}
			orderID = (String) arg1.get(OnPurchaseListener.ORDERID);
			if (orderID != null && orderID.trim().length() != 0) {
				result = result + ",OrderID 锛� " + orderID;
			}
			paycode = (String) arg1.get(OnPurchaseListener.PAYCODE);
			if (paycode != null && paycode.trim().length() != 0) {
				result = result + ",Paycode:" + paycode;
			}
		}
		System.out.println(result);
		//context.dismissProgressDialog();
	}

	

	@Override
	public void onUnsubscribeFinish(int code) {
		// TODO Auto-generated method stub
		String result = "閫�璁㈢粨鏋滐細" + Purchase.getReason(code);
		System.out.println(result);
		//context.dismissProgressDialog();
	}
}
