#ifndef __HASHTABLE_H
#define __HASHTABLE_H
#include <cstring>
#include "hash.h"

class Hash_Table
{
   public:
      Hash_Table(size_t size = 1300021) : next_data_(0)
      {
         construct_table(&hash_table_, size);
         data_array_ = new double [ size ];
         std::memset(data_array_, 0, sizeof(double)*size);
      }
      ~Hash_Table()
      {
         free_table(&hash_table_, 0);
         delete [] data_array_;
         data_array_ = 0;
         next_data_ = 0;
      }
      void clear()
      {
      }
      void insert(char* str, double d)
      {
         double* dp = (double*)(::lookup(str, &hash_table_));
         if (!dp)
         {
            dp = &data_array_[next_data_];
            next_data_++;
         }
         *dp = d;
         ::insert(str, (void*)dp, &hash_table_);
      }
      const double& operator[](char* str) const
      {
         static const double zero = 0.0;
         double* dp = (double*)(::lookup(str, (hash_table*)&hash_table_));
         if (dp) return *dp;
         return zero;
      }

   private:
      hash_table hash_table_;
      double* data_array_;
      int next_data_;
};
#endif
