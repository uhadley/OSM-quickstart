#pragma once
#include <sstream>

struct color{
    int r;
    int g;
    int b;
    color(int r_, int g_, int b_):r{r_},g{g_},b{b_}{}
    color():r{0},g{0},b{0}{}
    std::string tostr() const{
        std::stringstream ss;
        ss << "rgb(" << r << "," << g << "," << b << ")";
        return ss.str();   
    
    }
    };