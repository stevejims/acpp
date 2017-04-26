all: bin/blocking bin/threading bin/select bin/epoll bin/callbacks bin/coroutines bin/client_perf bin/client_breaker bin/client_once

clean:
	rm bin/*
	find src/ -name *.o

CXX=clang++-3.8 -O2 -stdlib=libstdc++ --std=c++14 -Isrc

# net

src/net/socket.o: src/net/socket.hpp src/net/socket.cpp
	$(CXX) -c src/net/socket.cpp -o src/net/socket.o

src/net/bind.o: src/net/bind.hpp src/net/bind.cpp
	$(CXX) -c src/net/bind.cpp -o src/net/bind.o

# common

src/common/table.o: src/common/table.hpp src/common/table.cpp
	$(CXX) -c src/common/table.cpp -o src/common/table.o

# clients

bin:
	mkdir -p bin

bin/client_perf: bin src/clients/client_perf.cpp src/net/socket.o src/net/connect.o
	$(CXX) src/clients/client_perf.cpp src/net/socket.o src/net/connect.o -o bin/client_perf

bin/client_breaker: bin src/clients/client_breaker.cpp src/net/socket.o src/net/connect.o
	$(CXX) src/clients/client_breaker.cpp src/net/socket.o src/net/connect.o -o bin/client_breaker

bin/client_once: bin src/clients/client_once.cpp src/net/socket.o src/net/connect.o
	$(CXX) src/clients/client_once.cpp src/net/socket.o src/net/connect.o -o bin/client_once

# servers

bin/blocking: bin src/blocking/blocking.cpp src/common/table.o src/net/socket.o src/net/bind.o
	$(CXX) src/blocking/blocking.cpp src/common/table.o src/net/socket.o src/net/bind.o -o bin/blocking

bin/threading: bin src/threading/threading.cpp src/common/table.o src/net/socket.o src/net/bind.o
	$(CXX) src/threading/threading.cpp src/common/table.o src/net/socket.o src/net/bind.o -o bin/threading -lpthread

bin/select: bin src/select/select.cpp src/common/table.o src/net/socket.o src/net/bind.o src/net/select.o
	$(CXX) src/select/select.cpp src/common/table.o src/net/socket.o src/net/bind.o src/net/select.o -o bin/select

bin/epoll: bin src/epoll/epoll.cpp src/common/table.o src/net/socket.o src/net/bind.o src/net/epoll.o
	$(CXX) src/epoll/epoll.cpp src/common/table.o src/net/socket.o src/net/bind.o src/net/epoll.o -o bin/epoll

bin/callbacks: bin src/callbacks/callbacks.cpp src/common/table.o src/net/socket.o src/net/bind.o src/net/epoll.o  src/callbacks/net_callbacks.hpp src/callbacks/net_callbacks.cpp
	$(CXX) src/callbacks/callbacks.cpp src/callbacks/net_callbacks.cpp src/common/table.o src/net/socket.o src/net/bind.o src/net/epoll.o -o bin/callbacks

bin/coroutines: bin src/coroutines/coroutines.cpp src/common/table.o src/net/socket.o src/net/bind.o src/net/epoll.o src/coroutines/net_coroutines.hpp src/coroutines/net_coroutines.cpp
	$(CXX) src/coroutines/coroutines.cpp src/coroutines/net_coroutines.cpp src/common/table.o src/net/socket.o src/net/bind.o src/net/epoll.o -o bin/coroutines -lboost_coroutine -lboost_system
