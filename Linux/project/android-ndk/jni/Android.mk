LOCAL_PATH   := $(call my-dir)

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
                ../../../src/UdpClient.cpp \
                ../../../src/UdpServer.cpp
MY_C_INCLUDES := $(LOCAL_PATH)/../../../src
MY_CFLAGS     := -fpic -fvisibility=hidden -fno-strict-aliasing -Wall -Wextra -Wswitch -Wno-deprecated-declarations -Wno-empty-body -Wno-conversion -Wno-sign-conversion -Wno-sign-compare -Wreturn-type -Wparentheses -Wno-pointer-sign -Wno-format -Wno-missing-braces -Wuninitialized -Wunreachable-code -Wunused-function -Wunused-value -Wunused-variable -Wno-unused-parameter -Wno-missing-field-initializers
MY_CPPFLAGS   := -fexceptions -frtti -fthreadsafe-statics -Wno-reorder -Wno-inconsistent-missing-override
MY_LDFLAGS    := -Wl,--no-undefined -Wl,-z,relro -Wl,-z,now -Wl,-z,noexecstack -Wl,-Bsymbolic
MY_LDLIBS     := -lz -ldl

ifneq ($(TARGET_ARCH_ABI),mips)
ifneq ($(TARGET_ARCH_ABI),mips64)
  MY_CFLAGS += -mfloat-abi=softfp
endif
endif

include $(CLEAR_VARS)
LOCAL_MODULE     := hpsocket_a
LOCAL_MODULE_FILENAME := libhpsocket
LOCAL_SRC_FILES  := $(MY_SRC_FILES) \
                    ../../../src/HPSocket.cpp \
                    ../../../src/HPSocket-SSL.cpp
LOCAL_C_INCLUDES := $(MY_C_INCLUDES)
LOCAL_CFLAGS     := $(MY_CFLAGS)
LOCAL_CPPFLAGS   := $(MY_CPPFLAGS)
include $(BUILD_STATIC_LIBRARY)

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


include $(CLEAR_VARS)
LOCAL_MODULE     := hpsocket4c_a
LOCAL_MODULE_FILENAME := libhpsocket4c
LOCAL_SRC_FILES  := $(MY_SRC_FILES) \
                    ../../../src/HPSocket4C.cpp \
                    ../../../src/HPSocket4C-SSL.cpp
LOCAL_C_INCLUDES := $(MY_C_INCLUDES)
LOCAL_CFLAGS     := $(MY_CFLAGS)
LOCAL_CPPFLAGS   := $(MY_CPPFLAGS)
include $(BUILD_STATIC_LIBRARY)

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
