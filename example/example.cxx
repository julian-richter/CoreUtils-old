#include "../src/CoreUtils.hxx"
#include <iostream>

int main() {
    CoreUtils::Utils::initialize();
    std::cout << "System Information Demo\n";
    std::cout << "=======================\n";
    std::cout << "Hostname: " << CoreUtils::System::getHostname() << "\n";
    std::cout << "Kernel: " << CoreUtils::System::getKernelVersion() << "\n";
    std::cout << "Distribution: " << CoreUtils::System::getDistribution() << "\n";
    std::cout << "Uptime: " << CoreUtils::System::getUptime() << " seconds\n";
    std::cout << "CPU Count: " << CoreUtils::System::getCpuCount() << "\n";

    auto cpuInfo = CoreUtils::System::getCpuInfo();
    std::cout << "CPU Model: " << cpuInfo.model_name << "\n";
    std::cout << "CPU Frequency: " << cpuInfo.frequency_mhz << " MHz\n";

    CoreUtils::Utils::cleanup();
    return 0;
}