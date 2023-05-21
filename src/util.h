#ifndef UTIL_H
#define UTIL_H
#include <istream>
#include <string>
#include <vector>
inline std::vector<std::string> SplitString(const std::string &input_str,
                                            const std::string &delimiter) {
  std::vector<std::string> result;
  size_t start_pos = 0;
  size_t end_pos = input_str.find(delimiter);

  while (end_pos != std::string::npos) {
    // Extract the substring between start_pos and end_pos
    std::string token = input_str.substr(start_pos, end_pos - start_pos);
    result.push_back(token);

    // Move start_pos to the next character after the delimiter
    start_pos = end_pos + delimiter.length();

    // Find the next delimiter in the input string
    end_pos = input_str.find(delimiter, start_pos);
  }

  // Add the last substring after the last delimiter (or the whole string if no
  // delimiter found)
  std::string last_token = input_str.substr(start_pos);
  result.push_back(last_token);

  return result;
}
#endif
