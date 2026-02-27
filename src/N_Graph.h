#ifndef __N_GRAPH_H
#define __N_GRAPH_H
const int MAX_N = 10;
class N_Graph
{
   public:
	   N_Graph() 
	   { 
		   memset(_str, ' ', _len); 
	       _str[_len] = '\0'; 
	   }
      N_Graph(const char* str)
      {
         memcpy(_str, str, _len);
       _str[_len] = '\0';
      }
      N_Graph(const N_Graph& g)
      {
         memcpy(_str, g._str, _len);
       _str[_len] = '\0';
      }
      N_Graph& operator=(const N_Graph& g)
      {
         memcpy(_str, g._str, _len);
       _str[_len] = '\0';
         return *this;
      }
     operator char* () const
     {
        return (char*)_str;
     }
      int operator==(const N_Graph& g)
      {
         return (memcmp(_str, g._str, _len) == 0);
      }
      int operator!=(const N_Graph& g)
      {
         return (memcmp(_str, g._str, _len) != 0);
      }
      int operator<(const N_Graph& g)
      {
         return (memcmp(_str, g._str, _len) < 0);
      }
      int operator<(const N_Graph& g) const
      {
         return (memcmp(_str, g._str, _len) < 0);
      }
	  friend std::ostream& operator<< (std::ostream& os, const N_Graph& g)
      {
         os << g._str;
         return os;
      }
      ~N_Graph() {};
     static void set_length(int len) { _len = len; }
      
   private:
      char _str[MAX_N];
     static int _len;
};

#endif