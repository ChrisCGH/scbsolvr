#ifndef __FIXED_H
#define __FIXED_H
#include <ctype.h>
#include <string.h>
#include <iostream>

class Fixed_Key
{
   friend class Alphabet;
   friend class Alphabet_Generator;
   public:
	   Fixed_Key() : _dummy(0)
      {
         memset(_fixed, ' ', sizeof(_fixed));
         set_index();
      }
	   Fixed_Key(const char* key) : _dummy(0)
      { 
         if (strlen(key) > sizeof(_fixed))
         {
            memcpy(_fixed, key, sizeof(_fixed));
         }
         else
         {
            memset(_fixed, ' ', sizeof(_fixed));
            memcpy(_fixed, key, strlen(key));
         }
         set_index();
      }
	   Fixed_Key(const Fixed_Key& f) : _dummy(0)
      {
         memcpy(_fixed, f._fixed, sizeof(_fixed));
         set_index();
      }
      ~Fixed_Key() {}
      Fixed_Key& operator=(const Fixed_Key& f)
      {
         if (this != &f)
         {
            memcpy(_fixed, f._fixed, sizeof(_fixed));
            set_index();
			_dummy = 0;
         }
         return *this;
      }
      char get_pt(const char ct) const
      {
         int i = ct - 'A';
         if (i < 0 || i >= sizeof(_fixed)) return ' ';
         return _fixed[i];
      }
      char get_ct(const char pt) const
      {
         int i = pt - 'a';
         if (i < 0 || i >= sizeof(_fixed)) return ' ';
         if (_index[i] < 0) return ' ';
         return (_index[i] + 'A');
      }
      void set(const char pt, const char ct)
      {
         if (!isupper(ct)) return;
         if (!islower(pt)) return;
         int i = ct - 'A';
         if (i < 0 || i >= sizeof(_fixed)) return;
         if (is_set(pt)) clear(get_ct(pt), 0);
         _fixed[i] = pt;
         set_index();
      }
      void clear(const char ct, int do_index = 1)
      {
         int i = ct - 'A';
         if (i < 0 || i >= sizeof(_fixed)) return;
         if (_fixed[i] != ' ')
         {
            _fixed[i] = ' ';
            if (do_index) set_index();
         }
      }
      int is_set(const char pt) const
      {
         return (get_ct(pt) != ' ');
      }
      int number_fixed() const
      {
         return _number_fixed;
      }
      void display()
      {
		  std::cout << "number fixed = " << _number_fixed << std::endl;
         if (_number_fixed)
         {
			 std::cout << "ABCDEFGHIJKLMNOPQRSTUVWXYZ" << std::endl;
            for (int i = 0; i < sizeof(_fixed); i++)
            {
				std::cout << _fixed[i];
            }
			std::cout << std::endl;
			std::cout << "Not fixed : [" << _not_fixed << "]" << std::endl;
         }
      }
	  const char* get_key() const
	  {
		  static char key[27];
		  for (char pt = 'a'; pt <= 'z'; pt++)
		  {
			  char ct = get_ct(pt);
			  if (ct == ' ') key[pt - 'a'] = '.';
			  else key[pt - 'a'] = ct;
		  }
		  key[26] = '\0';
		  return key;
	  }
	  int check()
	  {
		  // validation routine, for debugging
		  if (_dummy != 0) return -1;
		  int num_fixed = 0;
		  for (int i = 0; i < sizeof(_fixed); i++)
		  {
			  if (_index[i] != -1 && _index[i] < 0 && _index[i] >= sizeof(_fixed)) return -2;
			  if (_index[i] != -1) num_fixed++;
		  }
		  if (num_fixed != _number_fixed) return -3;
		  return 1;
	  }
	  void clear()
	  {
         memset(_fixed, ' ', sizeof(_fixed));
         set_index();
 	  }

   private:
      char _fixed[26];
	  char _dummy;
      char _not_fixed[27];
      int _index[26];
      int _number_fixed;
      void set_index()
      {
         int i = 0;
         _number_fixed = 0;
         for (i = 0; i < sizeof(_fixed); i++)
         {
            _index[i] = -1;
         }
         for (i = 0; i < sizeof(_fixed); i++)
         {
            if (_fixed[i] != ' ')
            {
               _index[_fixed[i] - 'a'] = i;
               _number_fixed++;
            }
         }
         i = 0;
         for (char c = 'a'; c <= 'z'; c++)
         {
            if (!is_set(c))
            {
               _not_fixed[i] = c;
               i++;
            }
         }
         _not_fixed[i] = '\0'; 
      }
};

#endif
