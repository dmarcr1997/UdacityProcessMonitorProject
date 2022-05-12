#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::stoi;
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
  return value;
}

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


float LinuxParser::MemoryUtilization() { 
  float totalUtilization;
  float freeResources;
  string line, key, memory;
  std::ifstream fileStream(kProcDirectory + kMeminfoFilename);
  if (fileStream.is_open()) {
    while(std::getline(fileStream, line)){
      std::istringstream linestream(line);
      while(linestream >> key >> memory){
        if(key == "MemTotal:"){
          totalUtilization = stof(memory);
        } 
        else if(key == "MemFree:"){
          freeResources = stof(memory);
        }
      }
    }
  }
  return (totalUtilization-freeResources)/totalUtilization; 
}


long LinuxParser::UpTime() { 
  long upTime = 20.223;
  string line, value;
  std::ifstream fileStream(kProcDirectory + kUptimeFilename);
  if (fileStream.is_open()) {
    std::getline(fileStream, line);
    std::istringstream linestream(line);
  	while(linestream >> value){
     	upTime = std::stold(value);
    }
  }
  return upTime;
}

vector<string> LinuxParser::CpuUtilization() { 
  vector<string> cpus;
  string line, key, value;
  std::ifstream fileStream(kProcDirectory + kStatFilename);
  if (fileStream.is_open()) {
    while(std::getline(fileStream, line)){
      std::istringstream linestream(line);
      while(linestream >> key >> value){
          if(key.find("cpu"))
              cpus.emplace_back(value);
      }
    }
  }
  return cpus;
}

float LinuxParser::CpuUtilization(int pid) { 
  float upTime = (float)LinuxParser::UpTime();
  float hertz = (float)sysconf(_SC_CLK_TCK);
  float totalTime, seconds, cpu_usage;
  string line, key, value;
  vector<string> processorStats;
  
  std::ifstream fileStream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (fileStream.is_open()) {
    std::getline(fileStream, line);
    std::istringstream linestream(line);
    while(linestream >> value){
      processorStats.emplace_back(value);
    }
  }
  
  totalTime = stof(processorStats[CPUStates::utime]) + stof(processorStats[CPUStates::stime]) + stof(processorStats[CPUStates::cutime]) + stof(processorStats[CPUStates::cstime]);
  seconds = upTime - (stof(processorStats[CPUStates::starttime]) / hertz);
  cpu_usage = 100 * ((totalTime / hertz)/seconds);
  return cpu_usage;
}

int LinuxParser::TotalProcesses() { 
  int totalProcesses = 10;
  string line, key, value;
  std::ifstream fileStream(kProcDirectory + kStatFilename);
  if (fileStream.is_open()) {
    while(std::getline(fileStream, line)){
      std::istringstream linestream(line);
      while(linestream >> key >> value){
          if(key == "processes")
              totalProcesses = stoi(value);
      }
    }
  }
  return totalProcesses;
}

int LinuxParser::RunningProcesses() { 
  int runningProcesses = 1;
  string line, key, value;
  std::ifstream fileStream(kProcDirectory + kStatFilename);
  if (fileStream.is_open()) {
    while(std::getline(fileStream, line)){
      std::istringstream linestream(line);
      while(linestream >> key >> value){
          if(key == "procs_running")
              runningProcesses = stoi(value);
      }
    }
  }
  return runningProcesses;
}

string LinuxParser::Command(int pid) {
  string line;
  string command = "";
  std::ifstream fileStream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (fileStream.is_open()) {
    std::getline(fileStream, line);
    if(line != "")
      command = line;
  }
  return command;
}

string LinuxParser::Ram(int pid) { 
  string line, key, value;
  string ram = "";
  std::ifstream fileStream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (fileStream.is_open()) {
    while(std::getline(fileStream, line)){
      std::istringstream linestream(line);
      while(linestream >> key >> value){
        if(key == "VmSize:")
          ram = value;
      }
    }
  }
  return ram;
}

string LinuxParser::Uid(int pid) { 
  string line, key, value;
  string uid = "";
  std::ifstream fileStream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (fileStream.is_open()) {
    while(std::getline(fileStream, line)){
      std::istringstream linestream(line);
      while(linestream >> key >> value){
        if(key == "Uid:")
          uid = value;
      }
    }
  }
  return uid;
}

string LinuxParser::User(int pid) { 
  string line, user, letter, value;
  string username = "";
  string uid = LinuxParser::Uid(pid);
  std::ifstream fileStream(kPasswordPath);
  if (fileStream.is_open()) {
    while(std::getline(fileStream, line)){
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
  	  
      linestream >> user >> letter >> value;
      if(value == uid)
        username = user;
    }
  }
  return username; 
}

long LinuxParser::UpTime(int pid) { 
  string line, value;
  long upTimeValue = 0;
  std::ifstream fileStream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (fileStream.is_open()) {
    while(std::getline(fileStream, line)){
      std::istringstream linestream(line);
      for(int i = 0; i <22; i++){
          linestream >> value;
      }
      upTimeValue = std::stold(value);
    }
  }
  return upTimeValue;
}
