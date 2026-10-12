#include <cstdio>
#include <string>
#include <vector>

#include "bsp_readfile.h"

int main(int argc, char* argv[]) {
  std::vector<std::string> args;
  args.resize(argc);

  for (int i = 0; i < argc; i++) args[i] = argv[i];

  readfile(std::filesystem::path(args[1]));

  return 0;
}