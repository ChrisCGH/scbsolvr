#ifndef __ALPHABET_H
#define __ALPHABET_H

#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <vector>
#include <iostream>
#include "fixed.h"

class Alphabet
{
   public:
      friend class Alphabet_Generator;
      Alphabet(const char* str= "abcdefghijklmnopqrstuvwxyz")
      {
         strcpy(_alphabet, str);
       _alphabet_size = strlen(_alphabet);
      }
      Alphabet(const Alphabet& alp)
      {
         strcpy(_alphabet, alp._alphabet);
       _alphabet_size = alp._alphabet_size;
      }
      Alphabet& operator=(const Alphabet& alp)
      {
         if (this == &alp) return *this;
         strcpy(_alphabet, alp._alphabet);
       _alphabet_size = alp._alphabet_size;
         return *this;
      }

      int operator==(const Alphabet& alp) const
      {
         return (strcmp(_alphabet, alp._alphabet) == 0);
      }
      int operator!=(const Alphabet& alp) const
      {
         return (strcmp(_alphabet, alp._alphabet) != 0);
      }
      int operator<(const Alphabet& alp) const 
      {
         return 1;
      }
 
      void display() const
      {
         const char* c = _alphabet;
         while (c && *c)
         {
			 std::cout << *c << " ";
            c++;
         }
		 std::cout << std::endl;
      }

	  void randomise1(Fixed_Key* fixed_key = 0, int stuck_count = 1)
	  {
		 if (stuck_count < 1) stuck_count = 1;
		 if (stuck_count > 24) stuck_count = 24;
		 int MAX_SWAPS = rand() % stuck_count + 3;
		 for (int number_of_swaps = 0; number_of_swaps < MAX_SWAPS; number_of_swaps++)
		 {
			 int i = rand() % _alphabet_size;
			 int j = rand() % _alphabet_size;
			 while (i == j ||
				    (fixed_key && 
					 (fixed_key->is_set(_alphabet[i]) ||
                      fixed_key->is_set(_alphabet[j]))))
			 {
   			    i = rand() % _alphabet_size;
			    j = rand() % _alphabet_size;
			 }
			 char c = _alphabet[i];
			 _alphabet[i] = _alphabet[j];
			 _alphabet[j] = c;	 
		 }
 
	  }
      void randomise(Fixed_Key* fixed_key = 0)
      {
         // generate a random key, by picking letters at random
         char alphabet[31];
         strcpy(alphabet,_alphabet); 
         static int first_time = 1;
         if (first_time)
         {
            srand(time(0));
            first_time = 0;
         }
         strcpy(_alphabet, "                              ");
         int j = _alphabet_size;
         int i = 0;
         int index = 0;
         if (fixed_key && fixed_key->number_fixed() > 0)
         {
            memcpy(_alphabet, fixed_key->_fixed, sizeof(fixed_key->_fixed));    
            j = _alphabet_size - fixed_key->number_fixed();
            strcpy(alphabet, fixed_key->_not_fixed);
            for (i = 0; i < _alphabet_size - fixed_key->number_fixed(); i++)
            {
               int num = rand() % j;
               while (index < _alphabet_size && _alphabet[index] != ' ') index++;
               _alphabet[index] = tolower(alphabet[num]);
               strncpy(alphabet + num, alphabet + num + 1, j - num - 1);
               alphabet[j - 1] = '\0';
               j--;
            }
            _alphabet[_alphabet_size] = '\0';
            //cout << "Alphabet::randomise(): ABCDEFGHIJKLMNOPQRSTUVWXYZ" << endl;
            //cout << "                     : " << _alphabet << endl;
         }
         else
         {
            for (i = 0; i < _alphabet_size; i++)
            {
               int num = rand() % j;
               while (index < _alphabet_size && _alphabet[index] != ' ') index++;
               _alphabet[index] = tolower(alphabet[num]);
               strncpy(alphabet + num, alphabet + num + 1, j - num - 1);
               alphabet[j - 1] = '\0';
               j--;
            }
         }
         _alphabet[_alphabet_size] = '\0';
      }
      const char* alphabet() const { return _alphabet; }

	  void start_swaps()
	  {
		  _c1 = 0;
		  _c2 = 0;
          _c3 = 0; 
		  _c4 = 0;
          _state = 2; 
	  }

	  void next_swap(Fixed_Key* fixed_key = 0)
	  {
		  if (_c2 > _c1 && _c2 < _alphabet_size)
		  {
			  char c = _alphabet[_c1];
			  _alphabet[_c1] = _alphabet[_c2];
			  _alphabet[_c2] = c;
		  }

		  _c2++;
		  if (_c2 >= _alphabet_size)
		  {
			  _c1++;
			  _c2 = _c1 + 1;
		  }
		  while (_c2 < _alphabet_size && fixed_key && 
                 (fixed_key->is_set(_alphabet[_c1]) ||
                  fixed_key->is_set(_alphabet[_c2])))
		  {
  		     _c2++;
		     if (_c2 >= _alphabet_size)
			 {
			    _c1++;
			    _c2 = _c1 + 1;
			 }
		  }
		  if (_c2 < _alphabet_size)
		  {
			  char c = _alphabet[_c1];
			  _alphabet[_c1] = _alphabet[_c2];
			  _alphabet[_c2] = c;
		  }
	  }

