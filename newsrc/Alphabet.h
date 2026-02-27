#ifndef __ALPHABET_H
#define __ALPHABET_H

#include <cstdlib>
#include <time.h>
#include <cstring>
#include <vector>
#include <iostream>
#include "fixed.h"

class Alphabet
{
   public:
      friend class Alphabet_Generator;
      Alphabet(const char* str= "abcdefghijklmnopqrstuvwxyz")
      {
         strcpy(alphabet_, str);
         alphabet_size_ = strlen(alphabet_);
      }
      Alphabet(const Alphabet& alp)
      {
         strcpy(alphabet_, alp.alphabet_);
         alphabet_size_ = alp.alphabet_size_;
      }
      Alphabet& operator=(const Alphabet& alp)
      {
         if (this == &alp) return *this;
         strcpy(alphabet_, alp.alphabet_);
         alphabet_size_ = alp.alphabet_size_;
         return *this;
      }

      int operator==(const Alphabet& alp) const
      {
         return (strcmp(alphabet_, alp.alphabet_) == 0);
      }
      int operator!=(const Alphabet& alp) const
      {
         return (strcmp(alphabet_, alp.alphabet_) != 0);
      }
      int operator<(const Alphabet& alp) const
      {
         return 1;
      }

      void display() const
      {
         const char* c = alphabet_;
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
            int i = rand() % alphabet_size_;
            int j = rand() % alphabet_size_;
            while (i == j ||
                   (fixed_key &&
                    (fixed_key->is_set(alphabet_[i]) ||
                     fixed_key->is_set(alphabet_[j]))))
            {
               i = rand() % alphabet_size_;
               j = rand() % alphabet_size_;
            }
            char c = alphabet_[i];
            alphabet_[i] = alphabet_[j];
            alphabet_[j] = c;
         }

      }
      void randomise(Fixed_Key* fixed_key = 0)
      {
         // generate a random key, by picking letters at random
         char alphabet[31];
         strcpy(alphabet,alphabet_);
         static int first_time = 1;
         if (first_time)
         {
            srand(time(0));
            first_time = 0;
         }
         strcpy(alphabet_, "                              ");
         int j = alphabet_size_;
         int i = 0;
         int index = 0;
         if (fixed_key && fixed_key->number_fixed() > 0)
         {
            memcpy(alphabet_, fixed_key->fixed_, sizeof(fixed_key->fixed_));
            j = alphabet_size_ - fixed_key->number_fixed();
            strcpy(alphabet, fixed_key->not_fixed_);
            for (i = 0; i < alphabet_size_ - fixed_key->number_fixed(); i++)
            {
               int num = rand() % j;
               while (index < alphabet_size_ && alphabet_[index] != ' ') index++;
               alphabet_[index] = tolower(alphabet[num]);
               strncpy(alphabet + num, alphabet + num + 1, j - num - 1);
               alphabet[j - 1] = '\0';
               j--;
            }
            alphabet_[alphabet_size_] = '\0';
            //cout << "Alphabet::randomise(): ABCDEFGHIJKLMNOPQRSTUVWXYZ" << endl;
            //cout << "                     : " << alphabet_ << endl;
         }
         else
         {
            for (i = 0; i < alphabet_size_; i++)
            {
               int num = rand() % j;
               while (index < alphabet_size_ && alphabet_[index] != ' ') index++;
               alphabet_[index] = tolower(alphabet[num]);
               strncpy(alphabet + num, alphabet + num + 1, j - num - 1);
               alphabet[j - 1] = '\0';
               j--;
            }
         }
         alphabet_[alphabet_size_] = '\0';
      }
      const char* alphabet() const
      {
         return alphabet_;
      }

      void start_swaps()
      {
         c1_ = 0;
         c2_ = 0;
         state_ = 2;
      }

      void next_swap(Fixed_Key* fixed_key = 0)
      {
         if (c2_ > c1_ && c2_ < alphabet_size_)
         {
            char c = alphabet_[c1_];
            alphabet_[c1_] = alphabet_[c2_];
            alphabet_[c2_] = c;
         }

         c2_++;
         if (c2_ >= alphabet_size_)
         {
            c1_++;
            c2_ = c1_ + 1;
         }
         while (c2_ < alphabet_size_ && fixed_key &&
                (fixed_key->is_set(alphabet_[c1_]) ||
                 fixed_key->is_set(alphabet_[c2_])))
         {
            c2_++;
            if (c2_ >= alphabet_size_)
            {
               c1_++;
               c2_ = c1_ + 1;
            }
         }
         if (c2_ < alphabet_size_)
         {
            char c = alphabet_[c1_];
            alphabet_[c1_] = alphabet_[c2_];
            alphabet_[c2_] = c;
         }
      }

      int end_swaps()
      {
         return (c2_ >= alphabet_size_);
      }
      int state()
      {
         return state_;
      }
   private:
      enum { ALPHABET_SIZE = 26 };
      int alphabet_size_;
      char alphabet_[ALPHABET_SIZE + 1];
      // variables for iterating through swaps
      char c1_;
      char c2_;
      char state_;
};

