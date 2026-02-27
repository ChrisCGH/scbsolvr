#pragma warning(disable:4786)
// program to produce ngraph table from 
// input ASCII file
#include <iostream>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <map>

const int MAX_N = 10;
class N_Graph
{
   public:
      N_Graph()
      {
         _str[0] = '\0';
      }
      N_Graph(const char* str)
      {
         strcpy(_str, str);
      }
      N_Graph(const N_Graph& g)
      {
         strcpy(_str, g._str);
      }
      N_Graph& operator=(const N_Graph& g)
      {
         strcpy(_str, g._str);
         return *this;
      }
      int operator==(const N_Graph& g)
      {
         return (strcmp(_str, g._str) == 0);
      }
      int operator!=(const N_Graph& g)
      {
         return (strcmp(_str, g._str) != 0);
      }
      int operator<(const N_Graph& g)
      {
         return (strcmp(_str, g._str) < 0);
      }
      int operator<(const N_Graph& g) const
      {
         return (strcmp(_str, g._str) < 0);
      }
      friend std::ostream& operator<< (std::ostream& os, const N_Graph& g)
      {
         os << g._str;
         return os;
      }
      ~N_Graph() {};
      
   private:
      char _str[MAX_N];
};

std::map<N_Graph, int> freq_table;
int total = 0;

void process_ngraphs(char* str, int n)
{
   char ngraph[MAX_N];
   char* c = str;
   while (*(c + n))
   {
      memcpy(ngraph, c, n);
      ngraph[n] = '\0'; 
      freq_table[N_Graph(ngraph)]++;
      total++;
      c++;
   }
}

int main(int argc, char** argv)
{
   int n = 4;
   if (argc > 1) n = atoi(argv[1]);
   // read file from stdin
   char buf[132];
   char buf1[132];
   int start = 0;

   std::cerr << "Reading file ..." << std::endl;
   while (std::cin.getline(buf, 132))
   {
      char* c = buf;
      char* d = buf1 + start;
      while (c && *c)
      {
         if (isalpha(*c))
         {
            *d++ = toupper(*c);
         }
         c++;
      }
      *d = '\0';
      int len = strlen(buf1);
      if (len > n - 1)
      {
         process_ngraphs(buf1, n);
         strncpy(buf1, buf1 + len - n + 1, n - 1);
         start = n - 1; 
      }
   }
   std::cerr << "Done" << std::endl;

   int x = (int)(-1.0 * log(100.0 / total)) + 1; 
   for (std::map<N_Graph, int>::iterator iter = freq_table.begin();
        iter != freq_table.end();
        ++iter)
   {
      double score = x + log((iter->second * 100.0) / total);
      std::cout << iter->first << " : " << iter->second << " " << score << std::endl;
   }
       
   return 0;
}
