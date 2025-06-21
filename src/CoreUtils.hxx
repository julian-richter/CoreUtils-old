#pragma once

#include "System/SystemInfo.hxx"
#include <memory>

namespace CoreUtils {

    // Version info
    constexpr const char* VERSION = "1.0.0";
    constexpr const char* BUILD_DATE = __DATE__;

    // Main API entry point
    class CoreUtils {
    public:
        // Get system information instance
        static SystemInfo& getSystemInfo();

        // Library info
        static const char* getVersion();
        static const char* getBuildDate();

        // Initialize/cleanup (if needed for future extensions)
        static void initialize();
        static void cleanup();

    private:
        // Singleton instances
        static std::unique_ptr<SystemInfo> s_system_info;
        static bool s_initialized;
    };

    // Convenience functions for quick access
    namespace System {
        std::string getHostname();
        std::string getKernelVersion();
        std::string getDistribution();
        long getUptime();
        int getCpuCount();
        LoadAverage getLoadAverage();
        CpuInfo getCpuInfo();
    }
}