#include <cctype>
#include <fstream>
#include <iostream>
#include <map>
#include <ostream>
#include <string>
#include <unordered_map>
#include <vector>
#define TOKENS_TO_DUMP 10

// -- DEFINIÇõES --

enum TokenType {
  TOKENTYPE_KEYWORD,
  TOKENTYPE_IDENTIFIER,
  TOKENTYPE_NUMBER,
  TOKENTYPE_OPERATOR,
  TOKENTYPE_COMPOUND_OPERATOR,
  TOKENTYPE_DELIMITER,
  TOKENTYPE_COMMENTS,
  TOKENTYPE_UNKNOWN,
  TOKENTYPE_ERROR
};

enum SymbolType {
  SYMBOLTYPE_VARIABLE,
  SYMBOLTYPE_FUNCTION,
  SYMBOLTYPE_PROCEDURE,
  SYMBOLTYPE_TYPE,
};

enum TokenCode {
  // Tokens especiais:
  TOKEN_EMPTY = 0,  // 0 - (Empty token)
  TOKEN_ERROR,      // 1 - (Error token)
  TOKEN_IDENTIFIER, // 2 - (Identifier token)
  TOKEN_NUMBER,     // 3 - (Number token)
  TOKEN_EOF,        // 4 - (Token for end of file)
  TOKEN_COMMENTS,   // 5 - (Token for comments)
  TOKEN_UNKNOWN,    // 6 - (Unknown token)

  // Operadores OPERATOR:
  TOKEN_PLUS = 100,  // 100 - +
  TOKEN_MINUS,       // 101 - -
  TOKEN_ASTERISK,    // 102 - *
  TOKEN_SLASH,       // 103 - /
  TOKEN_LESSTHAN,    // 104 - <
  TOKEN_GREATERTHAN, // 105 - >
  TOKEN_EQUAL,       // 106 - =

  // Operadores compostos COMPOUND_OPERATOR:
  TOKEN_ASSIGNMENT = 200, // 200 - :=
  TOKEN_LESSEQUAL,        // 201 - <=
  TOKEN_GREATEREQUAL,     // 202 - >=
  TOKEN_NOTEQUAL,         // 203 - <>

