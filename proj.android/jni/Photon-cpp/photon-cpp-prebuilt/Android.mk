# Photon-cpp

LOCAL_PATH := $(call my-dir)

all_static_libraries = common-cpp-static-prebuilt

lib_suffix := ${APP_OPTIM}_android_${APP_ABI}

lib_photon_cpp_static_name := photon-cpp-static_${lib_suffix}

include $(CLEAR_VARS)
LOCAL_MODULE            := photon-cpp-static-prebuilt
LOCAL_SRC_FILES         := lib$(lib_photon_cpp_static_name).a
LOCAL_STATIC_LIBRARIES  := $(all_static_libraries)
include $(PREBUILT_STATIC_LIBRARY)

$(call import-add-path, $(shell pwd)/../../../../Common-cpp/src/android)
$(call import-add-path, $(shell pwd)/../../../../Common-cpp)

$(call import-module,common-cpp-prebuilt)
