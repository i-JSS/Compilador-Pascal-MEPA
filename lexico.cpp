#include <cctype>
#include <string>
#include <unordered_map>
#define CODIGO_IDENTIFICADOR -1
#define CODIGO_NUMERO -2
#define CODIGO_ERRO -99

const std::unordered_map<std::string, int> simbolos_especiais = {
    {".", 1},  {":", 2},   {",", 3},   {"(", 4},   {")", 5},  {"=", 6},
    {"<", 7},  {">", 8},   {"+", 9},   {"-", 10},  {"*", 11}, {"[", 12},
    {"]", 13}, {":=", 14}, {"..", 15}, {"(*", 16}, {"*)", 17}};
const std::unordered_map<std::string, int> palavras_chave = {
    {"program", 100}, {"label", 101}, {"type", 102},      {"array", 103},
    {"of", 104},      {"var", 105},   {"procedure", 106}, {"function", 107},
    {"begin", 108},   {"end", 109},   {"if", 110},        {"then", 111},
    {"else", 112},    {"while", 113}, {"do", 114},        {"or", 115},
    {"div", 116},     {"not", 117}};

struct token {
  std::string content;
  int code;
};

int main() {}

token analisador_lexico(std::string::iterator &prox,
                        std::string::iterator &end) {
  std::string atom;

  while (prox != end && *prox == ' ')
    prox++;

  if (prox == end)
    return {"", CODIGO_ERRO};

  std::string s(1, *prox);
  if (simbolos_especiais.find(s) != simbolos_especiais.end()) {
    prox++;
    if (s == ":" && *prox == '=') {
      s = ":=";
      prox++;
    } else if (s == "." && *prox == '.') {
      s = "..";
      prox++;
    }
    return {s, simbolos_especiais.find(s)->second};
  }

  if (islower(*prox)) {
    while (prox != end && (islower(*prox) || isdigit(*prox))) {
      atom.push_back(*prox);
      prox++;
    }
    if (palavras_chave.find(atom) != palavras_chave.end()) {
      return {atom, palavras_chave.find(atom)->second};
    } else {
      return {atom, CODIGO_IDENTIFICADOR};
    }
  }

  if (isdigit(*prox)) {
    while (prox != end && isdigit(*prox)) {
      atom.push_back(*prox);
      prox++;
    }
    if (prox != end && islower(*prox)) {
      return {"", CODIGO_ERRO};
    }
    return {atom, CODIGO_NUMERO};
  }

  return {"", CODIGO_ERRO};
}
