#ifndef __CRIB_H
#define __CRIB_H

#include "fixed.h"
#include <string.h>

class Crib
{
public:
	Crib() : _ct(0), _crib(0), _pt(0), _position(0), _possible(0), _original_ct(0) {}
	Crib(const char* ct, const char* crib, int position = 0)
		: _ct(0), _crib(0), _pt(0), _position(position), _possible(0), _original_ct(0)
	{ 
		if (ct)
		{
		   _ct = new char [ strlen(ct) + 1 ];
		   _original_ct = new char [ strlen(ct) + 1];
		   copy_characters_upper(_ct, ct);
		   strcpy(_original_ct, ct);
		   _pt = new char [ strlen(ct) + 1 ];
		   if (crib)
		   {
		      _crib = new char [ strlen(crib) + 1 ];
		      copy_characters_lower(_crib, crib);
		   }
		   place_at(position);
		}
	}
	Crib(const Crib& c) : _ct(0), _crib(0), _pt(0), _position(0), _possible(0)
	{
		if (c._ct)
		{
		   _ct = new char [ strlen(c._ct) + 1 ];
		   _original_ct = new char [ strlen(c._original_ct) + 1];
		   strcpy(_ct, c._ct);
		   strcpy(_original_ct, c._original_ct);
		   _pt = new char [ strlen(_ct) + 1 ];
		   if (c._crib)
		   {
		      _crib = new char [ strlen(c._crib) + 1 ];
		      strcpy(_crib, c._crib);
		   }
		   place_at(c._position);
		}
	}
	~Crib()
	{
		delete [] _ct;
		delete [] _original_ct;
		delete [] _pt;
		delete [] _crib;
	}
	Crib& operator=(const Crib& c)
	{
		if (this == &c) return *this;
		delete [] _ct;
		delete [] _original_ct;
		delete [] _pt;
		delete [] _crib;
		if (c._ct)
		{
		   _ct = new char [ strlen(c._ct) + 1 ];
		   strcpy(_ct, c._ct);
		   _original_ct = new char [ strlen(c._original_ct) + 1 ];
		   strcpy(_original_ct, c._original_ct);
		   _pt = new char [ strlen(_ct) + 1 ];
		   if (c._crib)
		   {
		      _crib = new char [ strlen(c._crib) + 1 ];
		      strcpy(_crib, c._crib);
		   }
		   place_at(c._position);
		}
		else
		{
			_ct = 0;
			_original_ct = 0;
			_pt = 0;
			_crib = 0;
			_position = 0;
			_possible = 0;
			_fixed_key.clear();
		}
        return *this;
	}
    const char* get_ct() const { return _ct; }
	int is_possible() { return _possible; }
	void move_left() 
	{
		if (_position <= 0) return;
		place_at(_position - 1);
	}
	void move_right()
	{
		if (!_crib) return;
		if (strlen(_crib) + _position + 1 > strlen(_ct)) return;
		place_at(_position + 1);
	}
	void next_left()
	{
		int pos = _position;
		move_left();
		while (!_possible && _position > 0)
		{
			move_left();
		}
		if (!_possible) place_at(pos);
	}
	void next_right()
	{
		if (!_crib) return;
		int pos = _position;
		move_right();
		while (!_possible && strlen(_crib) + _position < strlen(_ct))
		{
			move_right();
		}
		if (!_possible) place_at(pos);
	}
	void clear()
	{
		_crib[0] = '\0';
		_position = 0;
		_possible = 0;
		_fixed_key.clear();
	}
	char* get_pt()
	{
		char* c = _ct;
		char* d = _pt;
		while (c && *c)
		{
			*d = _fixed_key.get_pt(*c);
			if (*d == ' ') *d = '.';
			c++;
			d++;
		}
		*d = '\0';
		return _pt;
	}
	const char* get_crib_string() const { return _crib; }
	Fixed_Key get_fixed_key() { return _fixed_key; }
	int ct_has_changed(const char* ct)
	{
		if (ct && _original_ct && strcmp(ct, _original_ct) != 0) return 1;
		if ((!ct || !_original_ct) && ct != _original_ct) return 1;
		return 0;
	}
	const char* get_original_ct() const { return _original_ct; }
    int get_position() const { return _position; }
	void place_at(int position)
	{
		if (!_crib) return;
		if (position + strlen(_crib) > strlen(_ct)) return;
		if (position < 0) return;
		_possible = 0;
		_position = position;
		_fixed_key.clear();
		char* c = _ct + position;
		char* d = _crib;
		while (d && *d)
		{
			if ((_fixed_key.get_ct(*d) != ' ' ||
				_fixed_key.get_pt(*c) != ' ') && 
				*c != _fixed_key.get_ct(*d))
			{
				return;
			}
			// associate *c ct with *d pt
			_fixed_key.set(*d, *c);
			c++;
			d++;
		}
		_possible = 1;
	}
private:
	char* _ct;
	char* _crib;
	char* _pt;
	int _position;
	int _possible;
	char* _original_ct;
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
	Fixed_Key _fixed_key;
};
#endif