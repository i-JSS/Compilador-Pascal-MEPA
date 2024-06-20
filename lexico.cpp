#include <cctype>
#include <clocale>
#include <iostream>
#include <locale>
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
  TOKEN_UNKNOWN,

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
  std::wstring content;
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

const std::unordered_map<std::wstring, TokenCode> simbolos_especiais = {
    {L".", TOKEN_DOT},      {L":", TOKEN_COLON},       {L",", TOKEN_COMMA},
    {L"(", TOKEN_LPAREN},   {L")", TOKEN_RPAREN},      {L"=", TOKEN_EQUAL},
    {L"<", TOKEN_LT},       {L">", TOKEN_GT},          {L"+", TOKEN_PLUS},
    {L"-", TOKEN_MINUS},    {L"*", TOKEN_STAR},        {L"[", TOKEN_LBRACKET},
    {L"]", TOKEN_RBRACKET}, {L":=", TOKEN_ASSIGNMENT}, {L"<=", TOKEN_LE},
    {L">=", TOKEN_GE},      {L"<>", TOKEN_NE},         {L";", TOKEN_SEMICOLON},
    {L"/", TOKEN_SLASH}};

const std::unordered_map<std::wstring, TokenCode> palavras_chave = {
    {L"program", TOKEN_PROGRAM},
    {L"label", TOKEN_LABEL},
    {L"type", TOKEN_TYPE},
    {L"array", TOKEN_ARRAY},
    {L"var", TOKEN_VAR},
    {L"procedure", TOKEN_PROCEDURE},
    {L"function", TOKEN_FUNCTION},
    {L"begin", TOKEN_BEGIN},
    {L"end", TOKEN_END},
    {L"if", TOKEN_IF},
    {L"then", TOKEN_THEN},
    {L"else", TOKEN_ELSE},
    {L"while", TOKEN_WHILE},
    {L"do", TOKEN_DO},
    {L"div", TOKEN_DIV},
    {L"and", TOKEN_AND},
    {L"goto", TOKEN_GOTO},
    {L"read", TOKEN_READ},
    {L"write", TOKEN_WRITE},
    {L"not", TOKEN_NOT},
    {L"of", TOKEN_OF},
    {L"or", TOKEN_OR}};

token analisador_lexico(std::wstring::iterator &prox,
                        const std::wstring::iterator &end) {
  std::wstring atom;
  std::locale loc("en_US.UTF8");
  for (; prox != end && *prox == ' '; prox++)
    ;

  if (prox == end)
    return {L"#", TOKEN_EOF};

  std::wstring s(1, *prox);
  if (simbolos_especiais.find(s) != simbolos_especiais.end()) {
    prox++;
    if (s == L":" && *prox == '=') {
      s = L":=";
      prox++;
    } else if (s == L"<" && *prox == '>') {
      s = L"<>";
      prox++;
    } else if (s == L"<" && *prox == '=') {
      s = L"<=";
      prox++;
    } else if (s == L">" && *prox == '=') {
      s = L">=";
      prox++;
    } else if (s == L"(" && *prox == '*') {
      prox++;
      while (prox != end) {
        if (*prox == '*' && (prox + 1) != end && *(prox + 1) == ')') {
          prox += 2;
          return {atom, TOKEN_COMMENTS};
        }
        prox++;
      }
      return {L"Fechamento de comentario não identificado", TOKEN_ERROR};
    }
    return {s, simbolos_especiais.find(s)->second};
  }

  if (std::isalpha(*prox, loc)) {
    while (prox != end && (isalnum(*prox, loc)))
      atom.push_back(*prox++);
    if (palavras_chave.find(atom) != palavras_chave.end())
      return {atom, palavras_chave.find(atom)->second};
    else
      return {atom, TOKEN_IDENTIFIER};
  }

  if (isdigit(*prox, loc)) {
    while (prox != end && isdigit(*prox, loc))
      atom.push_back(*prox++);
    if (prox != end && isalpha(*prox, loc))
      return {L"Letra em meio de numero.", TOKEN_ERROR};
    else
      return {atom, TOKEN_NUMBER};
  }
  prox++;
  return {s, TOKEN_UNKNOWN};
}

std::vector<token> getTokens(std::wstring source_code) {
  auto it = source_code.begin();
  std::vector<token> tokens;

  do {
    tokens.push_back(analisador_lexico(it, source_code.end()));
    if (tokens.back().code == TOKEN_ERROR) {
      std::wcerr << tokens.back().content << '\n';
      exit(1);
    }
  } while (tokens.back().code != TOKEN_EOF);
  return tokens;
}

std::wstring read_source_file() {
  std::wstring buffer;
  std::wstring source_code;
  while (std::getline(std::wcin, buffer))
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
  setlocale(LC_ALL, "");
  std::wstring source_code = read_source_file();
  std::vector<token> tokens = getTokens(source_code);

  auto counts = countTokenTypes(tokens);

  for (auto &count : counts) {
    std::string name = tokenTypeNames[count.first];
    std::cout << name << ": " << count.second << "\n";
  }
}
