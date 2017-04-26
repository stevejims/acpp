#include "net/socket.hpp"
#include "net/connect.hpp"

#include <iostream>
#include <vector>
#include <chrono>

using namespace net;

class Timer {
public:
	typedef std::chrono::steady_clock Clock;
	Timer()
		: start_(Clock::now()) {
	}
    std::size_t elapsed_ns() {
		const Clock::duration result(Clock::now() - start_);
		return result.count();
	}
private:
	Clock::time_point start_;
};

void concurrent_connections(const std::size_t num_iterations,
                            const std::size_t num_connections,
                            const std::size_t num_requests)
{
    Timer t;

    std::string result;
    std::vector<net::socket> connections;
    
    for (std::size_t i = 0; i < num_iterations; ++i) {
        connections.clear();

        for (std::size_t i = 0; i < num_connections; ++i) {
            connections.push_back(connect_local("socket"));
        }
        
        for (std::size_t j = 0; j < num_requests; ++j) {
            for (auto &socket : connections) {
                socket.send("one");
                socket.recv();
            }
        }        
    }
    const auto ns = t.elapsed_ns();
    std::cout << ns << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc!=3) {
        std::cout << "Usage: ./client_perf <num_iterations> <num_connections>"
                  << std::endl;
        return 1;
    }
    const auto num_iterations = std::stoull(argv[1]);
    const auto num_connections = std::stoull(argv[2]);

    concurrent_connections(num_iterations, num_connections, 10);
    
    return 0;
}
