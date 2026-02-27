#ifndef _SCORER_H
#define _SCORER_H

#include <map>
#include "hashtable.h"
#include <iostream>
#include <fstream>
#include <cctype>
#include <cstdlib>
#include "N_Graph.h"

class Scorer
{
   public:
      Scorer() : trigram_filename(0), trigram_count(0),
            tetragram_filename(0), tetragram_count(0),
            ngraph_filename(0), ngraph_count(0), prev_plaintext(0),
            prev_ngraph_score(0), scored_count_(0),
            ngraph_table(0), spaces_scored_(0),
            trigram_spaces_scored_(0), tetragram_spaces_scored_(0),
            ngraph_spaces_scored_(0)
      {
      }
      ~Scorer()
      {
         if (trigram_filename) delete trigram_filename;
         if (tetragram_filename) delete tetragram_filename;
         if (ngraph_filename) delete ngraph_filename;
         if (ngraph_table) delete ngraph_table;
         if (prev_plaintext) delete prev_plaintext;
         if (prev_ngraph_score) delete [] prev_ngraph_score;
      }

      int score(char* plaintext)
      {
         if (scoring_type == TRIGRAPH_SCORING) return score_trigram(plaintext);
         else if (scoring_type == TETRAGRAPH_SCORING) return score_tetragram(plaintext);
         else return score_ngraph(plaintext);
      }

      long int get_scored_count() const
      {
         return scored_count_;
      }

      int spaces_scored() const
      {
         return spaces_scored_;
      }

      void reset()
      {
         scored_count_ = 0;
         if (prev_plaintext) delete prev_plaintext;
         if (prev_ngraph_score) delete [] prev_ngraph_score;
         prev_plaintext = 0;
         prev_ngraph_score = 0;
      }

      void set_trigram_file(const char* filename)
      {
         if (trigram_filename && strcmp(filename, trigram_filename) == 0)
         {
            if (scoring_type != TRIGRAPH_SCORING)
            {
               scoring_type = TRIGRAPH_SCORING;
               scored_count_ = 0;
               spaces_scored_ = trigram_spaces_scored_;
            }
            return;
         }
         if (trigram_filename) delete trigram_filename;
         trigram_filename = new char [ strlen(filename) + 1 ];
         strcpy(trigram_filename, filename);
         spaces_scored_ = 0;

         std::fstream file(trigram_filename, std::ios::in);

         char line[132];
         memset((char*)trigram_table, 0, sizeof(trigram_table));

         while (file.getline(line, 132))
         {
            // format is XXX : NNNNN PCT
            char c1 = tolower(line[0]);
            if (c1 == 'z' + 1) spaces_scored_ = 1;
            char c2 = tolower(line[1]);
            char c3 = tolower(line[2]);
            char* c = strchr(line, ':');
            c += 2;
            while (c && *c && isdigit(*c)) c++;
            if (c) c++;
            if (c)
            {
               trigram_table[c1-'a'][c2-'a'][c3-'a'] = std::atof(c);
            }
            else trigram_table[c1-'a'][c2-'a'][c3-'a'] = 0.0;
            trigram_count++;
         }
         scoring_type = TRIGRAPH_SCORING;
         scored_count_ = 0;
         trigram_spaces_scored_ = spaces_scored_;
      }

      void set_tetragram_file(const char* filename)
      {
         if (tetragram_filename && strcmp(filename, tetragram_filename) == 0)
         {
            if (scoring_type != TETRAGRAPH_SCORING)
            {
               scoring_type = TETRAGRAPH_SCORING;
               scored_count_ = 0;
               spaces_scored_ = tetragram_spaces_scored_;
            }
            return;
         }
         if (tetragram_filename) delete tetragram_filename;
         tetragram_filename = new char [ strlen(filename) + 1 ];
         strcpy(tetragram_filename, filename);
         spaces_scored_ = 0;

         std::fstream file(tetragram_filename, std::ios::in);

         char line[132];

         memset((char*)tetragram_table, 0, sizeof(tetragram_table));
         while (file.getline(line, 132))
         {
            // format is XXXX : NNNNN PCT
            char* c = strchr(line, ':');
            c += 2;
            while (c && *c && isdigit(*c)) c++;
            if (c) c++;
            if (c)
            {
               tetragram_table[tolower(line[0])-'a'][tolower(line[1])-'a']
               [tolower(line[2])-'a'][tolower(line[3])-'a'] = std::atof(c);
               if (!spaces_scored_ && line[0] == 'z' + 1) spaces_scored_ = 1;
            }

            tetragram_count++;
         }
         scoring_type = TETRAGRAPH_SCORING;
         scored_count_ = 0;
         tetragram_spaces_scored_ = spaces_scored_;
      }
      void set_ngraph_file(const char* filename)
      {
         //    set_tetragram_file(filename);
         if (ngraph_filename && strcmp(filename, ngraph_filename) == 0)
         {
            if (scoring_type != NGRAPH_SCORING)
            {
               scoring_type = NGRAPH_SCORING;
               scored_count_ = 0;
               spaces_scored_ = ngraph_spaces_scored_;
            }
            return;
         }
         if (ngraph_filename)
         {
            delete ngraph_filename;
            delete ngraph_table;
         }
         ngraph_filename = new char [ strlen(filename) + 1 ];
         strcpy(ngraph_filename, filename);
         ngraph_table = new Hash_Table;
         spaces_scored_ = 0;

         std::fstream file(ngraph_filename, std::ios::in);

         char line[132];
         int first_time = 1;

         ngraph_table->clear();
         while (file.getline(line, 132))
         {
            // format is XXXXX : NNNNN PCT
            char ngraph[MAX_N];
            char* c = strchr(line, ':');
            if (c)
            {
               ngraph_length = c - line - 1;
               if (first_time)
               {
                  N_Graph::set_length(ngraph_length);
                  first_time = 0;
               }
               char* d = ngraph;
               char* e = line;
               for (int i = 0; i < ngraph_length; i++)
               {
                  *d = tolower(*e);
                  d++;
                  e++;
               }
               *d = '\0';
               if (!spaces_scored_ && ngraph[0] == 'z' + 1) spaces_scored_ = 1;
            }
            c += 2;
            while (c && *c && isdigit(*c)) c++;
            if (c) c++;
            if (c)
            {
               ngraph_table->insert(ngraph, std::atof(c));
            }

            tetragram_count++;
         }
         scoring_type = NGRAPH_SCORING;
         scored_count_ = 0;
         ngraph_spaces_scored_ = spaces_scored_;
      }

