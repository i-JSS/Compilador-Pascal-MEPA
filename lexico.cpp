#include <cctype>
#include <iostream>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

const std::vector<std::string> tokenTypeNames = {
    "KEYWORD",           "IDENTIFIER", "NUMBER",   "OPERATOR",
    "COMPOUND OPERATOR", "DELIMITER",  "COMMENTS", "UNKNOWN"};

enum TokenType {
  TOKENTYPE_KEYWORD,
  TOKENTYPE_IDENTIFIER,
  TOKENTYPE_NUMBER,
  TOKENTYPE_OPERATOR,
  TOKENTYPE_COMPOUND_OPERATOR,
  TOKENTYPE_DELIMITER,
  TOKENTYPE_COMMENTS,
  TOKENTYPE_UNKNOWN
};

enum TokenCode {
  // Tokens especiais 0xx:
  TOKEN_EMPTY = 0,  // 0 - (Empty token)
  TOKEN_ERROR,      // 1 - (Error token)
  TOKEN_IDENTIFIER, // 2 - (Identifier token)
  TOKEN_NUMBER,     // 3 - (Number token)
  TOKEN_EOF,        // 4 - (Token for end of file)
  TOKEN_COMMENTS,   // 5 - (Token for comments)
  TOKEN_UNKNOWN,    // 6 - (Unknown token)

  // Operadores OPERATOR:
  TOKEN_PLUS = 100, // 100 - +
  TOKEN_MINUS,      // 101 - -
  TOKEN_STAR,       // 102 - *
  TOKEN_SLASH,      // 103 - /
  TOKEN_LT,         // 104 - <
  TOKEN_GT,         // 105 - >
  TOKEN_EQUAL,      // 106 - =

  // Operadores compostos COMPOUND_OPERATOR:
  TOKEN_ASSIGNMENT = 200, // 200 - :=
  TOKEN_LE,               // 201 - <=
  TOKEN_GE,               // 202 - >=
  TOKEN_NE,               // 203 - <>

  // Delimitadores DELIMITER:
  TOKEN_LPAREN = 300, // 300 - (
  TOKEN_RPAREN,       // 301 - )
  TOKEN_LBRACKET,     // 302 - [
  TOKEN_RBRACKET,     // 303 - ]
  TOKEN_SEMICOLON,    // 304 - ;
  TOKEN_COMMA,        // 305 - ,
  TOKEN_COLON,        // 306 - :
  TOKEN_DOT,          // 307 - .
  TOKEN_APOSTROPHE,   // 308 - '
  TOKEN_QUOTES,       // 309 - "

  // Palavras-chave KEYWORD:
  TOKEN_AND = 400, // 400 - and
  TOKEN_ARRAY,     // 401 - array
  TOKEN_BEGIN,     // 402 - begin
  TOKEN_DIV,       // 403 - div
  TOKEN_DO,        // 404 - do
  TOKEN_ELSE,      // 405 - else
  TOKEN_END,       // 406 - end
  TOKEN_FUNCTION,  // 407 - function
  TOKEN_GOTO,      // 408 - goto
  TOKEN_IF,        // 409 - if
  TOKEN_LABEL,     // 410 - label
  TOKEN_NOT,       // 411 - not
  TOKEN_OF,        // 412 - of
  TOKEN_OR,        // 413 - or
  TOKEN_PROCEDURE, // 414 - procedure
  TOKEN_PROGRAM,   // 415 - program
  TOKEN_THEN,      // 416 - then
  TOKEN_TYPE,      // 417 - type
  TOKEN_VAR,       // 418 - var
  TOKEN_WHILE,     // 419 - while

  TOKEN_READ, // 420 - read
  TOKEN_WRITE // 421 - write
};

struct token {
  std::string content;
  TokenCode code;
};

