LOCAL_PATH := $(call my-dir)

# target lib : hp-testecho-ssl-pull-server
include $(CLEAR_VARS)
LOCAL_MODULE     := hp-testecho-ssl-pull-server
LOCAL_SRC_FILES  := server/server.cpp \
					../global/helper.cpp \
					../../src/common/BufferPool.cpp \
					../../src/common/FileHelper.cpp \
					../../src/common/FuncHelper.cpp \
					../../src/common/SysHelper.cpp \
					../../src/common/Thread.cpp	
LOCAL_C_INCLUDES := $(MY_C_INCLUDES)
LOCAL_CFLAGS     := $(MY_CFLAGS) -D_USE_HP_LIB -D_NEED_SSL -D_NEED_HTTP
LOCAL_CPPFLAGS   := $(MY_CPPFLAGS)
LOCAL_LDLIBS     := $(MY_LDLIBS)
LOCAL_SHARED_LIBRARIES:=hpsocket_so
include $(BUILD_EXECUTABLE)

# target lib :  hp-testecho-ssl-pull-client
include $(CLEAR_VARS)
LOCAL_MODULE     := hp-testecho-ssl-pull-client
LOCAL_SRC_FILES  := client/client.cpp \
					../global/helper.cpp \
					../../src/common/BufferPool.cpp \
					../../src/common/FileHelper.cpp \
					../../src/common/FuncHelper.cpp \
					../../src/common/SysHelper.cpp \
					../../src/common/Thread.cpp	
LOCAL_C_INCLUDES := $(MY_C_INCLUDES)
LOCAL_CFLAGS     := $(MY_CFLAGS) -D_USE_HP_LIB -D_NEED_SSL -D_NEED_HTTP
LOCAL_CPPFLAGS   := $(MY_CPPFLAGS)
LOCAL_LDLIBS     := $(MY_LDLIBS)
LOCAL_SHARED_LIBRARIES:=hpsocket_so
include $(BUILD_EXECUTABLE)

# target lib :  hp-testecho-ssl-pull-agent
include $(CLEAR_VARS)
LOCAL_MODULE     := hp-testecho-ssl-pull-agent
LOCAL_SRC_FILES  := agent/agent.cpp \
					../global/helper.cpp \
					../../src/common/BufferPool.cpp \
					../../src/common/FileHelper.cpp \
					../../src/common/FuncHelper.cpp \
					../../src/common/SysHelper.cpp \
					../../src/common/Thread.cpp	
LOCAL_C_INCLUDES := $(MY_C_INCLUDES)
LOCAL_CFLAGS     := $(MY_CFLAGS) -D_USE_HP_LIB -D_NEED_SSL -D_NEED_HTTP
LOCAL_CPPFLAGS   := $(MY_CPPFLAGS)
LOCAL_LDLIBS     := $(MY_LDLIBS)
LOCAL_SHARED_LIBRARIES:=hpsocket_so
include $(BUILD_EXECUTABLE)