  // Delimitadores DELIMITER:
  TOKEN_LPARENTHESIS = 300, // 300 - (
  TOKEN_RPARENTHESIS,       // 301 - )
  TOKEN_LBRACKET,           // 302 - [
  TOKEN_RBRACKET,           // 303 - ]
  TOKEN_SEMICOLON,          // 304 - ;
  TOKEN_COMMA,              // 305 - ,
  TOKEN_COLON,              // 306 - :
  TOKEN_PERIOD,             // 307 - .
  TOKEN_APOSTROPHE,         // 308 - '
  TOKEN_QUOTES,             // 309 - "

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

// -- PROTÓTIPOS DE FUNÇÃO --

// -- ANÁLISE SINTÁTICA
// -- PROGRAMAS E BLOCOS
void programa(std::vector<token>::iterator &current);
void bloco(std::vector<token>::iterator &current);

// -- DECLARAÇOES
void parte_declaraco_rotulos(std::vector<token>::iterator &current);
// parte_definições_tipos
// definicao_tipo
void tipo(std::vector<token>::iterator &current);
// indice
void parte_declaraco_variaveis(std::vector<token>::iterator &current);
void declaracao_variaveis(std::vector<token>::iterator &current);
void lista_identificadores(std::vector<token>::iterator &current);
void parte_declaracao_subrotinas(std::vector<token>::iterator &current);
void declaracao_procedimento(std::vector<token>::iterator &current);
void declaracao_funcao(std::vector<token>::iterator &current);
void parametros_formais(std::vector<token>::iterator &current);
void secao_parametros_formais(std::vector<token>::iterator &current);

// -- COMANDOS
void comando_composto(std::vector<token>::iterator &current);
void comando(std::vector<token>::iterator &current);
void comando_sem_rotulo(std::vector<token>::iterator &current);
void atribuicao(std::vector<token>::iterator &current);
void chamada_procedimento(std::vector<token>::iterator &current);
void desvio(std::vector<token>::iterator &current);
void comando_condicional(std::vector<token>::iterator &current);
void comando_repetitivo(std::vector<token>::iterator &current);

// -- EXPRESSÕES (Pode ser feita com análise de precedencia de operadores)
void lista_expressoes(std::vector<token>::iterator &current);
void expressao(std::vector<token>::iterator &current);
void relacao(std::vector<token>::iterator &current);
void expressao_simples(std::vector<token>::iterator &current);
void termo(std::vector<token>::iterator &current);
void fator(std::vector<token>::iterator &current);
void variavel(std::vector<token>::iterator &current);
void chamada_funcao(std::vector<token>::iterator &current);

// -- UTILIDADE SINTATICA
void check_token(std::vector<token>::iterator &current,
                 TokenCode expectedToken);
void rejeito(std::string msg);
bool isRelacao(TokenCode code);

// -- ANÁLISE LÉXICA
TokenType getTokenType(TokenCode code);
token analisador_lexico(std::string::iterator &prox,
                        const std::string::iterator &end);
std::vector<token> getTokens(std::string source_code);
std::map<TokenType, int> countTokenTypes(const std::vector<token> &tokens);

// -- FUNÇÕES DE UTILIDADE
std::string read_source_file(std::string file_path);
void printTokenDump(std::vector<token>::iterator &current);

// -- CONSTANTES --
const std::vector<std::string> tokenTypeNames = {
    "KEYWORD",           "IDENTIFIER", "NUMBER",   "OPERATOR",
    "COMPOUND OPERATOR", "DELIMITER",  "COMMENTS", "UNKNOWN"};

const std::vector<std::string> symbolTypeNames = {"VARIABLE", "FUNCTION",
                                                  "PROCEDURE", "TYPE"};
const std::unordered_map<std::string, TokenCode> simbolos_especiais = {
    {".", TOKEN_PERIOD},       {":", TOKEN_COLON},
    {",", TOKEN_COMMA},        {"(", TOKEN_LPARENTHESIS},
    {")", TOKEN_RPARENTHESIS}, {"=", TOKEN_EQUAL},
    {"<", TOKEN_LESSTHAN},     {">", TOKEN_GREATERTHAN},
    {"+", TOKEN_PLUS},         {"-", TOKEN_MINUS},
    {"*", TOKEN_ASTERISK},     {"[", TOKEN_LBRACKET},
    {"]", TOKEN_RBRACKET},     {":=", TOKEN_ASSIGNMENT},
    {"<=", TOKEN_LESSEQUAL},   {">=", TOKEN_GREATEREQUAL},
    {"<>", TOKEN_NOTEQUAL},    {";", TOKEN_SEMICOLON},
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
    // {"read", TOKEN_READ}, {"write", TOKEN_WRITE},
    {"not", TOKEN_NOT},
    {"of", TOKEN_OF},
    {"or", TOKEN_OR}};

const std::unordered_map<TokenCode, std::string> inverseIndex = {
    {TOKEN_PERIOD, "."},
    {TOKEN_COLON, ":"},
    {TOKEN_COMMA, ","},
    {TOKEN_LPARENTHESIS, "("},
    {TOKEN_RPARENTHESIS, ")"},
    {TOKEN_EQUAL, "="},
    {TOKEN_LESSTHAN, "<"},
    {TOKEN_GREATERTHAN, ">"},
    {TOKEN_PLUS, "+"},
    {TOKEN_MINUS, "-"},
    {TOKEN_ASTERISK, "*"},
    {TOKEN_LBRACKET, "["},
    {TOKEN_RBRACKET, "]"},
    {TOKEN_ASSIGNMENT, ":="},
    {TOKEN_LESSEQUAL, "<="},
    {TOKEN_GREATEREQUAL, ">="},
    {TOKEN_NOTEQUAL, "<>"},
    {TOKEN_SEMICOLON, ";"},
    {TOKEN_SLASH, "/"},
    {TOKEN_PROGRAM, "program"},
    {TOKEN_LABEL, "label"},
    {TOKEN_TYPE, "type"},
    {TOKEN_ARRAY, "array"},
    {TOKEN_VAR, "var"},
    {TOKEN_PROCEDURE, "procedure"},
    {TOKEN_FUNCTION, "function"},
    {TOKEN_BEGIN, "begin"},
    {TOKEN_END, "end"},
    {TOKEN_IF, "if"},
    {TOKEN_THEN, "then"},
    {TOKEN_ELSE, "else"},
    {TOKEN_WHILE, "while"},
    {TOKEN_DO, "do"},
    {TOKEN_DIV, "div"},
    {TOKEN_AND, "and"},
    {TOKEN_GOTO, "goto"},
    {TOKEN_READ, "read"},
    {TOKEN_WRITE, "write"},
    {TOKEN_NOT, "not"},
    {TOKEN_OF, "of"},
    {TOKEN_OR, "or"},
    {TOKEN_IDENTIFIER, "IDENTIFIER"},
    {TOKEN_NUMBER, "NUMBER"}};

// -- ANÁLISE LÉXICA --

// -- CONTAGEM DE TIPOS DE TOKEN (DEPRECATED)
[[deprecated("Não é necessário para questão de análise sintática")]]
TokenType getTokenType(TokenCode code) {
  if (code == TOKEN_IDENTIFIER)
    return TOKENTYPE_IDENTIFIER;
  if (code == TOKEN_NUMBER)
    return TOKENTYPE_NUMBER;
  if (code == TOKEN_COMMENTS)
    return TOKENTYPE_COMMENTS;
  if (code == TOKEN_UNKNOWN)
    return TOKENTYPE_UNKNOWN;
  if (code == TOKEN_ERROR)
    return TOKENTYPE_ERROR;
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

[[deprecated("Não é necessário para questão de análise sintática")]]
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

// -- ANÁLISE PROPIAMENTE DITA
token analisador_lexico(std::string::iterator &prox,
                        const std::string::iterator &end) {
  std::string atom;
  while (prox != end && isspace(*prox))
    prox++;

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
      s = "(*";
      prox++;
      auto temp = prox;
      while (temp != end) {
        if ((temp + 1) != end && *temp == '*' && *(temp + 1) == ')') {
          prox = temp + 2;
          return {s + "*)", TOKEN_COMMENTS}; // EXPLODE COMENTARIO
        }
        s.push_back(*temp);
        temp++;
      }
      prox--;
      return {"(", TOKEN_LPARENTHESIS};
    }
    return {s, simbolos_especiais.find(s)->second};
  }

