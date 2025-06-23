#include "CoreUtils.hxx"

namespace CoreUtils {

    // Static member definitions
    std::unique_ptr<SystemInfo> Utils::s_system_info = nullptr;
    bool Utils::s_initialized = false;
    std::mutex Utils::s_mutex;

    SystemInfo& Utils::getSystemInfo() {
        std::lock_guard<std::mutex> lock(s_mutex);
        if (!s_system_info) {
            s_system_info = std::make_unique<SystemInfo>();
        }
        return *s_system_info;
    }

    const char* Utils::getVersion() {
        return VERSION;
    }

    const char* Utils::getBuildDate() {
        return BUILD_DATE;
    }

    void Utils::initialize() {
        std::lock_guard<std::mutex> lock(s_mutex);
        if (!s_initialized) {
            // Future initialization code can go here
            s_initialized = true;
        }
    }

    void Utils::cleanup() {
        std::lock_guard<std::mutex> lock(s_mutex);
        if (s_initialized) {
            s_system_info.reset();
            s_initialized = false;
        }
    }

    // Convenience functions in System namespace
    namespace System {

        std::string getHostname() {
            return Utils::getSystemInfo().getHostname();
        }

        std::string getKernelVersion() {
            return Utils::getSystemInfo().getKernelVersion();
        }

        std::string getDistribution() {
            return Utils::getSystemInfo().getDistribution();
        }

        long getUptime() {
            return Utils::getSystemInfo().getUptime();
        }

        int getCpuCount() {
            return Utils::getSystemInfo().getCpuCount();
        }

        LoadAverage getLoadAverage() {
            return Utils::getSystemInfo().getLoadAverage();
        }

        CpuInfo getCpuInfo() {
            return Utils::getSystemInfo().getCpuInfo();
        }
    }

}