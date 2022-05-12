#include "processor.h"
#include "linux_parser.h"
#include <vector>
// TODO: Return the aggregate CPU utilization


float Processor::Utilization() { 
  enum cpuIndexValue {
	user = 0,
  	nice = 1,
  	system = 2,
  	idle = 3,
 	iowait = 4,
  	irq = 5,
  	softirq = 6,
  	steal = 7
	};
  std::vector<std::string> cpuUtils = LinuxParser::CpuUtilization();
  float idleCpus = std::stof(cpuUtils[cpuIndexValue::idle]) + std::stof(cpuUtils[cpuIndexValue::iowait]);
  float nonIdle = std::stof(cpuUtils[cpuIndexValue::user]) + std::stof(cpuUtils[cpuIndexValue::nice]) + std::stof(cpuUtils[cpuIndexValue::system]) + std::stof(cpuUtils[cpuIndexValue::irq]) + std::stof(cpuUtils[cpuIndexValue::softirq]) + std::stof(cpuUtils[cpuIndexValue::steal]);
  float total = idleCpus + nonIdle;
  return (total - idleCpus)/total;
}