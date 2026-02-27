#ifndef __FIXED_H
#define __FIXED_H
#include <cctype>
#include <cstring>
#include <iostream>

class Fixed_Key
{
      friend class Alphabet;
      friend class Alphabet_Generator;
   public:
      Fixed_Key() : dummy_(0)
      {
         memset(fixed_, ' ', sizeof(fixed_));
         set_index();
      }
      Fixed_Key(const char* key) : dummy_(0)
      {
         if (strlen(key) > sizeof(fixed_))
         {
            memcpy(fixed_, key, sizeof(fixed_));
         }
         else
         {
            memset(fixed_, ' ', sizeof(fixed_));
            memcpy(fixed_, key, strlen(key));
         }
         set_index();
      }
      Fixed_Key(const Fixed_Key& f) : dummy_(0)
      {
         memcpy(fixed_, f.fixed_, sizeof(fixed_));
         set_index();
      }
      ~Fixed_Key() {}
      Fixed_Key& operator=(const Fixed_Key& f)
      {
         if (this != &f)
         {
            memcpy(fixed_, f.fixed_, sizeof(fixed_));
            set_index();
            dummy_ = 0;
         }
         return *this;
      }
      void operator=(char*);
      char get_pt(const char ct) const
      {
         int i = ct - 'A';
         if (i < 0 || i >= sizeof(fixed_)) return ' ';
         return fixed_[i];
      }
      char get_ct(const char pt) const
      {
         int i = pt - 'a';
         if (i < 0 || i >= sizeof(fixed_)) return ' ';
         if (index_[i] < 0) return ' ';
         return (index_[i] + 'A');
      }
      void set(const char pt, const char ct)
      {
         if (!isupper(ct)) return;
         if (!islower(pt)) return;
         int i = ct - 'A';
         if (i < 0 || i >= sizeof(fixed_)) return;
         if (is_set(pt)) clear(get_ct(pt), 0);
         fixed_[i] = pt;
         set_index();
      }
      void clear(const char ct, int do_index = 1)
      {
         int i = ct - 'A';
         if (i < 0 || i >= sizeof(fixed_)) return;
         if (fixed_[i] != ' ')
         {
            fixed_[i] = ' ';
            if (do_index) set_index();
         }
      }
      int is_set(const char pt) const
      {
         return (get_ct(pt) != ' ');
      }
      int number_fixed() const
      {
         return number_fixed_;
      }
      void display()
      {
         std::cout << "number fixed = " << number_fixed_ << std::endl;
         if (number_fixed_)
         {
            std::cout << "ABCDEFGHIJKLMNOPQRSTUVWXYZ" << std::endl;
            for (int i = 0; i < sizeof(fixed_); i++)
            {
               std::cout << fixed_[i];
            }
            std::cout << std::endl;
            std::cout << "Not fixed : [" << not_fixed_ << "]" << std::endl;
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
         if (dummy_ != 0) return -1;
         int num_fixed = 0;
         for (int i = 0; i < sizeof(fixed_); i++)
         {
            if (index_[i] != -1 && index_[i] < 0 && index_[i] >= sizeof(fixed_)) return -2;
            if (index_[i] != -1) num_fixed++;
         }
         if (num_fixed != number_fixed_) return -3;
         return 1;
      }
      void clear()
      {
         memset(fixed_, ' ', sizeof(fixed_));
         set_index();
      }

   private:
      char fixed_[26];
      char dummy_;
      char not_fixed_[27];
      int index_[26];
      int number_fixed_;
      void set_index()
      {
         int i = 0;
         number_fixed_ = 0;
         for (i = 0; i < sizeof(fixed_); i++)
         {
            index_[i] = -1;
         }
         for (i = 0; i < sizeof(fixed_); i++)
         {
            if (fixed_[i] != ' ')
            {
               index_[fixed_[i] - 'a'] = i;
               number_fixed_++;
            }
         }
         i = 0;
         for (char c = 'a'; c <= 'z'; c++)
         {
            if (!is_set(c))
            {
               not_fixed_[i] = c;
               i++;
            }
         }
         not_fixed_[i] = '\0';
      }
};

#endif