  if (isalpha(*prox)) {
    while (prox != end && (isalpha(*prox) || isdigit(*prox))) {
      atom.push_back(*prox);
      prox++;
    }
    if (palavras_chave.find(atom) != palavras_chave.end())
      return {atom, palavras_chave.find(atom)->second};
    else
      return {atom, TOKEN_IDENTIFIER};
  }

  if (isdigit(*prox)) {
    bool rational_part = false;
    while (prox != end && (isdigit(*prox) || (*prox == '.' && !rational_part &&
                                              isdigit(*(prox + 1))))) {
      if (*prox == '.')
        rational_part = true;
      atom.push_back(*prox++);
    }
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
    } else if (tokens.back().code == TOKEN_COMMENTS)
      tokens.pop_back();
  } while (tokens.back().code != TOKEN_EOF);
  return tokens;
}

// -- ANÁLISE SINTÁTICA

// Tabela de símbolos
std::unordered_map<std::string, SymbolType> tabela_simbolos;

void programa(std::vector<token>::iterator &current) {
  check_token(current, TOKEN_PROGRAM);
  check_token(current, TOKEN_IDENTIFIER);
  check_token(current, TOKEN_LPARENTHESIS);
  lista_identificadores(current);
  check_token(current, TOKEN_RPARENTHESIS);
  check_token(current, TOKEN_SEMICOLON);
  bloco(current);
  check_token(current, TOKEN_PERIOD);
}

