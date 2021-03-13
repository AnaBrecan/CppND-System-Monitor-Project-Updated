#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  float MemTotal, MemFree;
  string line, key, value;
  bool is_Total = false, is_Free = false;

  std::ifstream filestream(kProcDirectory + kMeminfoFilename);

  if (filestream.is_open()){
    while(std::getline(filestream, line)){
      std::istringstream linestream(line);
      linestream >> key >> value;

      if(key == "MemTotal:") {
        MemTotal = std::stof(value);
        is_Total = true;
      }

      if(key == "MemFree:") {
        MemFree = std::stof(value);
        is_Free = true;
      }

      if (is_Total && is_Free)  return (MemTotal - MemFree)/MemTotal;
    }
  }
 return 0.0;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  string line, uptime_s;

  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if(filestream.is_open()){
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> uptime_s;
    return std::stol(uptime_s);
  }

  return 0.0;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  long jiffies = 0;
  vector<string> cpu_utilisation = LinuxParser::CpuUtilization();
  while (!cpu_utilisation.empty()){
    jiffies += std::stof(cpu_utilisation.back());
    cpu_utilisation.pop_back();
  }
  return jiffies;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
float LinuxParser::ActiveJiffies(int pid) {

  string line, pid_s, value, utime_s, stime_s, cutime_s, cstime_s;
  pid_s = to_string(pid);
  int i = 1;

  std::ifstream filestream(kProcDirectory + pid_s + kStatFilename);

  if(filestream.is_open()){
    std::getline(filestream,line);
    std::istringstream linestream(line);
    while (i<=13){
      linestream >> value;
      i++;
    }
    linestream >> utime_s >> stime_s >> cutime_s >> cstime_s;
    return (std::stof(utime_s) + std::stof(stime_s) + std::stof(cutime_s) + std::stof(cstime_s));
    }
 return 0;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  return LinuxParser::Jiffies() - LinuxParser::IdleJiffies(); }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> cpu_utilisation = LinuxParser::CpuUtilization();
  return std::stol(cpu_utilisation[kIdle_]) + std::stol(cpu_utilisation[kIOwait_]);
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  vector<string> cpu_utilisation;
  string line, key, value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> key;
    while(linestream >> value) cpu_utilisation.push_back(value);
  }

  return cpu_utilisation;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line, key, value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while(std::getline(filestream, line)){
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "processes") return stoi(value);
    }
  }
  return 0;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line, key, value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while(std::getline(filestream, line)){
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "procs_running") return stoi(value);
    }
  }
  return 0;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {

  string line, pid_s;
  pid_s = to_string(pid);

  std::ifstream filestream(kProcDirectory + pid_s + kCmdlineFilename);

  if(filestream.is_open()){
    std::getline(filestream,line);
    return line;
  }

  return string();
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  string line, key, value, pid_s;
  pid_s = to_string(pid);

  std::ifstream filestream(kProcDirectory + pid_s + kStatusFilename);

  if(filestream.is_open()){
    while (std::getline(filestream,line)){
      std::istringstream linestream(line);
      linestream >> key >> value;
      if(key == "VmSize:") return value;
    }
  }

  return string();
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  string line, key, value, pid_s;
  pid_s = to_string(pid);

  std::ifstream filestream(kProcDirectory + pid_s + kStatusFilename);

  if(filestream.is_open()){
    while (std::getline(filestream,line)){
      std::istringstream linestream(line);
      linestream >> key >> value;
      if(key == "Uid:") return value;
    }
  }
  return string();
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string line, key, value_1, value_2;
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> key >> value_1 >> value_2;
      if (value_2 == LinuxParser::Uid(pid)) return key;
    }
  }

  return string();
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
   string line, pid_s, value, uptime_s;
   pid_s = to_string(pid);
   int i = 1;

   std::ifstream filestream(kProcDirectory + pid_s + kStatFilename);

   if(filestream.is_open()){
     std::getline(filestream,line);
     std::istringstream linestream(line);
     while (i<=21){
       linestream >> value;
       i++;
     }
     linestream >> uptime_s;

     return LinuxParser::UpTime() - std::stol(uptime_s)/sysconf(_SC_CLK_TCK);
   }

   return 0;
}
