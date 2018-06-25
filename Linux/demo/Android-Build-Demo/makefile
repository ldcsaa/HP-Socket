SRC_DIR:=../../src
CSRC:=$(SRC_DIR)/HPSocket4C.cpp
CPPSRC:=$(SRC_DIR)/HPSocket.cpp
SRC:=$(SRC_DIR)/HttpAgent.cpp $(SRC_DIR)/HttpClient.cpp $(SRC_DIR)/HttpCookie.cpp $(SRC_DIR)/HttpHelper.cpp $(SRC_DIR)/HttpServer.cpp $(SRC_DIR)/MiscHelper.cpp $(SRC_DIR)/SocketHelper.cpp $(SRC_DIR)/TcpAgent.cpp $(SRC_DIR)/TcpClient.cpp $(SRC_DIR)/TcpPackAgent.cpp $(SRC_DIR)/TcpPackClient.cpp $(SRC_DIR)/TcpPackServer.cpp $(SRC_DIR)/TcpPullAgent.cpp $(SRC_DIR)/TcpPullClient.cpp $(SRC_DIR)/TcpPullServer.cpp $(SRC_DIR)/TcpServer.cpp $(SRC_DIR)/UdpCast.cpp $(SRC_DIR)/UdpClient.cpp $(SRC_DIR)/UdpServer.cpp $(SRC_DIR)/common/BufferPool.cpp $(SRC_DIR)/common/Event.cpp $(SRC_DIR)/common/FuncHelper.cpp $(SRC_DIR)/common/IODispatcher.cpp $(SRC_DIR)/common/PollHelper.cpp $(SRC_DIR)/common/RWLock.cpp $(SRC_DIR)/common/FileHelper.cpp $(SRC_DIR)/common/SysHelper.cpp $(SRC_DIR)/common/Thread.cpp $(SRC_DIR)/common/http/http_parser.cpp $(SRC_DIR)/common/crypto/Crypto.cpp helper.cpp

CXXFLAGS:=-DNDEBUG -D_SSL_DISABLED -D_NEED_HTTP -D_ZLIB_DISABLED -D_ICONV_DISABLED -std=c++1y -lc -g0 -O2 -fPIC -DPIC -fvisibility=hidden -Wl,-Bsymbolic $(CXXFLAGS)

HTTP_TARGETS:=build/http/shd build/http/chd build/http/ahd build/http/shs build/http/chs build/http/ahs
LIB_TARGETS:=build/lib/libhpsoket.so build/lib/libhpsoket4c.so build/lib/libhpsocket.a build/lib/libhpsocket4c.a
TARGETS:=$(HTTP_TARGETS)

.PHONY:all lib http clean

all: lib demo


demo: pack-demo udp-demo
	
	
udp-demo: server-udp.cpp lib
	mkdir -p build
	mkdir -p build/udp
	# make static executable binary by c++
	$(CXX) -static $(CXXFLAGS) -o build/udp/server-s -L`pwd`/build/lib/ server-udp.cpp -lhpsocket
	# make dynamic executable binary by c++
	$(CXX) -fPIE -pie -DPIE $(CXXFLAGS) -o build/udp/server-d -L`pwd`/build/lib/ server-udp.cpp -lhpsocket
	
	
pack-demo: server-pack.cpp client-pack.cpp agent-pack.cpp lib
	mkdir -p build
	mkdir -p build/pack
	# make static executable binary by c++
	$(CXX) -static $(CXXFLAGS) -o build/pack/server-s -L`pwd`/build/lib/ server-pack.cpp -lhpsocket
	$(CXX) -static $(CXXFLAGS) -o build/pack/client-s -L`pwd`/build/lib/ client-pack.cpp -lhpsocket
	$(CXX) -static $(CXXFLAGS) -o build/pack/agent-s  -L`pwd`/build/lib/  agent-pack.cpp -lhpsocket
	# make dynamic executable binary by c++
	$(CXX) -fPIE -pie -DPIE $(CXXFLAGS) -o build/pack/server-d -L`pwd`/build/lib/ server-pack.cpp -lhpsocket
	$(CXX) -fPIE -pie -DPIE $(CXXFLAGS) -o build/pack/client-d -L`pwd`/build/lib/ client-pack.cpp -lhpsocket
	$(CXX) -fPIE -pie -DPIE $(CXXFLAGS) -o build/pack/agent-d  -L`pwd`/build/lib/  agent-pack.cpp -lhpsocket
	
lib: build/lib $(LIB_TARGETS)


build/lib/libhpsoket.so: $(SRC) $(CPPSRC)
	$(CXX) -shared $(CXXFLAGS) -o build/lib/libhpsoket.so $(SRC) $(CPPSRC)

build/lib/libhpsoket4c.so: $(SRC) $(CSRC)
	$(CXX) -shared $(CXXFLAGS) -o build/lib/libhpsoket4c.so $(SRC) $(CSRC)

build/lib/libhpsocket.a: $(SRC) $(CPPSRC) $(CSRC)
	$(CXX) -static $(CXXFLAGS) -c $(SRC) $(CPPSRC) $(CSRC)
	$(AR) rcs -o build/lib/libhpsocket.a $(notdir $(SRC:.cpp=.o)) $(notdir $(CPPSRC:.cpp=.o))
	$(AR) rcs -o build/lib/libhpsocket4c.a $(notdir $(SRC:.cpp=.o)) $(notdir $(CSRC:.cpp=.o))
	rm -f *.o

http: build/http $(HTTP_TARGETS)

	
build/http/shd: $(CSRC) $(SRC) server-http.cpp
	$(CXX) -fPIE -pie -DPIE $(CXXFLAGS) -o build/http/shd $(CSRC) $(SRC) server-http.cpp
	
build/http/chd: $(CSRC) $(SRC) client-http.cpp
	$(CXX) -fPIE -pie -DPIE $(CXXFLAGS) -o build/http/chd $(CSRC) $(SRC) client-http.cpp
	
build/http/ahd: $(CSRC) $(SRC) agent-http.cpp
	$(CXX) -fPIE -pie -DPIE $(CXXFLAGS) -o build/http/ahd $(CSRC) $(SRC) agent-http.cpp
	
build/http/shs: $(CSRC) $(SRC) server-http.cpp
	$(CXX) -static $(CXXFLAGS) -o build/http/shs $(CSRC) $(SRC) server-http.cpp
	
build/http/chs: $(CSRC) $(SRC) client-http.cpp
	$(CXX) -static $(CXXFLAGS) -o build/http/chs $(CSRC) $(SRC) client-http.cpp
	
build/http/ahs: $(CSRC) $(SRC) agent-http.cpp
	$(CXX) -static $(CXXFLAGS) -o build/http/ahs $(CSRC) $(SRC) agent-http.cpp
	
build:
	mkdir -p build
	
build/http: build
	mkdir -p build/http
	
build/lib: build
	mkdir -p build/lib
	
clean:
	rm -rf build
	