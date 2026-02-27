#ifndef __NGRAPH_H
#define __NGRAPH_H
#pragma warning(disable:4786)
#include <fstream>
#include <ctype.h>
#include <math.h>
#include <map>
#include "N_Graph.h"

class Ngraph_File_Creator
{
public:
	Ngraph_File_Creator(const char* sample_file, const char* ngraph_file, int n, int spaces = 0)
		: _sample_file(0), _ngraph_file(0), _n(n), _total(0), 
		  _progress(0), _finished(0), _spaces(spaces)
	{
		_sample_file = new char [ strlen(sample_file) + 1 ];
		strcpy(_sample_file, sample_file);
		_ngraph_file = new char [ strlen(ngraph_file) + 1 ];
		strcpy(_ngraph_file, ngraph_file);
		N_Graph::set_length(_n);
	}
	void go()
	{
		create_ngraph_file();
		_finished = 1;
		_user_callback(_user_callback_object, *this);
	}
	int is_done() const { return _finished; }
	int get_progress() const { return _progress; }
	  // user callback function to allow results to be 
	  // passed back 
      typedef void (*USER_CALLBACK)(void*, const Ngraph_File_Creator&);
	  void set_user_callback(void* o, USER_CALLBACK f)
	  {
		  _user_callback = f;
		  _user_callback_object = o;
	  }
	~Ngraph_File_Creator()
	{
		delete [] _sample_file;
		delete [] _ngraph_file;
	}
private:
	int _n;
	char* _sample_file;
	char* _ngraph_file;
    std::map<N_Graph, int> freq_table;
    int _total;
	int _progress;
	int _finished;
	int _spaces;
	USER_CALLBACK _user_callback;
	void* _user_callback_object;

    void process_ngraphs(char* str)
	{
       char ngraph[MAX_N];
       char* c = str;
       while (*(c + _n))
	   {
          memcpy(ngraph, c, _n);
          ngraph[_n] = '\0'; 
          freq_table[N_Graph(ngraph)]++;
          _total++;
          c++;
	   }
	}
	void create_ngraph_file()
	{
        char buf[132];
        char buf1[132];
        int start = 0;
		_progress = 0;
		int prev_was_space = 0;
		std::fstream file(_sample_file, std::ios::in);
		{
           while (file.getline(buf, 132))
		   {
              char* c = buf;
              char* d = buf1 + start;
              while (c && *c)
			  {
                 if (isalpha(*c))
				 {
                    *d++ = toupper(*c);
					prev_was_space = 0;
				 }
				 else if (_spaces && 
					      (*c == ' ' || *c == '.' || *c == ',' || *c == ';') 
						  && !prev_was_space)
				 { 
                    *d++ = '{';
					prev_was_space = 1;
				 }
                 c++;
			  }
              *d = '\0';
              int len = strlen(buf1);
              if (len > _n - 1)
			  {
                 process_ngraphs(buf1);
                 strncpy(buf1, buf1 + len - _n + 1, _n - 1);
                 start = _n - 1; 
			  }
			  _progress += strlen(buf);
			  if (_progress % 1000 == 0)
			  {
				  _user_callback(_user_callback_object, *this);
			  }
		   }	
		   file.close();
		}
		_user_callback(_user_callback_object, *this);
		std::fstream nfile(_ngraph_file, std::ios::out);
		int x = (int)(-1.0 * log(100.0 / _total)) + 1; 
        for (std::map<N_Graph, int>::iterator iter = freq_table.begin();
             iter != freq_table.end();
             ++iter)
		{
           double score = x + log((iter->second * 100.0) / _total);
		   nfile << iter->first << " : " << iter->second << " " << score << std::endl;
		}
	}
};

#endif