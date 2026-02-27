#ifndef __TOP
#define __TOP
#include "Alphabet.h"
#include <vector>
class Top
{
   public:
      Top(int max_top)
            : max_top_(max_top), min_top_score_(-1.0), max_top_score_(-1.0)
      {
         score_.resize(max_top_);
         top_.resize(max_top_);
         clear();
      }
      Top& operator=(const Top& top)
      {
         score_.resize(top.score_.size());
         top_.resize(top.score_.size());
         for (unsigned int i = 0; i < top.score_.size(); i++)
         {
            score_[i] = top.score_[i];
            top_[i] = top.top_[i];
         }
         max_top_ = top.max_top_;
         min_top_score_ = score_[max_top_ - 1];
         max_top_score_ = score_[0];
         return *this;
      }
      void clear()
      {
         for (unsigned int i = 0; i < score_.size(); i++) score_[i] = -1;
         min_top_score_ = -1.0;
         max_top_score_ = -1.0;
      }
      int operator==(const Top& top)
      {
         if (max_top_ != top.max_top_ ||
               min_top_score_ != top.min_top_score_ ||
               max_top_score_ != top.max_top_score_) return 0;
         for (unsigned int i = 0; i < top.score_.size(); i++)
         {
            if (score_[i] != top.score_[i]) return 0;
         }
         return 1;
      }
      int qualifies(int score)
      {
         if (score > min_top_score_) return 1;
         return 0;
      }
      void add(const Alphabet& top, int score)
      {
         int i = 0;
         while (i < (int)top_.size() && score_[i] >= score) i++;
         if (i > 0 && score == score_[i - 1]) return;
         //if (i > 0 && top == top_[i - 1]) return;
         if (i < (int)top_.size())
         {
            for (int j = max_top_ - 1; j > i; j--)
            {
               top_[j] = top_[j-1];
               score_[j] = score_[j-1];
            }
            top_[i] = top;
            score_[i] = score;
            min_top_score_ = score_[max_top_ - 1];
            max_top_score_ = score_[0];
         }
      }
      const Alphabet& operator[] (int i) const
      {
         return top_[i];
      }
      const int score(int i) const
      {
         return (int)score_[i];
      }
      int size()
      {
         return top_.size();
      }
   private:
      std::vector<Alphabet> top_;
      std::vector<double> score_;
      double min_top_score_;
      double max_top_score_;
      int max_top_;
};
#endif
