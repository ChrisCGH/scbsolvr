#ifndef __N_GRAPH_H
#define __N_GRAPH_H
const int MAX_N = 10;
class N_Graph
{
   public:
      N_Graph()
      {
         memset(str_, ' ', len_);
         str_[len_] = '\0';
      }
      N_Graph(const char* str)
      {
         memcpy(str_, str, len_);
         str_[len_] = '\0';
      }
      N_Graph(const N_Graph& g)
      {
         memcpy(str_, g.str_, len_);
         str_[len_] = '\0';
      }
      N_Graph& operator=(const N_Graph& g)
      {
         memcpy(str_, g.str_, len_);
         str_[len_] = '\0';
         return *this;
      }
      operator char* () const
      {
         return (char*)str_;
      }
      int operator==(const N_Graph& g)
      {
         return (memcmp(str_, g.str_, len_) == 0);
      }
      int operator!=(const N_Graph& g)
      {
         return (memcmp(str_, g.str_, len_) != 0);
      }
      int operator<(const N_Graph& g)
      {
         return (memcmp(str_, g.str_, len_) < 0);
      }
      int operator<(const N_Graph& g) const
      {
         return (memcmp(str_, g.str_, len_) < 0);
      }
      friend std::ostream& operator<< (std::ostream& os, const N_Graph& g)
      {
         os << g.str_;
         return os;
      }
      ~N_Graph() {};
      static void set_length(int len)
      {
         len_ = len;
      }

   private:
      char str_[MAX_N];
      static int len_;
};

#endif