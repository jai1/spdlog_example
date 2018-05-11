#include <iostream>
#include <LoggerRegistry.h>

int runBenchMark(int howmany = 1000000, int queue_size = std::pow(2, 20), int threads = 20);
INITIALIZE_LOGGER()

int main(int argc, char *argv[]) {
    std::cout << basename((char *)__FILE__) << std::endl;
    std::cout << "Hello, World!" << std::endl;
    TRACE_LOG("Trace Hello World");
    DEBUG_LOG("Debug Hello World");
    INFO_LOG("Info Hello World");
    WARN_LOG("Warn Hello World => {}", "Jai");
    CRITICAL_LOG("Critical Hello World");
    ACCESS_LOG("{} is feeling sleepy", "Jai")

    return runBenchMark();
}