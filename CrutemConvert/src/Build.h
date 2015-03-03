#ifndef CC_BUILD_H
#define CC_BUILD_H

#include <string>


#define DEBUG 1

#define	_PFUN_	__PRETTY_FUNCTION__


#if	DEBUG
#	define SERROR( X ) (std::string( X ).append("->").append(_PFUN_))
#	define DERROR( X ) fprintf(stderr, "ERROR in %s : \"%s\"\n", _PFUN_, X )
#else
#	define SERROR( X ) X
#	define DERROR( X )
#endif

#define ERROR( X ) fprintf(stderr, "ERROR in %s : \"%s\"\n", _PFUN_, X )

#endif // CC_BUILD_H