	  int end_swaps()
	  {
		  return (_c2 >= _alphabet_size);
	  }
    void start_perms(int state = 2) 
     { 
        start_swaps(); 
		_state = state;
     } 

    void next_perm(Fixed_Key* fixed_key) 
    {
	   switch(_state)
	   {
	      case 2:
			  { 
                 next_swap(fixed_key); 
//                 if (end_swaps()) 
//				 { 
//                    start_3cycles(); 
//				 }  
			  } 
	      break;
	      case 3:
			  {
                 next_3cycle(fixed_key); 
//				 if (end_3cycles())
//				 {
//					 start_4cycles();
//				 }
			  }
		      break;
		  case 4:
			  {
				  next_4cycle(fixed_key);
			  }
			  break;
	   }
    } 

    int end_perms() 
    { 
		switch(_state)
		{
		case 2:
           return (_c2 >= _alphabet_size); 
		case 3:
           return (_c3 >= _alphabet_size); 
		case 4:
           return (_c4 >= _alphabet_size); 
		}
		return 1;
    } 

    void start_3cycles() 
    { 
       _c1 = 0; 
       _c2 = 1; 
       _c3 = 1; 
	   _c4 = 0;
       _state = 3; 
    } 

	    void start_4cycles() 
    { 
       _c1 = 0; 
       _c2 = 1; 
       _c3 = 2; 
	   _c4 = 2;
       _state = 4; 
    } 

    void next_3cycle(Fixed_Key* fixed_key) 
    { 
       if (_c3 > _c2 && _c3 < _alphabet_size) 
       { 
          char c = _alphabet[_c3]; 
          _alphabet[_c3] = _alphabet[_c2]; 
          _alphabet[_c2] = _alphabet[_c1]; 
          _alphabet[_c1] = c; 
       } 
       _c3++; 
       if (_c3 >= _alphabet_size) 
       { 
          _c2++; 
          _c3 = _c2 + 1; 
          if (_c3 >= _alphabet_size) 
          { 
             _c1++; 
             _c2 = _c1 + 1; 
             _c3 = _c2 + 1; 
          } 
       } 
       while (_c3 < _alphabet_size && fixed_key && 
              (fixed_key->is_set(_alphabet[_c1]) || 
               fixed_key->is_set(_alphabet[_c2]) || 
               fixed_key->is_set(_alphabet[_c3]))) 
       { 
          _c3++; 
          if (_c3 >= _alphabet_size) 
          { 
             _c2++; 
             _c3 = _c2 + 1; 

              _c3 = _c2 + 1; 
              if (_c3 >= _alphabet_size) 
              { 
                 _c1++; 
                 _c2 = _c1 + 1; 
                 _c3 = _c2 + 1; 
              } 
           } 
        } 
        if (_c3 < _alphabet_size) 
        { 
           char c = _alphabet[_c1]; 
           _alphabet[_c1] = _alphabet[_c2]; 
           _alphabet[_c2] = _alphabet[_c3]; 
           _alphabet[_c3] = c; 
        } 
     } 
    void next_4cycle(Fixed_Key* fixed_key) 
    { 
       if (_c4 > _c3 && _c4 < _alphabet_size) 
       { 
          char c = _alphabet[_c4]; 
          _alphabet[_c4] = _alphabet[_c3]; 
          _alphabet[_c3] = _alphabet[_c2]; 
          _alphabet[_c2] = _alphabet[_c1]; 
          _alphabet[_c1] = c; 
       } 
       _c4++; 
       if (_c4 >= _alphabet_size) 
       { 
          _c3++; 
          _c4 = _c3 + 1; 
          if (_c4 >= _alphabet_size) 
          { 
             _c2++; 
             _c3 = _c2 + 1; 
             _c4 = _c3 + 1;
			 if (_c4 >= _alphabet_size)
			 {
				 _c1++;
				 _c2 = _c1 + 1;
				 _c3 = _c2 + 1;
				 _c4 = _c3 + 1;
			 }
          } 
       } 
       while (_c4 < _alphabet_size && fixed_key && 
              (fixed_key->is_set(_alphabet[_c1]) || 
               fixed_key->is_set(_alphabet[_c2]) || 
               fixed_key->is_set(_alphabet[_c3]) || 
               fixed_key->is_set(_alphabet[_c4]))) 
       { 
          _c4++; 
          if (_c4 >= _alphabet_size) 
		  { 
             _c3++; 
             _c4 = _c3 + 1; 
             if (_c4 >= _alphabet_size) 
			 { 
                _c2++; 
                _c3 = _c2 + 1; 
                _c4 = _c3 + 1;
			    if (_c4 >= _alphabet_size)
				{
				    _c1++;
				    _c2 = _c1 + 1;
				    _c3 = _c2 + 1;
				    _c4 = _c3 + 1;
				}
			 } 
		  } 
	   } 
        if (_c4 < _alphabet_size) 
        { 
           char c = _alphabet[_c1]; 
           _alphabet[_c1] = _alphabet[_c2]; 
           _alphabet[_c2] = _alphabet[_c3]; 
           _alphabet[_c3] = _alphabet[_c4]; 
           _alphabet[_c4] = c; 
        } 
     } 
	  int end_3cycles()
	  {
		  return (_c3 >= _alphabet_size);
	  }
      int state() { return _state; } 
   private:
      enum { ALPHABET_SIZE = 26 };
      int _alphabet_size;
      char _alphabet[ALPHABET_SIZE + 1];
	  // variables for iterating through swaps
	  char _c1;
	  char _c2;
	  char _c3;
	  char _c4;
	  char _state;
};

