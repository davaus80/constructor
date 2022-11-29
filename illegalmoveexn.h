#ifndef _ILLEGALMOVEEXN_H_
#define _ILLEGALMOVEEXN_H_
#include <string>


// IllegalMoveExn is a class used as a custm exception.
// It carries a message and is generally used to indicate an illegal move.
class IllegalMoveExn {
	std::string message;
       public:
	IllegalMoveExn(std::string message);
	std::string getMessage();
};

#endif
