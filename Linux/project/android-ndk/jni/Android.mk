MY_SSL_DISABLED    := false
MY_UDP_DISABLED    := false
MY_HTTP_DISABLED   := false
MY_ZLIB_DISABLED   := false
MY_ICONV_DISABLED  := false

LOCAL_PATH         := $(call my-dir)

ifdef _SSL_DISABLED
  ifeq ($(_SSL_DISABLED),true)
    MY_SSL_DISABLED := $(_SSL_DISABLED)
  endif
endif

ifdef _UDP_DISABLED
  ifeq ($(_UDP_DISABLED),true)
    MY_UDP_DISABLED := $(_UDP_DISABLED)
  endif
endif

ifdef _HTTP_DISABLED
  ifeq ($(_HTTP_DISABLED),true)
    MY_HTTP_DISABLED := $(_HTTP_DISABLED)
  endif
endif

ifdef _ZLIB_DISABLED
  ifeq ($(_ZLIB_DISABLED),true)
    MY_ZLIB_DISABLED := $(_ZLIB_DISABLED)
  endif
endif

ifdef _ICONV_DISABLED
  ifeq ($(_ICONV_DISABLED),true)
    MY_ICONV_DISABLED := $(_ICONV_DISABLED)
  endif
endif

MY_SRC_FILES := ../../../src/common/BufferPool.cpp \
                ../../../src/common/crypto/Crypto.cpp \
                ../../../src/common/Event.cpp \
                ../../../src/common/FileHelper.cpp \
                ../../../src/common/FuncHelper.cpp \
                ../../../src/common/http/http_parser.c \
                ../../../src/common/IODispatcher.cpp \
                ../../../src/common/kcp/ikcp.c \
                ../../../src/common/PollHelper.cpp \
                ../../../src/common/RWLock.cpp \
                ../../../src/common/SysHelper.cpp \
                ../../../src/common/Thread.cpp \
                ../../../src/ArqHelper.cpp \
                ../../../src/HPThreadPool.cpp \
                ../../../src/HttpAgent.cpp \
                ../../../src/HttpClient.cpp \
                ../../../src/HttpCookie.cpp \
                ../../../src/HttpHelper.cpp \
                ../../../src/HttpServer.cpp \
                ../../../src/MiscHelper.cpp \
                ../../../src/SocketHelper.cpp \
                ../../../src/SSLAgent.cpp \
                ../../../src/SSLClient.cpp \
                ../../../src/SSLHelper.cpp \
                ../../../src/SSLServer.cpp \
                ../../../src/TcpAgent.cpp \
                ../../../src/TcpClient.cpp \
                ../../../src/TcpPackAgent.cpp \
                ../../../src/TcpPackClient.cpp \
                ../../../src/TcpPackServer.cpp \
                ../../../src/TcpPullAgent.cpp \
                ../../../src/TcpPullClient.cpp \
                ../../../src/TcpPullServer.cpp \
                ../../../src/TcpServer.cpp \
                ../../../src/UdpArqClient.cpp \
                ../../../src/UdpArqServer.cpp \
                ../../../src/UdpCast.cpp \
                ../../../src/UdpNode.cpp \
                ../../../src/UdpClient.cpp \
                ../../../src/UdpServer.cpp
MY_C_INCLUDES := ../../../src
MY_CFLAGS     := -fpic -fvisibility=hidden -fno-strict-aliasing -Wall -Wextra -Wswitch -Wno-deprecated-declarations -Wno-empty-body -Wno-conversion -Wno-sign-conversion -Wno-sign-compare -Wreturn-type -Wparentheses -Wno-pointer-sign -Wno-format -Wno-missing-braces -Wuninitialized -Wunreachable-code -Wunused-function -Wunused-value -Wunused-variable -Wno-unused-parameter -Wno-missing-field-initializers
MY_CPPFLAGS   := -fexceptions -frtti -fthreadsafe-statics -Wno-reorder -Wno-inconsistent-missing-override
MY_LDFLAGS    := -Wl,--no-undefined -Wl,-z,relro -Wl,-z,now -Wl,-z,noexecstack -Wl,-Bsymbolic
MY_LDLIBS     := -ldl
MY_WHOLE_STATIC_LIBRARIES :=

ifeq ($(MY_UDP_DISABLED),true)
  MY_CFLAGS += -D_UDP_DISABLED
endif

ifeq ($(MY_HTTP_DISABLED),true)
  MY_CFLAGS += -D_HTTP_DISABLED
endif

ifeq ($(MY_ZLIB_DISABLED),true)
  MY_CFLAGS += -D_ZLIB_DISABLED
else
  MY_LDLIBS += -lz
endif

