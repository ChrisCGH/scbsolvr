#pragma warning(disable:4786)
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <cctype>
#include "mono_solver.h"

int main(int argc, char** argv)
{
   char filename[132];
   char trigram_file[132];
   char tetragram_file[132];
   char ngraph_file[132];
   char line[132];
   char ciphertext[2000];
   int verbose = 0;

   strcpy(filename, "cryptogram.txt");
   strcpy(trigram_file, "english.tri");
   strcpy(tetragram_file, "english.tet");
   strcpy(ngraph_file, "english.tet");
   int use_trigram = 1;
   int try_limit = 0;

   int arg = 1;
   while (arg < argc)
   {
      if (strcmp(argv[arg], "-v") == 0)
      {
         verbose = 1; 
         arg++;
      }
      else if (strcmp(argv[arg], "-t") == 0)
      {
         arg++;
         if (arg < argc && (arg+1) < argc && argv[arg][0] != '-')
         {
            strcpy(trigram_file, argv[arg]);
            arg++;
         }
         use_trigram = 1;
      }
      else if (strcmp(argv[arg], "-T") == 0)
      {
         arg++;
         if (arg < argc && (arg+1) < argc && argv[arg][0] != '-')
         {
            strcpy(tetragram_file, argv[arg]);
            arg++;
         }
         use_trigram = 0;
      }      else if (strcmp(argv[arg], "-n") == 0)
      {
         arg++;
         if (arg < argc && (arg+1) < argc && argv[arg][0] != '-')
         {
            strcpy(ngraph_file, argv[arg]);
            arg++;
         }
         use_trigram = 2;
      }
      else if (strcmp(argv[arg], "-l") == 0)
      {
         arg++;
         try_limit = 5000000;
      }
      else
      {
         strcpy(filename, argv[arg]);
         arg++;
      }
   }

   // get ciphertext from file
   char buffer[2000];
   buffer[0] = '\0';
   ciphertext[0] = '\0';
   std::fstream file(filename, std::ios::in);
   while (file.getline(line, 132))
   {
      char* c = line;
      char* d = buffer;
      while (c && *c)
      {
         if (isalpha(*c))
         {
            *d = toupper(*c);
            d++;
         }
         c++;
      }
      *d = '\0';
      strcat(ciphertext, buffer); 
   }

   Mono_Solver solver;

   solver.set_cipher_text(ciphertext);

   if (use_trigram == 1) solver.set_trigraph_scoring(trigram_file);
   else if (use_trigram == 0) solver.set_tetragraph_scoring(tetragram_file);
   else solver.set_ngraph_scoring(ngraph_file);

   if (verbose) solver.set_verbose();
   if (try_limit) solver.set_max_iterations(2000000);

   solver.solve();

   for (int i = 0; i < 10; i++) solver.display_best(i);

   return 0;
}
