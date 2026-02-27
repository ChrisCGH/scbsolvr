#ifndef __MONO_SOLVER_H
#define __MONO_SOLVER_H
#include <iostream>
#include <fstream>
#include "mono.h"
#include "scorer.h"
#include "top.h"
#include "fixed.h"
#include <ctype.h>

class Mono_Solver
{
   public:
	  Mono_Solver() 
		  : _top(10), _max_top(1), 
		    _max_iterations(0), _ciphertext(0), _verbose(0), 
			_user_callback(0), _user_callback_object(0),
			_terminate(0), _fixed(0), _outtext(0)
	  {
	  }
      ~Mono_Solver()
	  {
		  delete [] _ciphertext;
		  delete [] _outtext;
	  }

	  int spaces_scored() const { return _scorer.spaces_scored(); }

	  void set_trigraph_scoring(const char* trifile)
	  {
		  _scorer.set_trigram_file(trifile);
	  }
	  void set_tetragraph_scoring(const char* tetrafile)
	  {
		  _scorer.set_tetragram_file(tetrafile);
	  }
	  void set_ngraph_scoring(const char* nfile)
	  {
		  _scorer.set_ngraph_file(nfile);
	  }
	  void set_scoring_file(const char* filename)
	  {
		  std::fstream file(filename, std::ios::in);
      
         char line[132];
      
         if (file.getline(line, 132))
         {
            // format is XXXXX : NNNNN PCT
			file.close();
            char* c = strchr(line, ':');
			switch (c - line)
			{
			case 4:
				set_trigraph_scoring(filename);
				break;
			case 5:
				set_tetragraph_scoring(filename);
				break;
			default:
				set_ngraph_scoring(filename);
                break;
			}
		 }
	  }

	  // user callback function to allow results to be 
	  // passed back 
      typedef void (*USER_CALLBACK)(void*, const Mono_Solver&);
	  void set_user_callback(void* o, USER_CALLBACK f)
	  {
		  _user_callback = f;
		  _user_callback_object = o;
	  }

	  void set_fixed(Fixed_Key* fixed)
	  {
		  _fixed = fixed;
	  }

