LOCAL_PATH    := $(call my-dir)
ROOT_PATH     := $(LOCAL_PATH)
DEPT_PATH     := $(LOCAL_PATH)/../dependent/android-ndk/$(TARGET_ARCH_ABI)
DEPT_INC_PATH := $(DEPT_PATH)/include
DEPT_LIB_PATH := $(DEPT_PATH)/lib
HP_LIB_PATH   := $(LOCAL_PATH)/../lib/android-ndk/$(TARGET_ARCH_ABI)

MY_C_INCLUDES := $(LOCAL_PATH)/../include
MY_CFLAGS     := -w -fPIC -fvisibility=hidden -fno-strict-aliasing -Wall -Wextra -Wswitch -Wno-deprecated-declarations -Wno-empty-body -Wno-conversion -Wno-sign-conversion -Wno-sign-compare -Wreturn-type -Wparentheses -Wno-pointer-sign -Wno-format -Wno-missing-braces -Wuninitialized -Wunreachable-code -Wunused-function -Wunused-value -Wunused-variable -Wno-unused-parameter -Wno-missing-field-initializers
MY_CPPFLAGS   := -fexceptions -frtti -fthreadsafe-statics -Wno-reorder -Wno-inconsistent-missing-override
MY_LDFLAGS    := -Wl,--no-undefined -Wl,-z,relro -Wl,-z,now -Wl,-z,noexecstack -Wl,-Bsymbolic
MY_WHOLE_STATIC_LIBRARIES := ssl crypto iconv
MY_LDLIBS     := -lz -lm

# local lib : iconv
include $(CLEAR_VARS)
LOCAL_MODULE    := iconv
LOCAL_SRC_FILES := $(DEPT_LIB_PATH)/libiconv.a
LOCAL_EXPORT_C_INCLUDES := $(DEPT_INC_PATH)
include $(PREBUILT_STATIC_LIBRARY)

# local lib : crypto
include $(CLEAR_VARS)
LOCAL_MODULE    := crypto
LOCAL_SRC_FILES := $(DEPT_LIB_PATH)/libcrypto.a
LOCAL_EXPORT_C_INCLUDES := $(DEPT_INC_PATH)
include $(PREBUILT_STATIC_LIBRARY)

# local lib : ssl
include $(CLEAR_VARS)
LOCAL_MODULE    := ssl
LOCAL_SRC_FILES := $(DEPT_LIB_PATH)/libssl.a
LOCAL_EXPORT_C_INCLUDES := $(DEPT_INC_PATH)
include $(PREBUILT_STATIC_LIBRARY)

# local lib : hpsocket
include $(CLEAR_VARS)
LOCAL_MODULE := hpsocket
LOCAL_SRC_FILES := $(HP_LIB_PATH)/libhpsocket.a
include $(PREBUILT_STATIC_LIBRARY)

# local lib : hpsocket4c
include $(CLEAR_VARS)
LOCAL_MODULE := hpsocket4c
LOCAL_SRC_FILES := $(HP_LIB_PATH)/libhpsocket4c.a
include $(PREBUILT_STATIC_LIBRARY)

# local lib : hpsocket_so
include $(CLEAR_VARS)
LOCAL_MODULE := hpsocket_so
LOCAL_SRC_FILES := $(HP_LIB_PATH)/libhpsocket.so
include $(PREBUILT_SHARED_LIBRARY)

# local lib : hpsocket4c_so
include $(CLEAR_VARS)
LOCAL_MODULE := hpsocket4c_so
LOCAL_SRC_FILES := $(HP_LIB_PATH)/libhpsocket4c.so
include $(PREBUILT_SHARED_LIBRARY)

### build all demos ###
include $(call all-makefiles-under,$(ROOT_PATH))

### build single demo ###

# #testecho
# include $(ROOT_PATH)/testecho/Android.mk

# #testecho-arq
# include $(ROOT_PATH)/testecho-arq/Android.mk

# #testecho-arq-pfm
# include $(ROOT_PATH)/testecho-arq-pfm/Android.mk

##testecho-http
#include $(ROOT_PATH)/testecho-http/Android.mk

# #testecho-http-4c(4c so)
#include $(ROOT_PATH)/testecho-http-4c/Android.mk

##testecho-lib(so & 4c so)
#include $(ROOT_PATH)/testecho-lib/Android.mk

##testecho-pack
#include $(ROOT_PATH)/testecho-pack/Android.mk

##testecho-pfm
#include $(ROOT_PATH)/testecho-pfm/Android.mk

##testecho-pull
#include $(ROOT_PATH)/testecho-pull/Android.mk

##testecho-ssl
# include $(ROOT_PATH)/testecho-ssl/Android.mk

##testecho-ssl-pack(4c so)
#include $(ROOT_PATH)/testecho-ssl-pack/Android.mk

##testecho-ssl-pfm(so)
#include $(ROOT_PATH)/testecho-ssl-pfm/Android.mk

##testecho-ssl-pull(so)
#include $(ROOT_PATH)/testecho-ssl-pull/Android.mk

##testecho-udp
#include $(ROOT_PATH)/testecho-udp/Android.mk

##testecho-udp-pfm
#include $(ROOT_PATH)/testecho-udp-pfm/Android.mk
