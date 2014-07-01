# LoadBalancing-cpp

LOCAL_PATH := $(call my-dir)

all_static_libraries = common-cpp-static-prebuilt \
                       photon-cpp-static-prebuilt

lib_suffix := ${APP_OPTIM}_android_${APP_ABI}

lib_loadbalancing_cpp_static_name := loadbalancing-cpp-prebuilt-static_${lib_suffix}

include $(CLEAR_VARS)
LOCAL_MODULE            := loadbalancing-cpp-static-prebuilt
LOCAL_SRC_FILES         := lib$(lib_loadbalancing_cpp_static_name).a
LOCAL_STATIC_LIBRARIES  := $(all_static_libraries)
include $(PREBUILT_STATIC_LIBRARY)

$(call import-add-path, $(shell pwd)/../../../../Common-cpp/src/android)
$(call import-add-path, $(shell pwd)/../../../../Photon-cpp/src/android)
$(call import-add-path, $(shell pwd)/../../../../Common-cpp)
$(call import-add-path, $(shell pwd)/../../../../Photon-cpp)

$(call import-module,common-cpp-prebuilt)
$(call import-module,photon-cpp-prebuilt)
