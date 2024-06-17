#include <cctype>
#include <clocale>
#include <cwctype>
#include <iostream>
#include <string>
int main(int argc, char *argv[]) {
  setlocale(LC_ALL, "");
  std::u32string str = "á";
  std::cout << (int)str << std::endl;
  std::cout << iswalpha(str) << std::endl;
  std::cout << str << std::endl;
  return 0;
}
