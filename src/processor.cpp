#include "processor.h"
#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  float active_jiffies = (float) LinuxParser::ActiveJiffies();
  float jiffies = (float) LinuxParser::Jiffies();
  return active_jiffies/jiffies;
}
