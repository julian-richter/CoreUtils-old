#include "System/SystemInfo.hxx"
#include <fstream>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include <sys/utsname.h>

namespace CoreUtils {

/**
 * @brief Constructs a SystemInfo object and initializes cached system data.
 *
 * Initializes member variables and loads the system hostname and kernel version using the uname system call. Other system information is set to default values and will be loaded on demand.
 */
SystemInfo::SystemInfo()
    : m_uptime(0)
    , m_running_processes(0)
    , m_total_processes(0)
    , m_cpu_info_loaded(false)
    , m_load_avg_loaded(false)
    , m_uptime_loaded(false)
    , m_stat_loaded(false) {

    // Load hostname and kernel version once
    struct utsname uts;
    if (uname(&uts) == 0) {
        m_hostname = uts.nodename;
        m_kernel_version = uts.release;
    }
}

/**
 * @brief Reads the entire contents of a file into a string.
 *
 * @param path Path to the file to be read.
 * @return std::string Contents of the file, or an empty string if the file cannot be opened.
 */
std::string SystemInfo::readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

/**
 * @brief Reads all lines from a file into a vector of strings.
 *
 * @param path Path to the file to be read.
 * @return std::vector<std::string> Vector containing each line of the file. Returns an empty vector if the file cannot be opened.
 */
std::vector<std::string> SystemInfo::readLines(const std::string& path) {
    std::vector<std::string> lines;
    std::ifstream file(path);
    std::string line;

    while (std::getline(file, line)) {
        lines.push_back(line);
    }

    return lines;
}

/**
 * @brief Returns the system hostname.
 *
 * The hostname is retrieved and cached during object construction.
 *
 * @return std::string The system's hostname.
 */
std::string SystemInfo::getHostname() {
    return m_hostname;
}

/**
 * @brief Returns the cached kernel version of the system.
 *
 * The kernel version is retrieved during object construction and remains constant until the object is refreshed.
 *
 * @return std::string The system's kernel version.
 */
std::string SystemInfo::getKernelVersion() {
    return m_kernel_version;
}

/**
 * @brief Retrieves the Linux distribution name.
 *
 * Attempts to extract the distribution's pretty name from `/etc/os-release`. If unavailable, falls back to the first line of `/etc/issue`. Returns "Unknown" if neither file provides the information.
 *
 * @return std::string The detected distribution name, or "Unknown" if not found.
 */
std::string SystemInfo::getDistribution() {
    // Try to read from /etc/os-release first
    std::string content = readFile("/etc/os-release");
    if (!content.empty()) {
        std::istringstream iss(content);
        std::string line;
        while (std::getline(iss, line)) {
            if (line.substr(0, 11) == "PRETTY_NAME") {
                size_t pos = line.find('=');
                if (pos != std::string::npos) {
                    std::string name = line.substr(pos + 1);
                    // Remove quotes
                    if (name.front() == '"' && name.back() == '"') {
                        name = name.substr(1, name.length() - 2);
                    }
                    return name;
                }
            }
        }
    }

    // Fallback to /etc/issue
    content = readFile("/etc/issue");
    if (!content.empty()) {
        return content.substr(0, content.find('\n'));
    }

    return "Unknown";
}

/**
 * @brief Returns the system uptime in seconds.
 *
 * Retrieves the cached uptime value, loading and parsing it from `/proc/uptime` if not already cached.
 * @return System uptime in seconds.
 */
long SystemInfo::getUptime() {
    if (!m_uptime_loaded) {
        parseUptime();
    }
    return m_uptime;
}

/**
 * @brief Reads and caches the system uptime in seconds from /proc/uptime.
 *
 * Updates the cached uptime value and marks it as loaded if the file is successfully read.
 */
void SystemInfo::parseUptime() {
    std::string content = readFile("/proc/uptime");
    if (!content.empty()) {
        std::istringstream iss(content);
        double uptime_seconds;
        iss >> uptime_seconds;
        m_uptime = static_cast<long>(uptime_seconds);
        m_uptime_loaded = true;
    }
}

/**
 * @brief Retrieves detailed CPU information for the system.
 *
 * Returns cached CPU details, including model name, frequency, core count, and thread count. Loads and caches the information from `/proc/cpuinfo` if not already available.
 *
 * @return CpuInfo Structure containing CPU model, frequency, number of cores, and threads.
 */
CpuInfo SystemInfo::getCpuInfo() {
    if (!m_cpu_info_loaded) {
        parseCpuInfo();
    }
    return m_cpu_info;
}

/**
 * @brief Parses and caches CPU information from /proc/cpuinfo.
 *
 * Extracts the CPU model name, frequency in MHz, number of cores, and number of threads from the system file. Updates the internal cache and marks CPU info as loaded.
 * 
 * @return An empty string.
 */
std::string SystemInfo::parseCpuInfo() {
    auto lines = readLines("/proc/cpuinfo");
    int cores = 0;
    int threads = 0;

    for (const auto& line : lines) {
        if (line.substr(0, 10) == "model name") {
            size_t pos = line.find(':');
            if (pos != std::string::npos && m_cpu_info.model_name.empty()) {
                m_cpu_info.model_name = line.substr(pos + 2); // +2 to skip ': '
            }
        } else if (line.substr(0, 8) == "cpu MHz") {
            size_t pos = line.find(':');
            if (pos != std::string::npos && m_cpu_info.frequency_mhz == 0.0) {
                m_cpu_info.frequency_mhz = std::stod(line.substr(pos + 2));
            }
        } else if (line.substr(0, 9) == "processor") {
            threads++;
        } else if (line.substr(0, 8) == "cpu cores") {
            size_t pos = line.find(':');
            if (pos != std::string::npos && cores == 0) {
                cores = std::stoi(line.substr(pos + 2));
            }
        }
    }

    m_cpu_info.cores = cores > 0 ? cores : threads;
    m_cpu_info.threads = threads;
    m_cpu_info_loaded = true;

    return "";
}

/**
 * @brief Returns the number of CPU threads available on the system.
 *
 * Retrieves and caches CPU information if not already loaded.
 * @return int Number of CPU threads.
 */
int SystemInfo::getCpuCount() {
    if (!m_cpu_info_loaded) {
        parseCpuInfo();
    }
    return m_cpu_info.threads;
}

/**
 * @brief Returns the system load averages for 1, 5, and 15 minutes.
 *
 * Loads and caches the load averages from the system if not already loaded.
 *
 * @return LoadAverage Structure containing the 1, 5, and 15-minute load averages.
 */
LoadAverage SystemInfo::getLoadAverage() {
    if (!m_load_avg_loaded) {
        parseLoadAvg();
    }
    return m_load_avg;
}

/**
 * @brief Parses and caches the system load averages from /proc/loadavg.
 *
 * Extracts the 1, 5, and 15-minute load averages and stores them in the internal cache.
 * Marks the load average data as loaded.
 */
void SystemInfo::parseLoadAvg() {
    std::string content = readFile("/proc/loadavg");
    if (!content.empty()) {
        std::istringstream iss(content);
        iss >> m_load_avg.one_min >> m_load_avg.five_min >> m_load_avg.fifteen_min;
        m_load_avg_loaded = true;
    }
}

/**
 * @brief Returns the number of currently running processes on the system.
 *
 * Retrieves and caches the count of running processes from system statistics if not already loaded.
 * @return int Number of running processes.
 */
int SystemInfo::getRunningProcesses() {
    if (!m_stat_loaded) {
        parseStat();
    }
    return m_running_processes;
}

/**
 * @brief Returns the total number of processes created since system boot.
 *
 * If the value is not already cached, it reads and parses the relevant system statistics to obtain the count.
 *
 * @return int Total number of processes since boot.
 */
int SystemInfo::getTotalProcesses() {
    if (!m_stat_loaded) {
        parseStat();
    }
    return m_total_processes;
}

/**
 * @brief Parses and caches running and total process counts from /proc/stat.
 *
 * Reads the /proc/stat file to extract the number of currently running processes and the total number of processes since boot, storing the results for later access.
 */
void SystemInfo::parseStat() {
    auto lines = readLines("/proc/stat");
    for (const auto& line : lines) {
        if (line.substr(0, 13) == "procs_running") {
            size_t pos = line.find(' ');
            if (pos != std::string::npos) {
                m_running_processes = std::stoi(line.substr(pos + 1));
            }
        } else if (line.substr(0, 9) == "processes") {
            size_t pos = line.find(' ');
            if (pos != std::string::npos) {
                m_total_processes = std::stoi(line.substr(pos + 1));
            }
        }
    }
    m_stat_loaded = true;
}

/**
 * @brief Invalidates all cached system information, forcing data to be reloaded on next access.
 *
 * Call this method to ensure that subsequent queries return up-to-date system information by clearing all internal cache flags.
 */
void SystemInfo::refresh() {
    m_cpu_info_loaded = false;
    m_load_avg_loaded = false;
    m_uptime_loaded = false;
    m_stat_loaded = false;
}

}