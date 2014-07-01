LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := cocos2dcpp_shared

LOCAL_MODULE_FILENAME := libcocos2dcpp

LOCAL_SRC_FILES := hellocpp/main.cpp \
                   ../../Classes/AppDelegate.cpp \
                   ../../Classes/HelloWorldScene.cpp \
                   ../../Classes/Photon/PhotonKeys.cpp \
                   ../../Classes/Photon/NetworkLogic.cpp \
                   ../../Classes/Photon/CocosNetworkLogic.cpp \
                   ../../Classes/Photon/CCConfirmMessage.cpp \
                   ../../Classes/Photon/CCRoomTableCell.cpp \
                   ../../Classes/Photon/CCLobbyViewDataSource.cpp \
                   ../../Classes/Photon/CCLobbyView.cpp \
                   ../../Classes/airhockey/GameLayer.cpp \
                   ../../Classes/airhockey/GameSprite.cpp

LOCAL_C_INCLUDES := $(LOCAL_PATH)/Common-cpp/inc \
					$(LOCAL_PATH)/Photon-cpp/inc \
					$(LOCAL_PATH)/LoadBalancing-cpp/inc \
					$(LOCAL_PATH)/../../Classes \
					$(LOCAL_PATH)/../../Classes/Photon \
					$(LOCAL_PATH)/../../Classes/airhockey

LOCAL_STATIC_LIBRARIES := photon-cpp-static-prebuilt common-cpp-static-prebuilt loadbalancing-cpp-static-prebuilt

LOCAL_WHOLE_STATIC_LIBRARIES += cocos2dx_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocosdenshion_static
LOCAL_WHOLE_STATIC_LIBRARIES += box2d_static
LOCAL_WHOLE_STATIC_LIBRARIES += chipmunk_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocos_extension_static

include $(BUILD_SHARED_LIBRARY)

$(call import-module,cocos2dx)
$(call import-module,cocos2dx/platform/third_party/android/prebuilt/libcurl)
$(call import-module,CocosDenshion/android)
$(call import-module,extensions)
$(call import-module,external/Box2D)
$(call import-module,external/chipmunk)

$(call import-add-path, $(shell pwd)/jni/Common-cpp)
$(call import-add-path, $(shell pwd)/jni/Photon-cpp)
$(call import-add-path, $(shell pwd)/jni/LoadBalancing-cpp)
$(call import-module, common-cpp-prebuilt)
$(call import-module, photon-cpp-prebuilt)
$(call import-module, loadbalancing-cpp-prebuilt)