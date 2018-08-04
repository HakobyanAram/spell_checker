#pragma once

#include <string>
#include <vector>
#include <boost/noncopyable.hpp>
#include <unordered_map>
#include <set>

struct word_data
{
 std::string _word;

 // we need sorted items for using intersections
 std::set<std::string> _projections1;
 std::set<std::string> _projections2;

 std::vector<std::string> _corrections;

 bool _is_last = false; // last word in line
};

class spell_checker: boost::noncopyable
{
public:
 spell_checker(const std::string& ifname, const std::string& ofname);
 void run();

private:
 void set_word_projections(std::vector<word_data>& words);
 void set_word_corrections();

 void parse_data();
 void write_new_data();
 
 std::string _ifname;
 std::string _ofname;

 std::vector<word_data> _misspelled_words;
 std::vector<word_data> _dictionary;

}; // spell_checker



