#ifndef L_STD_TYPE_DEFS_H
#define L_STD_TYPE_DEFS_H

#include <string>
#include <vector>

#ifndef int8
#	define int8 signed char
#endif

#ifndef uint8
#	define uint8 unsigned char
#endif

#ifndef int16
#	define	int16 short
#endif

#ifndef uint16
#	define	uint16 unsigned short
#endif

#ifndef int32
#	define int32 long
#endif

#ifndef uint32
#	define	uint32	unsigned long
#endif

#ifndef uint64
#	define uint64	unsigned long long
#endif

#ifndef int64
#	define int64 long long
#endif

#ifndef snooze
#	define snooze(X)	usleep( X )
#endif

/*
	In an effort to be portable, we can't use BString and BStringList.
*/


// Haiku lacks this, this may need to be #ifdef'd out for other OSes
// or for when Haiku gains this...
std::string to_string(int32 i);

typedef std::vector<std::string> LStringList;
typedef std::string LString;


#endif // L_STD_TYPE_DEFS_H

