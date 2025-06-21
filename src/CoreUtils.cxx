#include "CoreUtils.hxx"

namespace CoreUtils {

    // Static member definitions
    std::unique_ptr<SystemInfo> CoreUtils::s_system_info = nullptr;
    bool CoreUtils::s_initialized = false;

    SystemInfo& CoreUtils::getSystemInfo() {
        if (!s_system_info) {
            s_system_info = std::make_unique<SystemInfo>();
        }
        return *s_system_info;
    }

    const char* CoreUtils::getVersion() {
        return VERSION;
    }

    const char* CoreUtils::getBuildDate() {
        return BUILD_DATE;
    }

    void CoreUtils::initialize() {
        if (!s_initialized) {
            // Future initialization code can go here
            s_initialized = true;
        }
    }

    void CoreUtils::cleanup() {
        if (s_initialized) {
            s_system_info.reset();
            s_initialized = false;
        }
    }

    // Convenience functions in System namespace
    namespace System {

        std::string getHostname() {
            return CoreUtils::getSystemInfo().getHostname();
        }

        std::string getKernelVersion() {
            return CoreUtils::getSystemInfo().getKernelVersion();
        }

        std::string getDistribution() {
            return CoreUtils::getSystemInfo().getDistribution();
        }

        long getUptime() {
            return CoreUtils::getSystemInfo().getUptime();
        }

        int getCpuCount() {
            return CoreUtils::getSystemInfo().getCpuCount();
        }

        LoadAverage getLoadAverage() {
            return CoreUtils::getSystemInfo().getLoadAverage();
        }

        CpuInfo getCpuInfo() {
            return CoreUtils::getSystemInfo().getCpuInfo();
        }
    }

}