   private:
      long int scored_count_;
      enum constants {TRIGRAPH_SCORING = 3, TETRAGRAPH_SCORING = 4, NGRAPH_SCORING = 5};
      int scoring_type;
      int spaces_scored_;

      char* trigram_filename;
      int trigram_count;
      double trigram_table[27][27][27];
      int trigram_spaces_scored_;

      char* tetragram_filename;
      int tetragram_count;
      double tetragram_table[27][27][27][27];
      int tetragram_spaces_scored_;

      char* ngraph_filename;
      int ngraph_count;
      int ngraph_length;
      Hash_Table* ngraph_table;
      int ngraph_spaces_scored_;

      int score_trigram(char* plaintext)
      {
         scored_count_++;
         double score = 0.0;

         char* pos = plaintext;
         while (pos && *pos && (pos + 1) && *(pos + 1) && (pos + 2) && *(pos + 2))
         {
            score += trigram_table[*pos - 'a'][*(pos+1) -'a'][*(pos+2) - 'a'];
            pos++;
         }
         return (int)(100.0*score);
      }

      char* prev_plaintext;
      int plaintext_length;
      double prev_score;
      double* prev_ngraph_score;
      int score_ngraph(char* plaintext, int debug = 0)
      {
         double score = 0.0;
         scored_count_++;
         if (prev_plaintext == 0)
         {
            plaintext_length = strlen(plaintext);
            prev_plaintext = new char [plaintext_length + 1];
            prev_ngraph_score = new double [plaintext_length - ngraph_length + 1];
            for (int l = 0; l + ngraph_length <= plaintext_length; l++)
            {
               prev_ngraph_score[l] = (*ngraph_table)[N_Graph(plaintext + l)];
               score += prev_ngraph_score[l];
            }
         }
         else
         {
            score = prev_score;
            int l = 0;
            int next_k = 0;
            while (l < plaintext_length && next_k < plaintext_length - ngraph_length + 1)
            {
               while (plaintext[l] == prev_plaintext[l] && l < plaintext_length) l++;
               if (l < plaintext_length)
               {
                  int min_k = l - ngraph_length + 1;
                  if (min_k < next_k) min_k = next_k;
                  int max_k = l + 1;
                  if (max_k > plaintext_length - ngraph_length + 1)
                     max_k = plaintext_length - ngraph_length + 1;
                  for (int k = min_k; k < max_k; k++)
                  {
                     score -= prev_ngraph_score[k];
                     prev_ngraph_score[k] = (*ngraph_table)[N_Graph(plaintext + k)];
                     if (debug)
                     {
                        std::cout << k << " : " << N_Graph(plaintext + l) << " " << prev_ngraph_score[l] << std::endl;
                     }
                     score += prev_ngraph_score[k];
                  }
                  next_k = max_k;
                  l++;
               }
            }
         }
         strcpy(prev_plaintext, plaintext);
         prev_score = score;
         return (int)(100.0*score);
      }

      int score_tetragram(char* plaintext, int debug = 0)
      {
         scored_count_++;
         double score = 0.0;
         double s = 0.0;
         char* pos = plaintext;
         while (pos && *pos && pos+1 && *(pos+1) && pos+2 && *(pos+2) &&
                pos+3 && *(pos+3))
         {
            s = tetragram_table[*pos - 'a'][*(pos+1) - 'a']
                [*(pos+2) - 'a'][*(pos+3) - 'a'];
            if (debug)
            {
               std::cout << pos - plaintext << " : " << N_Graph(pos) << " " << s << std::endl;
            }
            if (s > 0.0) score += s;
            pos++;
         }
         return (int)(100.0 * score);
      }
};

#endif
