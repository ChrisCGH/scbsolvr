#ifndef __TOP
#define __TOP
#include "Alphabet.h"
#include <vector>
class Top
{
public:
   Top(int max_top) 
      : _max_top(max_top), _min_top_score(-1.0), _max_top_score(-1.0)
    {
      _score.resize(_max_top);
      _top.resize(_max_top);
	  clear();
   }
   Top& operator=(const Top& top)
   {
      _score.resize(top._score.size());
      _top.resize(top._score.size());
      for (unsigned int i = 0; i < top._score.size(); i++)
      {
         _score[i] = top._score[i];
         _top[i] = top._top[i];
      }
      _max_top = top._max_top;
      _min_top_score = _score[_max_top - 1];
      _max_top_score = _score[0];
      return *this;
   }
   void clear()
   {
      for (unsigned int i = 0; i < _score.size(); i++) _score[i] = -1;
      _min_top_score = -1.0;
      _max_top_score = -1.0;
   }
   int operator==(const Top& top)
   {
      if (_max_top != top._max_top ||
         _min_top_score != top._min_top_score ||
         _max_top_score != top._max_top_score) return 0;
      for (unsigned int i = 0; i < top._score.size(); i++)
      {
         if (_score[i] != top._score[i]) return 0;
      }
      return 1;
   }
   int qualifies(int score)
   {
      if (score > _min_top_score) return 1;
      return 0;
   }
   void add(const Alphabet& top, int score)
   {
      int i = 0;
      while (i < (int)_top.size() && _score[i] >= score) i++;
      if (i > 0 && score == _score[i - 1]) return;
      //if (i > 0 && top == _top[i - 1]) return;
      if (i < (int)_top.size())
      {
         for (int j = _max_top - 1; j > i; j--)
         {
            _top[j] = _top[j-1];
            _score[j] = _score[j-1];
         }
         _top[i] = top;
         _score[i] = score;
         _min_top_score = _score[_max_top - 1];
         _max_top_score = _score[0];
      }   
   }
   const Alphabet& operator[] (int i) const { return _top[i]; }
   const int score(int i) const { return (int)_score[i]; }
   int size() { return _top.size(); }
private:
   std::vector<Alphabet> _top;
   std::vector<double> _score;
   double _min_top_score;
   double _max_top_score;
   int _max_top;
};
#endif
