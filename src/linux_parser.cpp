#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::stol;
using std::string;
using std::to_string;
using std::vector;


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
  filestream.close();
  return value;
}


string LinuxParser::Kernel() {
  string os;
  string kernel;
  string version;
  string line;

  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }

  stream.close();
  return kernel;
}

// Update this to use std::filesystem
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

//Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  float mem_total;
  float mem_free;

  string line;
  string key;
  string value;

  bool is_total = false, is_free = false;

  std::ifstream filestream(kProcDirectory + kMeminfoFilename);

  if (filestream.is_open()){
    while(std::getline(filestream, line)){
      std::istringstream linestream(line);
      linestream >> key >> value;

      if(key == "MemTotal:") {
        mem_total = stof(value);
        is_total = true;
      }

      if(key == "MemFree:") {
        mem_free = stof(value);
        is_free = true;
      }

      if (is_total && is_free)  return (mem_total - mem_free)/mem_total;
    }
  }

 filestream.close();
 return 0.0;
}

// Read and return the system uptime
long LinuxParser::UpTime() {
  string line;
  string uptime_s;

  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if(filestream.is_open()){
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> uptime_s;
    return stol(uptime_s);
  }

  filestream.close();
  return 0.0;
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  long jiffies = 0;
  vector<string> cpu_utilisation = LinuxParser::CpuUtilization();
  while (!cpu_utilisation.empty()){
    jiffies += std::stof(cpu_utilisation.back());
    cpu_utilisation.pop_back();
  }

  return jiffies;
}

// Read and return the number of active jiffies for a PID
float LinuxParser::ActiveJiffies(int pid) {

  string line;
  string pid_s;
  string value;
  string utime_s;
  string stime_s;
  string cutime_s;
  string cstime_s;

  int i = 1;

  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);

  if(filestream.is_open()){
    std::getline(filestream,line);
    std::istringstream linestream(line);
    while (i<=13){
      linestream >> value;
      i++;
    }
    linestream >> utime_s >> stime_s >> cutime_s >> cstime_s;
    return (stof(utime_s) + stof(stime_s) + stof(cutime_s) + stof(cstime_s));
  }
 filestream.close();
 return 0;
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  return LinuxParser::Jiffies() - LinuxParser::IdleJiffies(); }

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> cpu_utilisation = LinuxParser::CpuUtilization();
  return stol(cpu_utilisation[kIdle_]) + stol(cpu_utilisation[kIOwait_]);
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  vector<string> cpu_utilisation;
  string line;
  string key;
  string value;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> key;
    while(linestream >> value) cpu_utilisation.push_back(value);
  }
  filestream.close();
  return cpu_utilisation;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line;
  string key;
  string value;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while(std::getline(filestream, line)){
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "processes") return stoi(value);
    }
  }

  filestream.close();
  return 0;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line;
  string key;
  string value;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while(std::getline(filestream, line)){
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "procs_running") return stoi(value);
    }
  }

  filestream.close();
  return 0;
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) {

  string line;

  std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);

  if(filestream.is_open()){
    std::getline(filestream,line);
    return line;
  }

  filestream.close();
  return string();
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string line;
  string key;
  string value;

  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);

  if(filestream.is_open()){
    while (std::getline(filestream,line)){
      std::istringstream linestream(line);
      linestream >> key >> value;
      if(key == "VmSize:") return value;
    }
  }

  filestream.close();
  return string();
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string line;
  string key;
  string value;

   std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);

  if(filestream.is_open()){
    while (std::getline(filestream,line)){
      std::istringstream linestream(line);
      linestream >> key >> value;
      if(key == "Uid:") return value;
    }
  }

  filestream.close();
  return string();
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string line;
  string key;
  string value_1;
  string value_2;

  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> key >> value_1 >> value_2;
      if (value_2 == LinuxParser::Uid(pid)) return key;
    }
  }

  filestream.close();
  return string();
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  string line;
  string value;
  string uptime_s;

  int i = 1;


  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);

  if(filestream.is_open()){
    std::getline(filestream,line);
    std::istringstream linestream(line);
    while (i<=21){
      linestream >> value;
      i++;
    }
    linestream >> uptime_s;

    return LinuxParser::UpTime() - stol(uptime_s)/sysconf(_SC_CLK_TCK);
  }

  filestream.close();
  return 0;
}
