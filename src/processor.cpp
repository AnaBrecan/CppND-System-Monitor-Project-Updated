#include "processor.h"
#include "linux_parser.h"

// Return the aggregate CPU utilization
float Processor::Utilization() {
  float active_jiffies = static_cast<float>(LinuxParser::ActiveJiffies());
  float jiffies = static_cast<float>(LinuxParser::Jiffies());
  return active_jiffies/jiffies;
}
