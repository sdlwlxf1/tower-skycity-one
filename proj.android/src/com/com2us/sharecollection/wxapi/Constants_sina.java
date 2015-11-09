package com.com2us.sharecollection.wxapi;

public interface Constants_sina {
	//debug
    //public static final String APP_KEY      = "1246688997";
    //release
    public static final String APP_KEY      = "1246688997";
    public static final String REDIRECT_URL = "https://api.weibo.com/oauth2/default.html";
    public static final String SCOPE = 
    		"email,direct_messages_read,direct_messages_write,"
    	            + "friendships_groups_read,friendships_groups_write,statuses_to_me_read,"
    	            + "follow_app_official_microblog," + "invitation_write";
}
