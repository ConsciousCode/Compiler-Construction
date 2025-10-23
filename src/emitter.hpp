#pragma once
#include <string>

class Emitter {
    public:
        void line(const std::string& s, bool tab);
        void label(const std::string& L);
        std::string gensym(const std::string& base);
        const std::string& str() const {return buf;}

    private:
        std::string buf;
        long counter = 0;
};