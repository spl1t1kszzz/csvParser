#include "Exceptions.h"

int Exceptions::returnCode() const {
    return this->code;
}

std::string Exceptions::whatReason() const  {
    return this->what;
}