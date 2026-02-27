#ifndef __CRIB_H
#define __CRIB_H

#include "fixed.h"
#include <string.h>

class Crib
{
   public:
      Crib() : ct_(0), crib_(0), pt_(0), position_(0), possible_(0), original_ct_(0) {}
      Crib(const char* ct, const char* crib, int position = 0)
            : ct_(0), crib_(0), pt_(0), position_(position), possible_(0), original_ct_(0)
      {
         if (ct)
         {
            ct_ = new char [ strlen(ct) + 1 ];
            original_ct_ = new char [ strlen(ct) + 1];
            copy_characters_upper(ct_, ct);
            strcpy(original_ct_, ct);
            pt_ = new char [ strlen(ct) + 1 ];
            if (crib)
            {
               crib_ = new char [ strlen(crib) + 1 ];
               copy_characters_lower(crib_, crib);
            }
            place_at(position);
         }
      }
      Crib(const Crib& c) : ct_(0), crib_(0), pt_(0), position_(0), possible_(0)
      {
         if (c.ct_)
         {
            ct_ = new char [ strlen(c.ct_) + 1 ];
            original_ct_ = new char [ strlen(c.original_ct_) + 1];
            strcpy(ct_, c.ct_);
            strcpy(original_ct_, c.original_ct_);
            pt_ = new char [ strlen(ct_) + 1 ];
            if (c.crib_)
            {
               crib_ = new char [ strlen(c.crib_) + 1 ];
               strcpy(crib_, c.crib_);
            }
            place_at(c.position_);
         }
      }
      ~Crib()
      {
         delete [] ct_;
         delete [] original_ct_;
         delete [] pt_;
         delete [] crib_;
      }
      Crib& operator=(const Crib& c)
      {
         if (this == &c) return *this;
         delete [] ct_;
         delete [] original_ct_;
         delete [] pt_;
         delete [] crib_;
         if (c.ct_)
         {
            ct_ = new char [ strlen(c.ct_) + 1 ];
            strcpy(ct_, c.ct_);
            original_ct_ = new char [ strlen(c.original_ct_) + 1 ];
            strcpy(original_ct_, c.original_ct_);
            pt_ = new char [ strlen(ct_) + 1 ];
            if (c.crib_)
            {
               crib_ = new char [ strlen(c.crib_) + 1 ];
               strcpy(crib_, c.crib_);
            }
            place_at(c.position_);
         }
         else
         {
            ct_ = 0;
            original_ct_ = 0;
            pt_ = 0;
            crib_ = 0;
            position_ = 0;
            possible_ = 0;
            fixed_key_.clear();
         }
         return *this;
      }
      const char* get_ct() const
      {
         return ct_;
      }
      int is_possible()
      {
         return possible_;
      }
      void move_left()
      {
         if (position_ <= 0) return;
         place_at(position_ - 1);
      }
      void move_right()
      {
         if (!crib_) return;
         if (strlen(crib_) + position_ + 1 > strlen(ct_)) return;
         place_at(position_ + 1);
      }
      void next_left()
      {
         int pos = position_;
         move_left();
         while (!possible_ && position_ > 0)
         {
            move_left();
         }
         if (!possible_) place_at(pos);
      }
      void next_right()
      {
         if (!crib_) return;
         int pos = position_;
         move_right();
         while (!possible_ && strlen(crib_) + position_ < strlen(ct_))
         {
            move_right();
         }
         if (!possible_) place_at(pos);
      }
      void clear()
      {
         if (crib_)
         {
            crib_[0] = '\0';
         }
         position_ = 0;
         possible_ = 0;
         fixed_key_.clear();
      }
      char* get_pt()
      {
         char* c = ct_;
         char* d = pt_;
         while (c && *c)
         {
            *d = fixed_key_.get_pt(*c);
            if (*d == ' ') *d = '.';
            c++;
            d++;
         }
         *d = '\0';
         return pt_;
      }
      const char* get_crib_string() const
      {
         return crib_;
      }
      Fixed_Key get_fixed_key()
      {
         return fixed_key_;
      }
      int ct_has_changed(const char* ct)
      {
         if (ct && original_ct_ && strcmp(ct, original_ct_) != 0) return 1;
         if ((!ct || !original_ct_) && ct != original_ct_) return 1;
         return 0;
      }
      const char* get_original_ct() const
      {
         return original_ct_;
      }
      int get_position() const
      {
         return position_;
      }
      void place_at(int position)
      {
         if (!crib_) return;
         if (position + strlen(crib_) > strlen(ct_)) return;
         if (position < 0) return;
         possible_ = 0;
         position_ = position;
         fixed_key_.clear();
         char* c = ct_ + position;
         char* d = crib_;
         while (d && *d)
         {
            if ((fixed_key_.get_ct(*d) != ' ' ||
                  fixed_key_.get_pt(*c) != ' ') &&
                  *c != fixed_key_.get_ct(*d))
            {
               return;
            }
            // associate *c ct with *d pt
            fixed_key_.set(*d, *c);
            c++;
            d++;
         }
         possible_ = 1;
      }
   private:
      char* ct_;
      char* crib_;
      char* pt_;
      int position_;
      int possible_;
      char* original_ct_;
      static void copy_characters_upper(char* t, const char* s)
      {
         while (s && *s)
         {
            if (isalpha(*s))
            {
               *t++ = toupper(*s);
            }
            s++;
         }
         *t = '\0';
      }
      static void copy_characters_lower(char* t, const char* s)
      {
         while (s && *s)
         {
            if (isalpha(*s))
            {
               *t++ = tolower(*s);
            }
            s++;
         }
         *t = '\0';
      }
      Fixed_Key fixed_key_;
};
#endif