void bloco(std::vector<token>::iterator &current) {
  if (current->code == TOKEN_LABEL)
    parte_declaraco_rotulos(current);
  if (current->code == TOKEN_VAR)
    parte_declaraco_variaveis(current);
  if (current->code == TOKEN_PROCEDURE || current->code == TOKEN_FUNCTION)
    parte_declaracao_subrotinas(current);
  comando_composto(current);
}

void parte_declaraco_rotulos(std::vector<token>::iterator &current) {
  check_token(current, TOKEN_LABEL);
  check_token(current, TOKEN_NUMBER);
  while (current->code == TOKEN_COMMA) {
    current++;
    check_token(current, TOKEN_NUMBER);
  }
  check_token(current, TOKEN_SEMICOLON);
}

void tipo(std::vector<token>::iterator &current) {
  check_token(current, TOKEN_IDENTIFIER);
}

void declaracao_variaveis(std::vector<token>::iterator &current) {
  lista_identificadores(current);
  check_token(current, TOKEN_COLON);
  tipo(current);
}

void parte_declaraco_variaveis(std::vector<token>::iterator &current) {
  check_token(current, TOKEN_VAR);
  declaracao_variaveis(current);
  while (current->code == TOKEN_SEMICOLON &&
         (current + 1)->code == TOKEN_IDENTIFIER) {
    current++;
    declaracao_variaveis(current);
  }
  check_token(current, TOKEN_SEMICOLON);
}

void lista_identificadores(std::vector<token>::iterator &current) {
  check_token(current, TOKEN_IDENTIFIER);
  tabela_simbolos[(current - 1)->content] = SYMBOLTYPE_VARIABLE;
  while (current->code == TOKEN_COMMA) {
    current++;
    check_token(current, TOKEN_IDENTIFIER);
    tabela_simbolos[(current - 1)->content] = SYMBOLTYPE_VARIABLE;
  }
}

void parte_declaracao_subrotinas(std::vector<token>::iterator &current) {
  while (current->code == TOKEN_PROCEDURE || current->code == TOKEN_FUNCTION) {
    if (current->code == TOKEN_PROCEDURE)
      declaracao_procedimento(current);
    else
      declaracao_funcao(current);
    check_token(current, TOKEN_SEMICOLON);
  }
}

void declaracao_procedimento(std::vector<token>::iterator &current) {
  check_token(current, TOKEN_PROCEDURE);
  check_token(current, TOKEN_IDENTIFIER);
  tabela_simbolos[(current - 1)->content] = SYMBOLTYPE_PROCEDURE;
  if (current->code == TOKEN_LPARENTHESIS)
    parametros_formais(current);
  check_token(current, TOKEN_SEMICOLON);
  bloco(current);
}

void declaracao_funcao(std::vector<token>::iterator &current) {
  check_token(current, TOKEN_FUNCTION);
  check_token(current, TOKEN_IDENTIFIER);
  tabela_simbolos[(current - 1)->content] = SYMBOLTYPE_FUNCTION;
  if (current->code == TOKEN_LPARENTHESIS)
    parametros_formais(current);
  check_token(current, TOKEN_COLON);
  check_token(current, TOKEN_IDENTIFIER);
  check_token(current, TOKEN_SEMICOLON);
  bloco(current);
}

void parametros_formais(std::vector<token>::iterator &current) {
  check_token(current, TOKEN_LPARENTHESIS);
  secao_parametros_formais(current);
  while (current->code == TOKEN_SEMICOLON) {
    current++;
    secao_parametros_formais(current);
  }
  check_token(current, TOKEN_RPARENTHESIS);
}

