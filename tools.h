#pragma once

#include <fstream>
#include <chrono>
#include <cstring>
#include <numeric>

namespace ns_tools
{
 ////////////////////////////////////////////////

 inline size_t split_str(const std::string& txt, std::vector<std::string>& strs, char ch)
 {
  size_t pos = txt.find(ch);
  size_t init_pos = 0;

  // decompose statement
  while (pos != std::string::npos)
  {
   strs.push_back(txt.substr(init_pos, pos - init_pos));
   init_pos = pos + 1;

   pos = txt.find(ch, init_pos);
  }

  // add the last one
  strs.push_back(txt.substr(init_pos, std::min(pos, txt.size()) - init_pos + 1));

  return strs.size();
 }

 ////////////////////////////////////////////////

 inline std::string get_current_time_str()
 {
  std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
  std::chrono::system_clock::duration tp = now.time_since_epoch();

  tp -= std::chrono::duration_cast<std::chrono::seconds>(tp);

  std::time_t tt = std::chrono::system_clock::to_time_t(now);
  std::tm t = *std::gmtime(&tt);

  // result string example: 2018-06-10 18:07:44.457
  char date[64];
  sprintf(date, "%04u-%02u-%02u %02u:%02u:%02u.%03u"
                , (t.tm_year + 1900)
                , t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec
                , (static_cast<unsigned>(tp / std::chrono::milliseconds(1))));
  return std::string(date);
 }

 ////////////////////////////////////////////////

 inline void log_message(const std::string& message)
 {
  std::ofstream ofs("logs.txt", std::ofstream::app);
  ofs << "[" << ns_tools::get_current_time_str() << "] " << message << std::endl;
  ofs.close();
 }

} // ns_tools
