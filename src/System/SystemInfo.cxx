#include "System/SystemInfo.hxx"
#include <fstream>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include <sys/utsname.h>

namespace CoreUtils {

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

std::string SystemInfo::readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::vector<std::string> SystemInfo::readLines(const std::string& path) {
    std::vector<std::string> lines;
    std::ifstream file(path);
    std::string line;

    while (std::getline(file, line)) {
        lines.push_back(line);
    }

    return lines;
}

std::string SystemInfo::getHostname() {
    return m_hostname;
}

std::string SystemInfo::getKernelVersion() {
    return m_kernel_version;
}

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

long SystemInfo::getUptime() {
    if (!m_uptime_loaded) {
        parseUptime();
    }
    return m_uptime;
}

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

CpuInfo SystemInfo::getCpuInfo() {
    if (!m_cpu_info_loaded) {
        parseCpuInfo();
    }
    return m_cpu_info;
}

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

int SystemInfo::getCpuCount() {
    if (!m_cpu_info_loaded) {
        parseCpuInfo();
    }
    return m_cpu_info.threads;
}

LoadAverage SystemInfo::getLoadAverage() {
    if (!m_load_avg_loaded) {
        parseLoadAvg();
    }
    return m_load_avg;
}

void SystemInfo::parseLoadAvg() {
    std::string content = readFile("/proc/loadavg");
    if (!content.empty()) {
        std::istringstream iss(content);
        iss >> m_load_avg.one_min >> m_load_avg.five_min >> m_load_avg.fifteen_min;
        m_load_avg_loaded = true;
    }
}

int SystemInfo::getRunningProcesses() {
    if (!m_stat_loaded) {
        parseStat();
    }
    return m_running_processes;
}

int SystemInfo::getTotalProcesses() {
    if (!m_stat_loaded) {
        parseStat();
    }
    return m_total_processes;
}

/**
 * Parses the system statistics from the /proc/stat file.
 *
 * This method reads and processes the contents of the /proc/stat
 * file to extract relevant system statistics, such as the number of
 * running processes and the total number of processes. The extracted
 * information is cached in the corresponding member variables for
 * later retrieval.
 *
 * Once this method is called, the system statistics are marked as
 * loaded, and subsequent calls to retrieve process information will
 * use the cached values unless the data is explicitly refreshed.
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
 * Resets all cached system information flags to their unloaded state.
 *
 * This method clears the internal flags used to indicate whether
 * specific pieces of system information (CPU info, load average, uptime,
 * and process statistics) have been loaded. After this method is called,
 * these data will need to be reloaded when requested.
 *
 * Use this method to force the system info object to refresh its state
 * by reloading system data from the relevant sources.
 */
void SystemInfo::refresh() {
    m_cpu_info_loaded = false;
    m_load_avg_loaded = false;
    m_uptime_loaded = false;
    m_stat_loaded = false;
}

}