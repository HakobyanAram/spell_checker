#include <iostream>
#include <boost/format.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include "tools.h"
#include "spell_checker.h"

/******************************************************************/

/**
 * first: input filename;
 * second: output-filename.
 */
extern std::pair<std::string, std::string>
parse_command_line(int argc, char **argv);

/******************************************************************/

int main(int argc, char** argv)
{
 if (argc == 1)
 {
  std::ostringstream os;
  os << "please choose input data-file. Terminate.";
  ns_tools::log_message(os.str());

  return 1;
 }

 try
 {
  const std::pair<std::string, std::string>& args = parse_command_line(argc, argv);
  if (args.first.length() == 0) {
   return 0;
  }

  if (!boost::filesystem::exists(args.first))
  {
   std::ostringstream os;
   os << "data-file doesn't exist. Terminate.";
   ns_tools::log_message(os.str());

   return 1;
  }

  spell_checker sc(args.first, args.second);
  sc.run();
 }
 catch (const std::exception& ex)
 {
  const std::string& str = (boost::format("application finished with exception: %1%") % ex.what()).str();
  std::ostringstream os;
  os << __FILE__ << "(" << __LINE__ << "): " << str;
  ns_tools::log_message(os.str());

  return 1;
 }
 catch (...)
 {
  std::ostringstream os;
  os << __FILE__ << ":(" << __LINE__ << ")[unknown exception]" << std::endl;
  ns_tools::log_message(os.str());
 }

 return 0;
}

/******************************************************************/

std::pair<std::string, std::string>
parse_command_line(int argc, char **argv)
{
 std::pair<std::string, std::string> result_pair = std::make_pair("input.txt", "output.txt");

 boost::program_options::options_description desc;
 desc.add_options()
  ("help,h", "spell checker (correct words by finding them in the dictionary)")
  ("input-file,i", boost::program_options::value<std::string>(&result_pair.first), "input filename")
  ("output-file,o", boost::program_options::value<std::string>(&result_pair.second), "output filename");

 boost::program_options::variables_map vars;
 boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(desc).run(), vars);
 boost::program_options::notify(vars);

 if (vars.count("help"))
 {
  std::ostringstream os;
  os << desc;
  ns_tools::log_message(os.str());
  result_pair.first.clear();
 }

 return result_pair;
}