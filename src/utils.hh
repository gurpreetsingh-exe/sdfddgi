#ifndef UTILS_H
#define UTILS_H

#include <string>

#define FORCE_INLINE __attribute__((always_inline))

std::string openFileBrowser();
std::string readFile(const std::string& fileName);

#endif // !UTILS_H
