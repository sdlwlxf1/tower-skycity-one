LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

$(call import-add-path,$(LOCAL_PATH)/../../cocos2d)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/external)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/cocos)

LOCAL_MODULE := cocos2dcpp_shared

LOCAL_MODULE_FILENAME := libcocos2dcpp


# 遍历目录及子目录的函数
define walk
    $(wildcard $(1)) $(foreach e, $(wildcard $(1)/*), $(call walk, $(e)))
endef
 
# 遍历Classes目录
ALLFILES = $(call walk, $(LOCAL_PATH)/../../Classes)
                    
FILE_LIST := hellocpp/main.cpp \
hellocpp/test.cpp \
# 从所有文件中提取出所有.cpp文件
FILE_LIST += $(filter %.cpp, $(ALLFILES))
 
LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)
				   

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../Classes \
$(LOCAL_PATH)/../../cocos2d/libiconv/include \
#$(LOCAL_PATH)/../../cocos2d/libiconv/libcharset \
#$(LOCAL_PATH)/../../cocos2d/libiconv/lib \
#$(LOCAL_PATH)/../../cocos2d/libiconv/libcharset/include \
#$(LOCAL_PATH)/../../cocos2d/libiconv/srclib \
#$(LOCAL_PATH)/../../cocos2d/libiconv \

LOCAL_WHOLE_STATIC_LIBRARIES := cocos2dx_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocosdenshion_static

LOCAL_WHOLE_STATIC_LIBRARIES := cocos_lua_static

# LOCAL_WHOLE_STATIC_LIBRARIES += box2d_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocosbuilder_static
# LOCAL_WHOLE_STATIC_LIBRARIES += spine_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocostudio_static
# LOCAL_WHOLE_STATIC_LIBRARIES += cocos_network_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocos_extension_static

LOCAL_WHOLE_STATIC_LIBRARIES += libiconv_static
include $(BUILD_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := libidentifyapp
LOCAL_SRC_FILES := prebuilt/libidentifyapp.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libcasdkjni
LOCAL_SRC_FILES := prebuilt/libcasdkjni.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libweibosdkcore
LOCAL_SRC_FILES := prebuilt/libweibosdkcore.so
include $(PREBUILT_SHARED_LIBRARY)

$(call import-module,scripting/lua-bindings)

$(call import-module,.)
$(call import-module,audio/android)

# $(call import-module,Box2D)
$(call import-module,editor-support/cocosbuilder)
# $(call import-module,editor-support/spine)
$(call import-module,editor-support/cocostudio)
# $(call import-module,network)
$(call import-module,extensions)
$(call import-module,libiconv)
