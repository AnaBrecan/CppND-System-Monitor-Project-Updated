#include <string>
#include <math.h>

#include "format.h"
#define MIN 60
#define HOUR 3600

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
  long hh, mm, ss;
  string h, m, s;
  string time;

  hh = floor(seconds/HOUR);
  ss = seconds - hh*HOUR;
  mm = floor(ss/MIN);
  ss = ss - mm*MIN;

  if (hh < 10) {
    h = "0" + std::to_string(hh);
  }
  else {
    h = std::to_string(hh);
  }

  if (mm < 10) {
    m = "0" + std::to_string(mm);
  }
  else{
    m = std::to_string(mm);
  }

  if (ss < 10) {
    s = "0" + std::to_string(ss);
  }
  else{
    s = std::to_string(ss);
  }
  
  return h + ":" + m + ":" + s;
}
