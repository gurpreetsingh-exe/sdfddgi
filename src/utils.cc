#include "utils.hh"

std::string openFileBrowser() {
  char filename[512];
  FILE* f = popen("zenity --file-selection", "r");
  fgets(filename, 512, f);
  auto obj = std::string(filename);
  obj[obj.size() - 1] = '\0';
  return obj;
}
