#ifndef __MONO_H
#define __MONO_H
#include <iostream>
#include <string.h>

class Mono
{
   public:
      Mono()
      {
         static char* initial_key = "abcdefghijklmnopqrstuvwxyz";
         //strcpy(key, initial_key);
		 _key = initial_key;
      }
      ~Mono() {}
      void set_key(const char* new_key)
	  {
         //strcpy(key, new_key);
		 _key = const_cast<char*>(new_key);
      }
      void display()
      {
         static char* PT = "abcdefghijklmnopqrstuvwxyz";
         char CT[27];
         
         for (int i = 0; i < 26; i++)
         {
//            CT[key[i] - 'a'] = 'A' + i; 
            CT[_key[i] - 'a'] = 'A' + i; 
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
            CT[_key[i] - 'a'] = 'A' + i; 
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
            else *d = _key[i];
            c++;
            d++;
         }
      }

   private:
	  char* _key;
      char key[27];
};
#endif
