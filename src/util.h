#ifndef UTIL_H
#define UTIL_H
#include <vector>
#include <string>
#include <istream>
inline std::vector<std::string> SplitString(const std::string &input_str,const std::string&delimiter){
    std::string line;
    std::vector<std::string> lines;
    size_t pos = 0;
    std::string token;
    std::string str = input_str;
    while ((pos = str.find(delimiter)) != std::string::npos) {
        token = str.substr(0, pos);
        lines.push_back(token);
        str.erase(0, pos + delimiter.length());
    }
    lines.push_back(input_str);
    return lines
}


#endif
