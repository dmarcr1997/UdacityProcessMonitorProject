#include <string>

#include "format.h"

using std::string;

string Format::ElapsedTime(long seconds) {   
  long const secondsInHour = 3600;
  long const secondsInMinutes = 60;
  
  long hours, minutes;
  hours = seconds/secondsInHour;
  seconds = seconds%secondsInHour;
  minutes = seconds/secondsInMinutes;
  seconds = seconds%secondsInMinutes;
  return (std::to_string(hours) + ":" + std::to_string(minutes) + ":" + std::to_string(seconds));
}