void secao_parametros_formais(std::vector<token>::iterator &current) {
  if (current->code == TOKEN_VAR)
    check_token(current, TOKEN_VAR);
  lista_identificadores(current);
  check_token(current, TOKEN_COLON);
  check_token(current, TOKEN_IDENTIFIER);
  tabela_simbolos[(current - 1)->content] = SYMBOLTYPE_TYPE;
}

void comando_composto(std::vector<token>::iterator &current) {
  check_token(current, TOKEN_BEGIN);
  comando(current);
  while (current->code == TOKEN_SEMICOLON) {
    current++;
    comando(current);
  }
  check_token(current, TOKEN_END);
}

void comando(std::vector<token>::iterator &current) {
  if (current->code == TOKEN_NUMBER) {
    check_token(current, TOKEN_NUMBER);
    check_token(current, TOKEN_COLON);
  }
  comando_sem_rotulo(current);
}

void comando_sem_rotulo(std::vector<token>::iterator &current) {
  switch (current->code) {
  case TOKEN_IF:
    comando_condicional(current);
    break;
  case TOKEN_WHILE:
    comando_repetitivo(current);
    break;
  case TOKEN_BEGIN:
    comando_composto(current);
    break;
  case TOKEN_GOTO:
    desvio(current);
    break;
  case TOKEN_IDENTIFIER:
    switch ((current + 1)->code) {
    case TOKEN_ASSIGNMENT:
      atribuicao(current);
      break;
    default:
      chamada_procedimento(current);
      break;
    }
    break;
  default:
    printTokenDump(current);
    rejeito("ESPERADO IF, WHILE, BEGIN, GOTO, OU IDENTIFICADOR RECEBIDO: " +
            current->content);
    break;
  }
}

void atribuicao(std::vector<token>::iterator &current) {
  variavel(current);
  check_token(current, TOKEN_ASSIGNMENT);
  expressao(current);
}

void chamada_procedimento(std::vector<token>::iterator &current) {
  check_token(current, TOKEN_IDENTIFIER);
  if (current->code == TOKEN_LPARENTHESIS) {
    check_token(current, TOKEN_LPARENTHESIS);
    lista_expressoes(current);
    check_token(current, TOKEN_RPARENTHESIS);
  }
}

void desvio(std::vector<token>::iterator &current) {
  check_token(current, TOKEN_GOTO);
  check_token(current, TOKEN_NUMBER);
}

void comando_condicional(std::vector<token>::iterator &current) {
  check_token(current, TOKEN_IF);
  expressao(current);
  check_token(current, TOKEN_THEN);
  comando_sem_rotulo(current);
  if (current->code == TOKEN_ELSE) {
    check_token(current, TOKEN_ELSE);
    comando_sem_rotulo(current);
  }
}

void comando_repetitivo(std::vector<token>::iterator &current) {
  check_token(current, TOKEN_WHILE);
  expressao(current);
  check_token(current, TOKEN_DO);
  comando_sem_rotulo(current);
}

void lista_expressoes(std::vector<token>::iterator &current) {
  expressao(current);
  while (current->code == TOKEN_COMMA) {
    current++;
    expressao(current);
  }
}

void expressao(std::vector<token>::iterator &current) {
  expressao_simples(current);
  if (isRelacao(current->code)) {
    current++;
    expressao_simples(current);
  }
}

void expressao_simples(std::vector<token>::iterator &current) {
  if (current->code == TOKEN_PLUS || current->code == TOKEN_MINUS)
    current++;
  termo(current);
  while (current->code == TOKEN_PLUS || current->code == TOKEN_MINUS ||
         current->code == TOKEN_OR) {
    current++;
    termo(current);
  }
}

void termo(std::vector<token>::iterator &current) {
  fator(current);
  while (current->code == TOKEN_ASTERISK || current->code == TOKEN_DIV ||
         current->code == TOKEN_AND) {
    current++;
    fator(current);
  }
}

