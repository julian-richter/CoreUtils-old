#pragma once

#include <string>
#include <vector>

namespace CoreUtils {

  struct CpuInfo {
    std::string model_name;
    int cores;
    int threads;
    double frequency_mhz;
  };

  struct LoadAverage {
    double one_min;
    double five_min;
    double fifteen_min;
  };

  class SystemInfo {
  public:
    SystemInfo();
    ~SystemInfo() = default;

    // Basic system info
    std::string getHostname();
    std::string getKernelVersion();
    std::string getDistribution();
    long getUptime(); // seconds since boot

    // CPU related
    CpuInfo getCpuInfo();
    int getCpuCount();
    LoadAverage getLoadAverage();

    // System load and stats
    int getRunningProcesses();
    int getTotalProcesses();

    // Refresh data from system files
    void refresh();

  private:
    // Helper methods for reading system files
    std::string readFile(const std::string& path);
    std::vector<std::string> readLines(const std::string& path);
    std::string parseCpuInfo();
    void parseLoadAvg();
    void parseUptime();
    void parseStat();

    // Cached data
    CpuInfo m_cpu_info;
    LoadAverage m_load_avg;
    long m_uptime;
    int m_running_processes;
    int m_total_processes;
    std::string m_hostname;
    std::string m_kernel_version;

    // Helper flags
    bool m_cpu_info_loaded;
    bool m_load_avg_loaded;
    bool m_uptime_loaded;
    bool m_stat_loaded;
  };
}