      int solve()
	  {
         //cout << _ciphertext << endl;
		 _terminate = 0;
		 int max_score = -1000000;
		 int local_max_score = -1000000;
		 int best_local_max_score = -1000000;
		 int score = -1000000;
		 int done = 0;
		 int i = 0;
		 char* outtext = new char [ strlen(_ciphertext) + 1 ];
		 _scorer.reset();
		 _top.clear();

		 time_t start_time = time(0);

		 Alphabet random("abcdefghijklmnopqrstuvwxyz");
		 Alphabet best_local_max_alphabet;
		 Alphabet local_max_alphabet;
		 stuck_count_ = 0;

		 Top top(_max_top);
		 i = 0;
		 while (i < _max_top)
		 {
			 random.randomise(_fixed);
			 _mono.set_key(random.alphabet());
			 
			 _mono.decode(_ciphertext, outtext);
			 score = _scorer.score(outtext);
			 if (top.qualifies(score))
			 {
			    top.add(random, score);
			    i++;
			 }
		 }
		 
		 while (!done)
		 {
			 Top top1(_max_top);
			 for (int g = 0; g < top.size(); g++)
			 {
				 int local_max_found = 0;
				 int local_max_score = -1000000;
				 random = top[g];
				 while (!local_max_found)
				 {
					 random.start_swaps();
					 max_score = -1000000;
					 Alphabet best_alphabet;
					 while (!random.end_swaps())
					 {
						 _mono.set_key(random.alphabet());
						 _mono.decode(_ciphertext, outtext);
						 score = _scorer.score(outtext);
						 if (score > max_score)
						 {
							 max_score = score;
							 best_alphabet = random;
							 if (max_score > best_local_max_score)
							 {
								 best_local_max_score = max_score;
								 best_local_max_alphabet = best_alphabet;
								 stuck_count_ = 0;
								 long int secs = time(0) - start_time;
								 long int mins = secs / 60;
								 secs -= 60 * mins; 
								 if (_verbose)
								 {
									 std::cout << _scorer.get_scored_count() << " alphabets in ";
									 std::cout << mins << " minutes, " << secs << " seconds" << std::endl;
									 _mono.display();
									 std::cout << "Score = " << best_local_max_score << std::endl;
									 std::cout << outtext << std::endl;   
									 score_ = best_local_max_score;
								 }
			                     if (_user_callback)
								 {
				                    _user_callback(_user_callback_object, *this);
								 }
							 } 
						 }

						 if (top1.qualifies(score))
						 {
							 top1.add(random, score);
						 }
						 // keep track of the absolute best scorers.
						 if (_top.qualifies(score))
						 {
							 _top.add(random, score);
						 }
						 random.next_swap(_fixed);
					 } 
					 if (max_score > local_max_score)
					 { 
						 local_max_score = max_score;
						 local_max_alphabet = best_alphabet;
						 random = best_alphabet;
						 _mono.set_key(random.alphabet());
					 }
					 else
                     {
						stuck_count_++;
					    local_max_found = 1;
					 }
				 }
			 }
			 if (_max_iterations > 0 && _scorer.get_scored_count() > _max_iterations) 
			 {
				 if (_verbose)
				 {
				    long int secs = time(0) - start_time;
				    long int mins = secs / 60;
				    secs -= 60 * mins; 
					std::cout << _scorer.get_scored_count() << " alphabets in ";
					std::cout << mins << " minutes, " << secs << " seconds" << std::endl;
				    _mono.set_key(best_local_max_alphabet.alphabet());
				    _mono.decode(_ciphertext, outtext);
				    _mono.display();
					std::cout << "Score = " << best_local_max_score << std::endl;
					std::cout << outtext << std::endl;
				 }
				 delete [] outtext;
				 return 0;
			 }
			 if (_user_callback)
			 {
				 _user_callback(_user_callback_object, *this);
			 }
			 if (_terminate) 
			 {
				 delete [] outtext;
				 clear();
				 return 0;
			 }
			 if (top == top1)
			 {
				 top.clear();
		         i = 0;
		         while (i < _max_top)
				 {
					random = best_local_max_alphabet;
				    random.randomise1(_fixed, stuck_count_);
				    _mono.set_key(random.alphabet());
				    _mono.decode(_ciphertext, outtext);
				    score = _scorer.score(outtext);
			        if (top.qualifies(score))
					{
				       top.add(random, score);
   				       i++;
					}
				 }
			 }
			 else
			 {
				 top = top1;
			 }
		 }
		 delete [] outtext;
		 return 0;
      }

	  int keys_checked() const
	  {
		  return _scorer.get_scored_count();
	  }

	  void set_cipher_text(const char* ciphertext)
	  {
		  if (_ciphertext) delete [] _ciphertext;
		  _ciphertext = new char [ strlen(ciphertext) + 1 ];
		  const char* c = ciphertext;
		  char* d = _ciphertext;
		  int space_last = 0;
		  while (c && *c)
		  {
			  if (isalpha(*c))	   
			  {
				  *d++ = toupper(*c);
				  space_last = 0;
			  }
			  else if (_scorer.spaces_scored() && !space_last && *c == ' ')
			  {
				  *d++ = 'z' + 1; // we represent space as 'z' + 1
                  space_last = 1;
			  }
			  c++;
		  }
		  *d = '\0';
		  if (_outtext) delete [] _outtext;
		  _outtext = new char [ strlen(_ciphertext) + 1 ];
	  }