void fator(std::vector<token>::iterator &current) {
  // como diferenciar chamadas de função?
  switch (current->code) {
  case TOKEN_NUMBER:
    current++;
    break;
  case TOKEN_LPARENTHESIS:
    current++;
    expressao(current);
    check_token(current, TOKEN_RPARENTHESIS);
    break;
  case TOKEN_NOT:
    current++;
    fator(current);
    break;
  case TOKEN_IDENTIFIER:
    if (tabela_simbolos[current->content] == SYMBOLTYPE_VARIABLE)
      variavel(current);
    else if (tabela_simbolos[current->content] == SYMBOLTYPE_FUNCTION)
      chamada_funcao(current);
    else

      rejeito("ESPERADO VARIÁVEL OU CHAMADA DE FUNÇÃO, RECEBIDO: " +
              current->content);
    break;
  default:
    printTokenDump(current);
    rejeito("ESPERADO NUMERO, PARENTESE, NOT OU IDENTIFICADOR, RECEBIDO " +
            current->content);
    break;
  }
}

void variavel(std::vector<token>::iterator &current) {
  check_token(current, TOKEN_IDENTIFIER);
}

void chamada_funcao(std::vector<token>::iterator &current) {
  check_token(current, TOKEN_IDENTIFIER);
  if (current->code == TOKEN_LPARENTHESIS) {
    current++;
    lista_expressoes(current);
    check_token(current, TOKEN_RPARENTHESIS);
  }
}

// -- FUNÇÕES DE UTILIDADE --
std::string read_source_file(std::string file_path) {
  std::ifstream file(file_path);
  if (!file.is_open()) {
    std::cerr << "Erro awo abrir arquivo: " << file_path << std::endl;
    exit(1);
  }
  std::string buffer;
  std::string source_code;
  while (std::getline(file, buffer))
    source_code += buffer + '\n';

  file.close();
  return source_code;
}

void rejeito(std::string msg) {
  std::cout << "Rejeito\n";
  std::cerr << "Erro: " << msg << '\n';
  exit(0);
}
void printTokenDump(std::vector<token>::iterator &current) {
  auto temp = current;
  std::string tokenDump;
  int i = 0;
  while (temp->code != TOKEN_EOF && i < TOKENS_TO_DUMP) {
    tokenDump += temp->content + ", ";
    i++;
    temp++;
  }
  std::cerr << "TOKEN DUMP: " << tokenDump << "\n";
}

void check_token(std::vector<token>::iterator &current,
                 TokenCode expectedToken) {
  if (current->code != expectedToken) {
    std::string errorMsg =
        "Expected token: " + inverseIndex.find(expectedToken)->second +
        " where " + current->content + " found";
    printTokenDump(current);
    rejeito(errorMsg);
  }
  current++;
}

bool isRelacao(TokenCode code) {
  return code == TOKEN_EQUAL || code == TOKEN_NOTEQUAL ||
         code == TOKEN_LESSTHAN || code == TOKEN_LESSEQUAL ||
         code == TOKEN_GREATEREQUAL || code == TOKEN_GREATERTHAN;
}

int main(int argc, char *argv[]) {
  // argumento 0 é a própia chamada do executável
  std::string file_path(argv[1]);
  std::string source_code = read_source_file(file_path);
  std::vector<token> tokens = getTokens(source_code);
  auto it = tokens.begin();
  programa(it);
#ifdef DEBUG
  std::cout << "tokens:\n";
  for (auto &token : tokens)
    std::cout << tokenTypeNames[getTokenType(token.code)] << "-"
              << token.content << "\n";
  std::cout << "simbolos:\n";
  for (auto &[nome, tipo] : tabela_simbolos)
    std::cout << nome << " - " << symbolTypeNames[tipo] << '\n';
#endif
  if (it->code == TOKEN_EOF)
    std::cout << "Aceito\n";
  else {
    printTokenDump(it);
    rejeito("Não se chegou ao final do programa");
  }
  return 0;
}