ifneq ($(TARGET_ARCH_ABI),mips)
  ifneq ($(TARGET_ARCH_ABI),mips64)

    MY_CFLAGS += -mfloat-abi=softfp
	
	ifeq ($(MY_SSL_DISABLED),true)
      MY_CFLAGS += -D_SSL_DISABLED
    else
      MY_WHOLE_STATIC_LIBRARIES += ssl crypto
    endif
	
	ifeq ($(MY_ICONV_DISABLED),true)
      MY_CFLAGS += -D_ICONV_DISABLED
    else
      MY_WHOLE_STATIC_LIBRARIES += iconv
    endif

  else
    MY_CFLAGS += -D_SSL_DISABLED -D_ICONV_DISABLED
  endif
else
  MY_CFLAGS += -D_SSL_DISABLED -D_ICONV_DISABLED
endif

#$(call __ndk_warning,MY_CFLAGS is value '$(MY_CFLAGS)')

# local lib : charset
#include $(CLEAR_VARS)
#LOCAL_MODULE    := charset
#LOCAL_SRC_FILES := ../../../dependent/android-ndk/$(TARGET_ARCH_ABI)/lib/libcharset.a
#LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../../../dependent/android-ndk/$(TARGET_ARCH_ABI)/include
#include $(PREBUILT_STATIC_LIBRARY)

# local lib : iconv
include $(CLEAR_VARS)
LOCAL_MODULE    := iconv
LOCAL_SRC_FILES := ../../../dependent/android-ndk/$(TARGET_ARCH_ABI)/lib/libiconv.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../../../dependent/android-ndk/$(TARGET_ARCH_ABI)/include
include $(PREBUILT_STATIC_LIBRARY)

# local lib : crypto
include $(CLEAR_VARS)
LOCAL_MODULE    := crypto
LOCAL_SRC_FILES := ../../../dependent/android-ndk/$(TARGET_ARCH_ABI)/lib/libcrypto.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../../../dependent/android-ndk/$(TARGET_ARCH_ABI)/include
include $(PREBUILT_STATIC_LIBRARY)

# local lib : ssl
include $(CLEAR_VARS)
LOCAL_MODULE    := ssl
LOCAL_SRC_FILES := ../../../dependent/android-ndk/$(TARGET_ARCH_ABI)/lib/libssl.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../../../dependent/android-ndk/$(TARGET_ARCH_ABI)/include
include $(PREBUILT_STATIC_LIBRARY)


# target lib : hpsocket.a
include $(CLEAR_VARS)
LOCAL_MODULE     := hpsocket_a
LOCAL_MODULE_FILENAME := libhpsocket
LOCAL_SRC_FILES  := $(MY_SRC_FILES) \
                    ../../../src/HPSocket.cpp \
                    ../../../src/HPSocket-SSL.cpp
LOCAL_WHOLE_STATIC_LIBRARIES := $(MY_WHOLE_STATIC_LIBRARIES)
LOCAL_C_INCLUDES := $(MY_C_INCLUDES)
LOCAL_CFLAGS     := $(MY_CFLAGS)
LOCAL_CPPFLAGS   := $(MY_CPPFLAGS)
include $(BUILD_STATIC_LIBRARY)

# target lib : hpsocket.so
include $(CLEAR_VARS)
LOCAL_MODULE     := hpsocket_so
LOCAL_MODULE_FILENAME := libhpsocket
LOCAL_WHOLE_STATIC_LIBRARIES := hpsocket_a
LOCAL_C_INCLUDES := $(MY_C_INCLUDES)
LOCAL_CFLAGS     := $(MY_CFLAGS)
LOCAL_CPPFLAGS   := $(MY_CPPFLAGS)
LOCAL_LDFLAGS    := $(MY_LDFLAGS)
LOCAL_LDLIBS     := $(MY_LDLIBS)
include $(BUILD_SHARED_LIBRARY)


# target lib : hpsocket4c.a
include $(CLEAR_VARS)
LOCAL_MODULE     := hpsocket4c_a
LOCAL_MODULE_FILENAME := libhpsocket4c
LOCAL_SRC_FILES  := $(MY_SRC_FILES) \
                    ../../../src/HPSocket4C.cpp \
                    ../../../src/HPSocket4C-SSL.cpp
LOCAL_WHOLE_STATIC_LIBRARIES := $(MY_WHOLE_STATIC_LIBRARIES)
LOCAL_C_INCLUDES := $(MY_C_INCLUDES)
LOCAL_CFLAGS     := $(MY_CFLAGS)
LOCAL_CPPFLAGS   := $(MY_CPPFLAGS)
include $(BUILD_STATIC_LIBRARY)

# target lib : hpsocket4c.so
include $(CLEAR_VARS)
LOCAL_MODULE     := hpsocket4c_so
LOCAL_MODULE_FILENAME := libhpsocket4c
LOCAL_WHOLE_STATIC_LIBRARIES := hpsocket4c_a
LOCAL_C_INCLUDES := $(MY_C_INCLUDES)
LOCAL_CFLAGS     := $(MY_CFLAGS)
LOCAL_CPPFLAGS   := $(MY_CPPFLAGS)
LOCAL_LDFLAGS    := $(MY_LDFLAGS)
LOCAL_LDLIBS     := $(MY_LDLIBS)
include $(BUILD_SHARED_LIBRARY)
