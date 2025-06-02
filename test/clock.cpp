#include <iostream>
#include <chrono>
#include <iomanip>

int main() {
    // Get current time
    auto now = std::chrono::system_clock::now();
    std::time_t current_time = std::chrono::system_clock::to_time_t(now);

    // Convert to local time
    std::tm* local_time = std::localtime(&current_time);

    // Print formatted time as H:M:S
    std::cout << std::put_time(local_time, "%H:%M:%S") << std::endl;

    return 0;
}

