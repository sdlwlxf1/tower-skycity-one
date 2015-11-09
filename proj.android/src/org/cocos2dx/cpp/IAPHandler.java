package org.cocos2dx.cpp;

import com.lxf.game.tower.R;

import IAP.IAPJni;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.os.Handler;
import android.os.Message;
import android.widget.Toast;

public class IAPHandler extends Handler {

	public static final int INIT_FINISH = 10000;
	public static final int BILL_FINISH = 10001;
	public static final int QUERY_FINISH = 10002;
	public static final int UNSUB_FINISH = 10003;
	public static final int SHOW_DIALOG = 10004;

	private AppActivity context;

	public IAPHandler(Activity context) {
		this.context = (AppActivity) context;
	}

	@Override
	public void handleMessage(Message msg) {
		super.handleMessage(msg);
		int what = msg.what;
		switch (what) {
		case INIT_FINISH:
			initShow((String) msg.obj);
			//context.dismissProgressDialog();
			break;
		case SHOW_DIALOG:
	        DialogMessage dm = (DialogMessage)msg.obj;
	        new AlertDialog.Builder(context)
	        .setTitle(dm.title)
	        .setMessage(dm.msg).setNegativeButton("cancle", new DialogInterface.OnClickListener() {
	        
	        @Override
	        public void onClick(DialogInterface dialog, int which) {
	                dialog.dismiss();
	            }
	        })
	        .setPositiveButton("Ok",
	                       new DialogInterface.OnClickListener() {
	        
	            @Override
	            public void onClick(DialogInterface dialog, int which) {
	                dialog.dismiss();
	                IAPJni.exitApp();
	            }
	        })
	        .create().show();
	        break;
		default:
			break;
		}
	}

	private void initShow(String msg) {
		Toast.makeText(context, "初始化：" + msg, Toast.LENGTH_LONG).show();
	}

	public void showDialog(String title, String msg) {
		AlertDialog.Builder builder = new AlertDialog.Builder(context);
		builder.setTitle(title);
		builder.setIcon(context.getResources().getDrawable(R.drawable.icon));
		builder.setMessage((msg == null) ? "Undefined error" : msg);
		builder.setPositiveButton("OK", new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int whichButton) {
				dialog.dismiss();
			}
		});
		builder.create().show();
	}

}
