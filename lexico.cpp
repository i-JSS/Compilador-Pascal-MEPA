#include <cctype>
#include <cstdio>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

enum TokenCode {
  // Tokens especiais 0xx:
  TOKEN_EMPTY = 0,  // 0 - (Empty token)
  TOKEN_ERROR,      // 1 - (Error token)
  TOKEN_IDENTIFIER, // 2 - (Identifier token)
  TOKEN_NUMBER,     // 3 - (Number token)
  TOKEN_EOF,        // 4 - (Token for end of file)

  // Simbolos Especiais 1xx:
  TOKEN_DOT = 100,  // 100 - .
  TOKEN_COLON,      // 101 - :
  TOKEN_COMMA,      // 102 - ,
  TOKEN_LPAREN,     // 103 - (
  TOKEN_RPAREN,     // 104 - )
  TOKEN_EQUAL,      // 105 - =
  TOKEN_LT,         // 106 - <
  TOKEN_GT,         // 107 - >
  TOKEN_PLUS,       // 108 - +
  TOKEN_MINUS,      // 109 - -
  TOKEN_STAR,       // 110 - *
  TOKEN_LBRACKET,   // 111 - [
  TOKEN_RBRACKET,   // 112 - ]
  TOKEN_SEMICOLON,  // 113 - ;
  TOKEN_APOSTROPHE, // 114 - '

  // Simbolos Especiais Compostos 2xx:
  TOKEN_ASSIGNMENT = 200, // 200 - :=
  TOKEN_SUBRANGE,         // 201 - ..
  TOKEN_LCOMMENT,         // 202 - (*
  TOKEN_RCOMMENT,         // 203 - *)

  // Palavras-chave 3xx:
  TOKEN_PROGRAM = 300, // 301 - program
  TOKEN_LABEL,         // 302 - label
  TOKEN_TYPE,          // 303 - type
  TOKEN_ARRAY,         // 304 - array
  TOKEN_OF,            // 305 - of
  TOKEN_VAR,           // 306 - var
  TOKEN_PROCEDURE,     // 307 - procedure
  TOKEN_FUNCTION,      // 308 - function
  TOKEN_BEGIN,         // 309 - begin
  TOKEN_END,           // 310 - end
  TOKEN_IF,            // 311 - if
  TOKEN_THEN,          // 312 - then
  TOKEN_ELSE,          // 313 - else
  TOKEN_WHILE,         // 314 - while
  TOKEN_DO,            // 315 - do
  TOKEN_OR,            // 316 - or
  TOKEN_DIV,           // 317 - div
  TOKEN_NOT            // 318 - not
};

bool isIdentifier(TokenCode token_code) {
  return token_code == TOKEN_IDENTIFIER;
}

bool isNumber(TokenCode token_code) { return token_code == TOKEN_NUMBER; }

bool isSpecialSymbol(TokenCode token_code) {
  return token_code < 200 && token_code >= 100;
}

bool isCompoundSymbol(TokenCode token_code) {
  return token_code < 300 && token_code >= 200;
}

bool isKeyword(TokenCode token_code) {
  return token_code < 400 && token_code >= 300;
}

struct token {
  std::string content;
  int code;
};

const std::unordered_map<std::string, TokenCode> simbolos_especiais = {
    {".", TOKEN_DOT},       {":", TOKEN_COLON},       {",", TOKEN_COMMA},
    {"(", TOKEN_LPAREN},    {")", TOKEN_RPAREN},      {"=", TOKEN_EQUAL},
    {"<", TOKEN_LT},        {">", TOKEN_GT},          {"+", TOKEN_PLUS},
    {"-", TOKEN_MINUS},     {"*", TOKEN_STAR},        {"[", TOKEN_LBRACKET},
    {"]", TOKEN_RBRACKET},  {":=", TOKEN_ASSIGNMENT}, {"..", TOKEN_SUBRANGE},
    {"(*", TOKEN_LCOMMENT}, {"*)", TOKEN_RCOMMENT},   {";", TOKEN_SEMICOLON},
    {"'", TOKEN_APOSTROPHE}};

const std::unordered_map<std::string, TokenCode> palavras_chave = {
    {"program", TOKEN_PROGRAM},
    {"label", TOKEN_LABEL},
    {"type", TOKEN_TYPE},
    {"array", TOKEN_ARRAY},
    {"of", TOKEN_OF},
    {"var", TOKEN_VAR},
    {"procedure", TOKEN_PROCEDURE},
    {"function", TOKEN_FUNCTION},
    {"begin", TOKEN_BEGIN},
    {"end", TOKEN_END},
    {"if", TOKEN_IF},
    {"then", TOKEN_THEN},
    {"else", TOKEN_ELSE},
    {"while", TOKEN_WHILE},
    {"do", TOKEN_DO},
    {"or", TOKEN_OR},
    {"div", TOKEN_DIV},
    {"not", TOKEN_NOT}};

token analisador_lexico(std::string::iterator &prox,
                        const std::string::iterator &end) {
  std::string atom;
  for (; prox != end && *prox == ' '; prox++)
    ;

  if (prox == end)
    return {"#", TOKEN_EOF};

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

  if (isalpha(*prox)) {
    while (prox != end && (isalpha(*prox) || isdigit(*prox)))
      atom.push_back(*prox++);
    if (palavras_chave.find(atom) != palavras_chave.end())
      return {atom, palavras_chave.find(atom)->second};
    else
      return {atom, TOKEN_IDENTIFIER};
  }

  if (isdigit(*prox)) {
    while (prox != end && isdigit(*prox))
      atom.push_back(*prox++);
    if (prox != end && isalpha(*prox))
      return {"Letra em meio de numero.", TOKEN_ERROR};
    else
      return {atom, TOKEN_NUMBER};
  }
  return {"Token inesperado.", TOKEN_ERROR};
}

std::vector<token> getTokens(std::string source_code) {
  auto it = source_code.begin();
  std::vector<token> tokens;

  do {
    tokens.push_back(analisador_lexico(it, source_code.end()));
  } while (tokens.back().code != TOKEN_EOF);
  return tokens;
}

std::string read_source_file() {
  std::string buffer;
  std::string source_code;
  while (std::getline(std::cin, buffer))
    source_code += buffer;

  return source_code;
}

int main() {
  std::string source_code = read_source_file();
  std::vector<token> tokens = getTokens(source_code);

  for (token &t : tokens) {
    std::cout << t.code << " - " << t.content << '\n';
  }
}
