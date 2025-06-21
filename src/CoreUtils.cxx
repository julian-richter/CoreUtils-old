#include "CoreUtils.hxx"

namespace CoreUtils {

    // Static member definitions
    std::unique_ptr<SystemInfo> CoreUtils::s_system_info = nullptr;
    bool CoreUtils::s_initialized = false;

    /**
     * @brief Returns a reference to the singleton SystemInfo instance.
     *
     * Lazily initializes the SystemInfo object if it does not already exist.
     * @return Reference to the singleton SystemInfo instance.
     */
    SystemInfo& CoreUtils::getSystemInfo() {
        if (!s_system_info) {
            s_system_info = std::make_unique<SystemInfo>();
        }
        return *s_system_info;
    }

    /**
     * @brief Returns the version string of the application.
     *
     * @return const char* Null-terminated version string defined at compile time.
     */
    const char* CoreUtils::getVersion() {
        return VERSION;
    }

    /**
     * @brief Returns the build date of the application.
     *
     * @return const char* Compile-time build date string.
     */
    const char* CoreUtils::getBuildDate() {
        return BUILD_DATE;
    }

    /**
     * @brief Marks the CoreUtils system as initialized.
     *
     * Sets the initialization flag to true if it is not already set. Intended as an entry point for future initialization logic.
     */
    void CoreUtils::initialize() {
        if (!s_initialized) {
            // Future initialization code can go here
            s_initialized = true;
        }
    }

    /**
     * @brief Releases system resources and resets initialization state.
     *
     * Cleans up the singleton SystemInfo instance and marks the CoreUtils system as uninitialized.
     */
    void CoreUtils::cleanup() {
        if (s_initialized) {
            s_system_info.reset();
            s_initialized = false;
        }
    }

    // Convenience functions in System namespace
    namespace System {

        /**
         * @brief Retrieves the system's hostname.
         *
         * @return std::string The hostname of the current system.
         */
        std::string getHostname() {
            return CoreUtils::getSystemInfo().getHostname();
        }

        /**
         * @brief Retrieves the system kernel version as a string.
         *
         * @return std::string The kernel version of the current system.
         */
        std::string getKernelVersion() {
            return CoreUtils::getSystemInfo().getKernelVersion();
        }

        /**
         * @brief Retrieves the name of the operating system distribution.
         *
         * @return std::string The system distribution name.
         */
        std::string getDistribution() {
            return CoreUtils::getSystemInfo().getDistribution();
        }

        /**
         * @brief Retrieves the system uptime in seconds.
         *
         * @return long Number of seconds the system has been running.
         */
        long getUptime() {
            return CoreUtils::getSystemInfo().getUptime();
        }

        /**
         * @brief Returns the number of CPUs available on the system.
         *
         * @return int Number of CPUs detected.
         */
        int getCpuCount() {
            return CoreUtils::getSystemInfo().getCpuCount();
        }

        /**
         * @brief Retrieves the system load average.
         *
         * @return LoadAverage The current system load average statistics.
         */
        LoadAverage getLoadAverage() {
            return CoreUtils::getSystemInfo().getLoadAverage();
        }

        /**
         * @brief Retrieves detailed information about the system's CPU.
         *
         * @return CpuInfo Structure containing CPU details such as model, speed, and core information.
         */
        CpuInfo getCpuInfo() {
            return CoreUtils::getSystemInfo().getCpuInfo();
        }
    }

}