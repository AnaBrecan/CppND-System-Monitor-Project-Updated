#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function

string Format::ElapsedTime(long seconds) {
    long hh, mm, ss;
    string time;
    hh = floor(seconds/HOUR);
    ss = seconds - hh*HOUR;
    mm = floor(ss/MIN);
    ss = ss - mm*MIN;
    time = std::to_string(hh) + ":" + std::to_string(mm) + ":" + std::to_string(ss);

    return time; 
}
