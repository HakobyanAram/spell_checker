#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

#include "spell_checker.h"
#include "tools.h"

spell_checker::spell_checker(const std::string& ifname, const std::string& ofname) : _ifname(ifname), _ofname(ofname) { }

void spell_checker::run()
{
   parse_data();

   set_word_projections(_misspelled_words);
   set_word_projections(_dictionary);

   set_word_corrections();

   write_new_data();
}

void spell_checker::set_word_projections(std::vector<word_data>& words)
{
   for (size_t i = 0; i != words.size(); ++i)
   {
      word_data& item = words[i];

      // fill by variants with chopped words (combinations with deducted 1 and 2 symbols)
      for (size_t j = 0; j < item._word.length(); ++j)
      {
         std::string word = item._word;
         item._projections1.insert(word.erase(j, 1));

         for (size_t k = j + 1; k < item._word.length() - 1; ++k)
         {
            std::string word2 = word;
            item._projections2.insert(word2.erase(k, 1));
         }
      }
   }
}

void spell_checker::set_word_corrections()
{
   for (word_data& item_word: _dictionary)
   {
      int level = 2;

      for (word_data& item_dict_word: _misspelled_words)
      {
         if (item_dict_word._word == item_word._word)
         {
            item_word._corrections.clear();
            item_word._corrections.push_back(item_dict_word._word);
            level = 0;
            continue;
         }

         if (level > 0)
         {
            std::vector<std::string> intersection;

            std::vector<std::string> self_v;
            self_v.push_back(item_dict_word._word);

            std::set_intersection(item_word._projections1.begin(), item_word._projections1.end(), self_v.begin(),
                                  self_v.end(), std::back_inserter(intersection));

            if (intersection.size() > 0)
            {
               item_word._corrections.clear();
               item_word._corrections.push_back(item_dict_word._word);
               level = 1;
               continue;
            }

            intersection.clear();

            self_v.clear();
            self_v.push_back(item_word._word);

            std::set_intersection(item_dict_word._projections1.begin(), item_dict_word._projections1.end(),
                                  self_v.begin(), self_v.end(), std::back_inserter(intersection));

            if (intersection.size() > 0)
            {
               if (level != 1) {
                  item_word._corrections.clear();
               }

               item_word._corrections.push_back(item_dict_word._word);
               level = 1;
               continue;
            }
         }

         if (level > 1)
         {
            std::vector<std::string> intersection;

            std::vector<std::string> self_v;
            self_v.push_back(item_dict_word._word);

            std::set_intersection(item_word._projections2.begin(), item_word._projections2.end(), self_v.begin(),
                                  self_v.end(), std::back_inserter(intersection));

            if (intersection.size() > 0)
            {
               item_word._corrections.push_back(item_dict_word._word);
               continue;
            }

            intersection.clear();
            self_v.clear();

            self_v.push_back(item_word._word);

            std::set_intersection(item_dict_word._projections2.begin(), item_dict_word._projections2.end(),
                                  self_v.begin(), self_v.end(), std::back_inserter(intersection));

            if (intersection.size() > 0)
            {
               item_word._corrections.push_back(item_dict_word._word);
               continue;
            }
         }

         if (level > 1)
         {
            std::vector<std::string> intersection;

            std::set_intersection(item_dict_word._projections1.begin(), item_dict_word._projections1.end(),
                                  item_word._projections1.begin(), item_word._projections1.end(),
                                  std::back_inserter(intersection));

            if (intersection.size() > 0)
            {
               item_word._corrections.push_back(item_dict_word._word);
               continue;
            }
         }
      }
   }
}

void spell_checker::parse_data()
{
   std::ifstream ifs;
   ifs.open(_ifname, std::ifstream::out);

   if (!ifs.good())
   {
      std::ostringstream os;
      os << "can't open input data-file. Terminate.";
      ns_tools::log_message(os.str());

      return;
   }

   std::string line;
   bool miss_finished = false; // misspelled words already readed
   bool dict_started = false; // dictionary reading has been started

   while (std::getline(ifs, line))
   {
      if (line.empty())
      { continue; }

      if ((line.length() == 3) && (line == "==="))
      {
         miss_finished = true;
         dict_started = !dict_started;

         continue;
      }

      // filling misspelled words while we can
      if (!miss_finished)
      {
         std::vector<std::string> miss_words;
         ns_tools::split_str(line, miss_words, ' ');
         std::for_each(miss_words.begin(), miss_words.end(), [&](const std::string& item)
         {
            word_data w;
            w._word = item;
            _misspelled_words.push_back(std::move(w));
         });
      }
         // filling dictionary words
      else if (dict_started)
      {
         std::vector<std::string> words;
         ns_tools::split_str(line, words, ' ');

         if (words.size() > 0)
         {
            size_t widx = words.size() - 1;
            size_t i = 0;
            for (const std::string& item: words)
            {
               word_data w;
               w._word = item;
               w._is_last = (i == widx);
               _dictionary.push_back(std::move(w));

               ++i;
            }
         }
      }

      if (!dict_started && miss_finished)
      { break; }
   }
   ifs.close();
}

void spell_checker::write_new_data()
{
   std::ofstream ofs;
   ofs.open(_ofname, std::ifstream::trunc);

   if (!ofs.good())
   {
      std::ostringstream os;
      os << "can't open output data-file. Terminate.";
      ns_tools::log_message(os.str());

      return;
   }

   size_t i = 0;
   for (const word_data& w_item: _dictionary)
   {
      const size_t& c_size = w_item._corrections.size();
      std::ostringstream os;

      if (c_size == 0) {
         os << "{" << w_item._word << "?}";
      }
      else if (c_size == 1) {
         os << *w_item._corrections.begin();
      }
         // if more than one
      else
      {
         std::ostringstream os_items;
         std::for_each(w_item._corrections.begin(), w_item._corrections.end(),
                       [&w_item, &os_items](const std::string& item)
                       {
                          os_items << item;
                          if (item != w_item._corrections.back()) {
                             os_items << " ";
                          }
                       });

         os << "{" << os_items.str() << "}";
      }

      if (i != (_dictionary.size() - 1)) {
         os << (!w_item._is_last ? " " : "\n");
      }

      ofs << os.str();
      ++i;
   }

   ofs.close();
}

