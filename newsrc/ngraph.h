#ifndef __NGRAPH_H
#define __NGRAPH_H
#pragma warning(disable:4786)
#include <fstream>
#include <ctype.h>
#include <math.h>
#include <map>
#include "N_Graph.h"
#include "callback.h"

class Ngraph_File_Creator
{
   public:
      Ngraph_File_Creator(const char* sample_file, const char* ngraph_file, int n, int spaces = 0)
            : sample_file_(0), ngraph_file_(0), n_(n), total_(0),
            progress_(0), finished_(0), spaces_(spaces), callback_(0)
      {
         sample_file_ = new char [ strlen(sample_file) + 1 ];
         strcpy(sample_file_, sample_file);
         ngraph_file_ = new char [ strlen(ngraph_file) + 1 ];
         strcpy(ngraph_file_, ngraph_file);
         N_Graph::set_length(n_);
      }
      void go()
      {
         create_ngraph_file();
         finished_ = 1;
         if (callback_)
         {
            callback_->process_progress(*this);
         }
      }
      int is_done() const
      {
         return finished_;
      }
      int get_progress() const
      {
         return progress_;
      }
      void set_callback(Ngraph_File_Creator_Callback* callback)
      {
         callback_ = callback;
      }
      ~Ngraph_File_Creator()
      {
         delete [] sample_file_;
         delete [] ngraph_file_;
      }
   private:
      int n_;
      char* sample_file_;
      char* ngraph_file_;
      std::map<N_Graph, int> freq_table;
      int total_;
      int progress_;
      int finished_;
      int spaces_;
      Ngraph_File_Creator_Callback* callback_;
      void process_ngraphs(char* str)
      {
         char ngraph[MAX_N];
         char* c = str;
         while (*(c + n_))
         {
            memcpy(ngraph, c, n_);
            ngraph[n_] = '\0';
            freq_table[N_Graph(ngraph)]++;
            total_++;
            c++;
         }
      }
      void create_ngraph_file()
      {
         char buf[132];
         char buf1[132];
         int start = 0;
         progress_ = 0;
         int prev_was_space = 0;
         std::fstream file(sample_file_, std::ios::in);
         {
            while (file.getline(buf, 132))
            {
               char* c = buf;
               char* d = buf1 + start;
               while (c && *c)
               {
                  if (isalpha(*c))
                  {
                     *d++ = toupper(*c);
                     prev_was_space = 0;
                  }
                  else if (spaces_ &&
                           (*c == ' ' || *c == '.' || *c == ',' || *c == ';')
                           && !prev_was_space)
                  {
                     *d++ = '{';
                     prev_was_space = 1;
                  }
                  c++;
               }
               *d = '\0';
               int len = strlen(buf1);
               if (len > n_ - 1)
               {
                  process_ngraphs(buf1);
                  strncpy(buf1, buf1 + len - n_ + 1, n_ - 1);
                  start = n_ - 1;
               }
               progress_ += strlen(buf);
               if (progress_ % 1000 == 0)
               {
                  if (callback_)
                  {
                     callback_->process_progress(*this);
                  }
               }
            }
            file.close();
         }
         if (callback_)
         {
            callback_->process_progress(*this);
         }
         std::fstream nfile(ngraph_file_, std::ios::out);
         int x = (int)(-1.0 * log(100.0 / total_)) + 1;
         for (std::map<N_Graph, int>::iterator iter = freq_table.begin();
               iter != freq_table.end();
               ++iter)
         {
            double score = x + log((iter->second * 100.0) / total_);
            nfile << iter->first << " : " << iter->second << " " << score << std::endl;
         }
      }
};

#endif