#ifndef __MONO_H
#define __MONO_H
#include <iostream>
#include <cstring>

class Mono
{
   public:
      Mono()
      {
         static const char* initial_key = "abcdefghijklmnopqrstuvwxyz";
         //strcpy(key, initial_key);
         key_ = const_cast<char*>(initial_key);
      }
      ~Mono() {}
      void set_key(const char* new_key)
      {
         //strcpy(key, new_key);
         key_ = const_cast<char*>(new_key);
      }
      void display()
      {
         static const char* PT = "abcdefghijklmnopqrstuvwxyz";
         char CT[27];

         for (int i = 0; i < 26; i++)
         {
//            CT[key[i] - 'a'] = 'A' + i;
            CT[key_[i] - 'a'] = 'A' + i;
         }
         CT[26] = '\0';
         std::cout << CT << std::endl;
         std::cout << PT << std::endl;
      }
      char* get_key()
      {
         static char CT[27];

         for (int i = 0; i < 26; i++)
         {
//            CT[key[i] - 'a'] = 'A' + i;
            CT[key_[i] - 'a'] = 'A' + i;
         }
         CT[26] = '\0';
         return CT;
      }
      void decode(const char* ciphertext, char* outtext)
      {
         const char* c = ciphertext;
         char* d = outtext;
         strcpy(outtext, ciphertext);
         while (c && *c)
         {
            int i = *c - 'A';
            if (*c == 'z' + 1) *d = *c;
//            else *d = key[i];
            else *d = key_[i];
            c++;
            d++;
         }
      }

   private:
      char* key_;
      char key[27];
};
#endif