TokenType getTokenType(TokenCode code) {
  if (code == TOKEN_IDENTIFIER)
    return TOKENTYPE_IDENTIFIER;
  if (code == TOKEN_NUMBER)
    return TOKENTYPE_NUMBER;
  if (code == TOKEN_COMMENTS)
    return TOKENTYPE_COMMENTS;
  if (code == TOKEN_UNKNOWN)
    return TOKENTYPE_UNKNOWN;
  if (code >= 400)
    return TOKENTYPE_KEYWORD;
  if (code >= 300 && code < 400)
    return TOKENTYPE_DELIMITER;
  if (code >= 200 && code < 300)
    return TOKENTYPE_COMPOUND_OPERATOR;
  if (code >= 100 && code < 200)
    return TOKENTYPE_OPERATOR;
  return TOKENTYPE_UNKNOWN;
}

const std::unordered_map<std::string, TokenCode> simbolos_especiais = {
    {".", TOKEN_DOT},      {":", TOKEN_COLON},       {",", TOKEN_COMMA},
    {"(", TOKEN_LPAREN},   {")", TOKEN_RPAREN},      {"=", TOKEN_EQUAL},
    {"<", TOKEN_LT},       {">", TOKEN_GT},          {"+", TOKEN_PLUS},
    {"-", TOKEN_MINUS},    {"*", TOKEN_STAR},        {"[", TOKEN_LBRACKET},
    {"]", TOKEN_RBRACKET}, {":=", TOKEN_ASSIGNMENT}, {"<=", TOKEN_LE},
    {">=", TOKEN_GE},      {"<>", TOKEN_NE},         {";", TOKEN_SEMICOLON},
    {"/", TOKEN_SLASH}};

const std::unordered_map<std::string, TokenCode> palavras_chave = {
    {"program", TOKEN_PROGRAM},
    {"label", TOKEN_LABEL},
    {"type", TOKEN_TYPE},
    {"array", TOKEN_ARRAY},
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
    {"div", TOKEN_DIV},
    {"and", TOKEN_AND},
    {"goto", TOKEN_GOTO},
    {"read", TOKEN_READ},
    {"write", TOKEN_WRITE},
    {"not", TOKEN_NOT},
    {"of", TOKEN_OF},
    {"or", TOKEN_OR}};

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
    } else if (s == "<" && *prox == '>') {
      s = "<>";
      prox++;
    } else if (s == "<" && *prox == '=') {
      s = "<=";
      prox++;
    } else if (s == ">" && *prox == '=') {
      s = ">=";
      prox++;
    } else if (s == "(" && *prox == '*') {
      prox++;
      while (prox != end) {
        if (*prox == '*' && (prox + 1) != end && *(prox + 1) == ')') {
          prox += 2;
          return {atom, TOKEN_COMMENTS};
        }
        prox++;
      }
      return {"Fechamento de comentario não identificado", TOKEN_ERROR};
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
  prox++;
  return {s, TOKEN_UNKNOWN};
}

std::vector<token> getTokens(std::string source_code) {
  auto it = source_code.begin();
  std::vector<token> tokens;

  do {
    tokens.push_back(analisador_lexico(it, source_code.end()));
    if (tokens.back().code == TOKEN_ERROR) {
      std::cerr << tokens.back().content << '\n';
      exit(1);
    }
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

std::map<TokenType, int> countTokenTypes(const std::vector<token> &tokens) {
  std::map<TokenType, int> counts = {{TOKENTYPE_KEYWORD, 0},
                                     {TOKENTYPE_IDENTIFIER, 0},
                                     {TOKENTYPE_NUMBER, 0},
                                     {TOKENTYPE_OPERATOR, 0},
                                     {TOKENTYPE_COMPOUND_OPERATOR, 0},
                                     {TOKENTYPE_DELIMITER, 0},
                                     {TOKENTYPE_COMMENTS, 0},
                                     {TOKENTYPE_UNKNOWN, 0}};
  for (const token &t : tokens) {
    TokenType type = getTokenType(t.code);
    counts[type]++;
  }
  // TIRA O EOF DA CONTAGEM
  counts[TOKENTYPE_UNKNOWN]--;
  return counts;
}

int main() {
  std::string source_code = read_source_file();
  std::vector<token> tokens = getTokens(source_code);

  auto counts = countTokenTypes(tokens);

  for (auto &count : counts) {
    std::string name = tokenTypeNames[count.first];
    std::cout << name << ": " << count.second << "\n";
  }
}