	  int is_cipher_text_different(const char* new_ciphertext)
	  {
		  if (!_ciphertext) return 1;
		  const char* d = new_ciphertext;
		  char* c = _ciphertext;
		  while (d && *d)	   
		  {
			  if (isalpha(*d))
			  {
				  if (toupper(*d) != *c) return 1;
				  c++;
			  }
			  d++;
		  }
		  return 0;
	  }

	  void set_verbose() 
	  {
		  _verbose = 1;
	  }
	  void set_quiet()
	  {
		  _verbose = 0;
	  }
	  void set_max_iterations(int max_iterations)
	  {
		  _max_iterations = max_iterations;
		  if (_max_iterations < 0) _max_iterations = 0;
	  }
	  void display_best(int index = 0)
	  {
		  if (index > 9) index = 9;
		  if (index < 0) index = 0;
		  Alphabet best = _top[index];
          Mono m;
		  m.set_key(best.alphabet());
		  m.decode(_ciphertext, _outtext);
		  int score = _top.score(index);
          m.display();
		  std::cout << "Score = " << score << std::endl;
		  std::cout << _outtext << std::endl;   
	  }
	  const char* best_result(int index = 0) const
	  {
		  if (!_ciphertext) return 0;
		  if (index > 9) index = 9;
		  if (index < 0) index = 0;
		  Alphabet best = _top[index];
          Mono m;
		  m.set_key(best.alphabet());
		  m.decode(_ciphertext, _outtext);
		  int score = _top.score(index); 
		  return _outtext;
	  }
	  const char* best_key(int index = 0) const
	  {
		  if (index > 9) index = 9;
		  if (index < 0) index = 0;
		  Alphabet best = _top[index];
          Mono m;
		  m.set_key(best.alphabet());
          return m.get_key();
	  }
	  void set_terminate()
	  {
		  // set flag for termination
		  _terminate = 1;
	  }
	  int get_terminate() const
	  {
		  return _terminate;
	  }
	  static double phi(const char* cipher)
	  {
		 int freq[26];
		 int i = 0;
         for (i = 0; i < 26; i++) freq[i] = 0;
		 const char* c = cipher;
		 int total = 0;
		 while (c && *c)
		 {
			 if (isalpha(*c))
			 {
				 freq[tolower(*c) - 'a']++;
				 total++;
			 }
			 c++;
		 }
		 if (total == 0) return 0.0;
		 double phi_value = 0.0;
		 for (i = 0; i < 26; i++)
		 {
            phi_value += freq[i] * (freq[i] - 1);
		 }
		 phi_value /= total * (total - 1);
		 return phi_value;
	  }
	  static char* reformat(const char* plain, const char* cipher)
	  {
         static char* buffer = 0;
         static int buffer_len = 0;
         if (strlen(cipher) > buffer_len)
		 {
            delete [] buffer;
            buffer_len = strlen(cipher);
            buffer = new char [ buffer_len + 1 ];
		 }

         const char* c = cipher;
         const char* o = plain;
         char* b = buffer;
   
         while (c && *c)
		 {
            if (isalpha(*c))
			{
               //if (isupper(*c)) *b = toupper(*o);
               //if (islower(*c)) *b = tolower(*o);
			   *b = tolower(*o);
               o++;
			}
            else
			{
               *b = *c;
			}
            b++;
            c++;
		 }
         *b = '\0';
         return buffer;
	  }
      int stuck() const { return stuck_count_; }
	  int score() const { return score_; }

   private:
	  void clear()
	  {
		  _fixed = 0;
		  _max_top = 1;
		  _max_iterations = 0;
		  delete [] _ciphertext;
		  _ciphertext = 0;
		  _verbose = 0;
	  }
      Scorer _scorer;
      Mono _mono;
      Top _top;
	  Fixed_Key* _fixed;
      int _max_top;
      int _max_iterations;
	  int _verbose;
	  int _terminate;
	  int stuck_count_;
	  int score_;
	  USER_CALLBACK _user_callback;
	  void* _user_callback_object;
      char* _ciphertext;
	  char* _outtext;
};
#endif
