#include "utils.hh"
#include <fstream>

std::string openFileBrowser() {
  char filename[512];
  FILE* f = popen("zenity --file-selection", "r");
  fgets(filename, 512, f);
  auto obj = std::string(filename);
  obj[obj.size() - 1] = '\0';
  return obj;
}

std::string readFile(const std::string& fileName) {
  std::fstream in(fileName, std::ios::in | std::ios::binary);
  if (in) {
    std::string contents;
    in.seekg(0, std::ios::end);
    contents.resize(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(&contents[0], contents.size());
    in.close();
    return contents;
  }
  throw(errno);
}