class Alphabet_Generator
{
   public:
      Alphabet_Generator(const Alphabet& alp, Fixed_Key* fixed_key = 0)
      {
         generate(alp, fixed_key);
      }
      int size() { return _alphabet_list.size(); }
      const Alphabet& operator[] (int i) { return _alphabet_list[i]; }
      
   private:
      std::vector<Alphabet> _alphabet_list;
      void generate(const Alphabet& alp, Fixed_Key* fixed_key = 0)   
      {
         _alphabet_list.push_back(alp);
         // (i) swap all pairs
         int i = 0;
         for (i = 0; i < alp._alphabet_size; i++)
         {
            for (int j = i+1; j < alp._alphabet_size; j++)
            {
               if (!fixed_key || 
                   (!fixed_key->is_set(alp._alphabet[i]) &&
                    !fixed_key->is_set(alp._alphabet[j])))
               {
                  Alphabet a(alp);
                  // swap i and j in this
                  a._alphabet[j] = alp._alphabet[i];
                  a._alphabet[i] = alp._alphabet[j];
                  _alphabet_list.push_back(a);
               }
            }
         }
return;
         // (ii) move and slide
         int i1;
         int j1;
         for (i = 0; i < alp._alphabet_size; i++)
         {
            for (int j = i+2; j < alp._alphabet_size; j++)
            {
               Alphabet a(alp);
               char c = a._alphabet[i];
               memcpy(&a._alphabet[i], &a._alphabet[i+1], j - i);
               a._alphabet[j] = c;
               _alphabet_list.push_back(a);

               Alphabet b(a);
               i1 = alp._alphabet_size - 1 - j;
               j1 = alp._alphabet_size - 1 - i;
               c = b._alphabet[i1];
               memcpy(&b._alphabet[i1], &b._alphabet[i1+1], j1 - i1);
               b._alphabet[j1] = c;
               _alphabet_list.push_back(b);

               Alphabet d(alp);
               c = d._alphabet[j];
               memmove(&d._alphabet[i+1], &d._alphabet[i], j - i);
               d._alphabet[i] = c;
               _alphabet_list.push_back(d);

               b = d;
               c = b._alphabet[j1];
               memmove(&b._alphabet[i1+1], &b._alphabet[i1], j1 - i1);
               b._alphabet[i1] = c;
               _alphabet_list.push_back(b);
            }
         }
return;
         // (iii) rotate groups of 3 characters
         for (i = 0; i < alp._alphabet_size; i++)
         {
            Alphabet a(alp);
            int j = i;
            int l = 0;
            char c = a._alphabet[j];
            int next_j = j+1;
            if (next_j > alp._alphabet_size - 1) next_j = 0;
            for (l = 0; l < 2; l++)
            {
               a._alphabet[j] = a._alphabet[next_j];
               j++;
               if (j > alp._alphabet_size - 1) j = 0;
               next_j++;
               if (next_j > alp._alphabet_size - 1) next_j = 0;
            }  
            a._alphabet[j] = c;
            _alphabet_list.push_back(a);
            
            a = alp; 
            j = i;
            c = a._alphabet[j];
            next_j = j-1;
            if (next_j < 0) next_j = alp._alphabet_size - 1;
            for (l = 0; l < 2; l++)
            {
               a._alphabet[j] = a._alphabet[next_j];
               j--;
               if (j < 0) j = alp._alphabet_size - 1;
               next_j--;
               if (next_j < 0) next_j = alp._alphabet_size - 1;
            }  
            a._alphabet[j] = c;
            _alphabet_list.push_back(a);
         }
      }
     
};

#endif

