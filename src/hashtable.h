#ifndef __HASHTABLE_H
#define __HASHTABLE_H
#include "hash.h"

class Hash_Table
{
   public:
	   Hash_Table(size_t size = 1300021) : _next_data(0)
	  {
         construct_table(&_hash_table, size);
		 _data_array = new double [ size ];
		 memset(_data_array, 0, sizeof(double)*size);
	  }
	  ~Hash_Table()
	  {
		 free_table(&_hash_table, 0);
		 delete [] _data_array;  
		 _data_array = 0;
		 _next_data = 0;
	  }
	  void clear()
	  {
	  }
	  void insert(char* str, double d)
	  {
		  double* dp = (double*)(::lookup(str, &_hash_table));
		  if (!dp) 
		  {
		     dp = &_data_array[_next_data];
			 _next_data++;
		  }
		  *dp = d;
		  ::insert(str, (void*)dp, &_hash_table);
	  }
	  const double& operator[](char* str) const
	  {
		  static const double zero = 0.0;
		  double* dp = (double*)(::lookup(str, (hash_table*)&_hash_table));
		  if (dp) return *dp;
		  return zero;
	  }	  

   private:
	   hash_table _hash_table;
	   double* _data_array;
	   int _next_data;
};
#endif