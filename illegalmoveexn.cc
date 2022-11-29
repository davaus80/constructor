#include <string>
#include "illegalmoveexn.h"

IllegalMoveExn::IllegalMoveExn(std::string message) 
	: message{message} {}

std::string IllegalMoveExn::getMessage() { return message; }

