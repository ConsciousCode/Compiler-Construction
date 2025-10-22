#include "emitter.hpp"
#include <sstream>

void Emitter::line(const std::string& s) {
    buf += s;
    buf += '\n';
}

void Emitter::label(const std::string& L) {
    buf += L;
    buf += ":\n";
}

std::string Emitter::gensym(const std::string& base) {
    std::ostringstream os;
    os << base << "_" << counter++;
    return os.str();
}