class Alphabet_Generator
{
   public:
      Alphabet_Generator(const Alphabet& alp, Fixed_Key* fixed_key = 0)
      {
         generate(alp, fixed_key);
      }
      int size()
      {
         return alphabet_list_.size();
      }
      const Alphabet& operator[] (int i)
      {
         return alphabet_list_[i];
      }

   private:
      std::vector<Alphabet> alphabet_list_;
      void generate(const Alphabet& alp, Fixed_Key* fixed_key = 0)
      {
         alphabet_list_.push_back(alp);
         // (i) swap all pairs
         int i = 0;
         for (i = 0; i < alp.alphabet_size_; i++)
         {
            for (int j = i+1; j < alp.alphabet_size_; j++)
            {
               if (!fixed_key ||
                     (!fixed_key->is_set(alp.alphabet_[i]) &&
                      !fixed_key->is_set(alp.alphabet_[j])))
               {
                  Alphabet a(alp);
                  // swap i and j in this
                  a.alphabet_[j] = alp.alphabet_[i];
                  a.alphabet_[i] = alp.alphabet_[j];
                  alphabet_list_.push_back(a);
               }
            }
         }
         return;
         // (ii) move and slide
         int i1;
         int j1;
         for (i = 0; i < alp.alphabet_size_; i++)
         {
            for (int j = i+2; j < alp.alphabet_size_; j++)
            {
               Alphabet a(alp);
               char c = a.alphabet_[i];
               memcpy(&a.alphabet_[i], &a.alphabet_[i+1], j - i);
               a.alphabet_[j] = c;
               alphabet_list_.push_back(a);

               Alphabet b(a);
               i1 = alp.alphabet_size_ - 1 - j;
               j1 = alp.alphabet_size_ - 1 - i;
               c = b.alphabet_[i1];
               memcpy(&b.alphabet_[i1], &b.alphabet_[i1+1], j1 - i1);
               b.alphabet_[j1] = c;
               alphabet_list_.push_back(b);

               Alphabet d(alp);
               c = d.alphabet_[j];
               memmove(&d.alphabet_[i+1], &d.alphabet_[i], j - i);
               d.alphabet_[i] = c;
               alphabet_list_.push_back(d);

               b = d;
               c = b.alphabet_[j1];
               memmove(&b.alphabet_[i1+1], &b.alphabet_[i1], j1 - i1);
               b.alphabet_[i1] = c;
               alphabet_list_.push_back(b);
            }
         }
         return;
         // (iii) rotate groups of 3 characters
         for (i = 0; i < alp.alphabet_size_; i++)
         {
            Alphabet a(alp);
            int j = i;
            int l = 0;
            char c = a.alphabet_[j];
            int next_j = j+1;
            if (next_j > alp.alphabet_size_ - 1) next_j = 0;
            for (l = 0; l < 2; l++)
            {
               a.alphabet_[j] = a.alphabet_[next_j];
               j++;
               if (j > alp.alphabet_size_ - 1) j = 0;
               next_j++;
               if (next_j > alp.alphabet_size_ - 1) next_j = 0;
            }
            a.alphabet_[j] = c;
            alphabet_list_.push_back(a);

            a = alp;
            j = i;
            c = a.alphabet_[j];
            next_j = j-1;
            if (next_j < 0) next_j = alp.alphabet_size_ - 1;
            for (l = 0; l < 2; l++)
            {
               a.alphabet_[j] = a.alphabet_[next_j];
               j--;
               if (j < 0) j = alp.alphabet_size_ - 1;
               next_j--;
               if (next_j < 0) next_j = alp.alphabet_size_ - 1;
            }
            a.alphabet_[j] = c;
            alphabet_list_.push_back(a);
         }
      }

};

#endif

