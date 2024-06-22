#include <cctype>
#include <fstream>
#include <iostream>
#include <ostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#define TOKENS_TO_DUMP 10

// -- DEFINIÇõES --
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

// -- FUNÇÕES DE UTILIDADE
std::string read_source_file(std::string file_path);
void printTokenDump(std::vector<token>::iterator &current);

// -- CONSTANTES --

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

class Lexer {
private:
  std::string::iterator prox;
  const std::string::iterator end;

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

public:
  explicit Lexer(std::string &source_code)
      : prox(source_code.begin()), end(source_code.end()) {}

  token getNextToken() {
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

    if (islower(*prox)) {
      while (prox != end && (islower(*prox) || isdigit(*prox))) {
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
      while (prox != end &&
             (isdigit(*prox) ||
              (*prox == '.' && !rational_part && isdigit(*(prox + 1))))) {
        if (*prox == '.')
          rational_part = true;
        atom.push_back(*prox++);
      }
      return {atom, TOKEN_NUMBER};
    }

    prox++;
    return {s, TOKEN_UNKNOWN};
  }
};
// -- ANÁLISE SINTÁTICA

// Tabela de símbolos

class Parser {
public:
  explicit Parser(std::string &source_code)
      : lexer(source_code), current(lexer.getNextToken()) {}

  void parse() {
    programa();
    if (current.code == TOKEN_EOF) {
      std::cout << "Aceito\n";
      exit(0);
    } else {
      rejeito("Não se chegou ao final do programa");
    }
  }

private:
  const std::vector<std::string> symbolTypeNames = {"VARIABLE", "FUNCTION",
                                                    "PROCEDURE", "TYPE"};
  Lexer lexer;
  token current;
  std::unordered_map<std::string, SymbolType> tabela_simbolos;

  void insertSymbol(SymbolType type) {
    std::string symbol = current.content;
    tabela_simbolos.insert(std::make_pair(symbol, type));
  }

  void rejeito(std::string msg) {
    std::cout << "Rejeito\n";
    std::cerr << "Erro: " << msg << '\n';
    exit(0);
  }

  void next_token() { current = lexer.getNextToken(); }

  void check_token(TokenCode expectedToken) {
    if (current.code != expectedToken) {
      std::string errorMsg =
          "Expected token: " + inverseIndex.find(expectedToken)->second +
          " where " + current.content + " found";
      rejeito(errorMsg);
    }
    next_token();
  }

  void programa() {
    check_token(TOKEN_PROGRAM);
    check_token(TOKEN_IDENTIFIER);
    check_token(TOKEN_LPARENTHESIS);
    lista_identificadores();
    check_token(TOKEN_RPARENTHESIS);
    check_token(TOKEN_SEMICOLON);
    bloco();
    check_token(TOKEN_PERIOD);
  }

  void bloco() {
    if (current.code == TOKEN_LABEL)
      parte_declaraco_rotulos();
    if (current.code == TOKEN_VAR)
      parte_declaraco_variaveis();
    if (current.code == TOKEN_PROCEDURE || current.code == TOKEN_FUNCTION)
      parte_declaracao_subrotinas();
    comando_composto();
  }

  void parte_declaraco_rotulos() {
    check_token(TOKEN_LABEL);
    check_token(TOKEN_NUMBER);
    while (current.code == TOKEN_COMMA) {
      next_token();
      check_token(TOKEN_NUMBER);
    }
    check_token(TOKEN_SEMICOLON);
  }

  void tipo() { check_token(TOKEN_IDENTIFIER); }

  void declaracao_variaveis() {
    lista_identificadores();
    check_token(TOKEN_COLON);
    tipo();
  }

  // Refatorar declaracao de variavieis
  void parte_declaraco_variaveis() {
    // check_token(TOKEN_VAR);
    // declaracao_variaveis();
    // while (current.code == TOKEN_SEMICOLON &&
    //        (current + 1)->code == TOKEN_IDENTIFIER) {
    //   next_token();
    //   declaracao_variaveis();
    // }
    // check_token(TOKEN_SEMICOLON);
  }

  void lista_identificadores() {
    check_token(TOKEN_IDENTIFIER);
    insertSymbol(SYMBOLTYPE_VARIABLE);
    while (current.code == TOKEN_COMMA) {
      next_token();
      check_token(TOKEN_IDENTIFIER);
      insertSymbol(SYMBOLTYPE_VARIABLE);
    }
  }

  void parte_declaracao_subrotinas() {
    while (current.code == TOKEN_PROCEDURE || current.code == TOKEN_FUNCTION) {
      if (current.code == TOKEN_PROCEDURE)
        declaracao_procedimento();
      else
        declaracao_funcao();
      check_token(TOKEN_SEMICOLON);
    }
  }

  void declaracao_procedimento() {
    check_token(TOKEN_PROCEDURE);
    check_token(TOKEN_IDENTIFIER);
    insertSymbol(SYMBOLTYPE_PROCEDURE);
    if (current.code == TOKEN_LPARENTHESIS)
      parametros_formais();
    check_token(TOKEN_SEMICOLON);
    bloco();
  }

  void declaracao_funcao() {
    check_token(TOKEN_FUNCTION);
    check_token(TOKEN_IDENTIFIER);
    insertSymbol(SYMBOLTYPE_FUNCTION);
    if (current.code == TOKEN_LPARENTHESIS)
      parametros_formais();
    check_token(TOKEN_COLON);
    check_token(TOKEN_IDENTIFIER);
    check_token(TOKEN_SEMICOLON);
    bloco();
  }

  void parametros_formais() {
    check_token(TOKEN_LPARENTHESIS);
    secao_parametros_formais();
    while (current.code == TOKEN_SEMICOLON) {
      next_token();
      secao_parametros_formais();
    }
    check_token(TOKEN_RPARENTHESIS);
  }

  void secao_parametros_formais() {
    if (current.code == TOKEN_VAR)
      check_token(TOKEN_VAR);
    lista_identificadores();
    check_token(TOKEN_COLON);
    check_token(TOKEN_IDENTIFIER);
    insertSymbol(SYMBOLTYPE_FUNCTION);
  }

  void comando_composto() {
    check_token(TOKEN_BEGIN);
    comando();
    while (current.code == TOKEN_SEMICOLON) {
      next_token();
      comando();
    }
    check_token(TOKEN_END);
  }

  void comando() {
    if (current.code == TOKEN_NUMBER) {
      check_token(TOKEN_NUMBER);
      check_token(TOKEN_COLON);
    }
    comando_sem_rotulo();
  }

  void comando_sem_rotulo() {
    // switch (current.code) {
    // case TOKEN_IF:
    //   comando_condicional();
    //   break;
    // case TOKEN_WHILE:
    //   comando_repetitivo();
    //   break;
    // case TOKEN_BEGIN:
    //   comando_composto();
    //   break;
    // case TOKEN_GOTO:
    //   desvio();
    //   break;
    // // Tabela de símbolos aqui!
    // case TOKEN_IDENTIFIER:
    //   switch ((current + 1)->code) {
    //   case TOKEN_ASSIGNMENT:
    //     atribuicao();
    //     break;
    //   default:
    //     chamada_procedimento();
    //     break;
    //   }
    //   break;
    // default:
    //   rejeito("ESPERADO IF, WHILE, BEGIN, GOTO, OU IDENTIFICADOR RECEBIDO: "
    //   +
    //           current.content);
    //   break;
    // }
  }

  void atribuicao() {
    variavel();
    check_token(TOKEN_ASSIGNMENT);
    expressao();
  }

  void chamada_procedimento() {
    check_token(TOKEN_IDENTIFIER);
    if (current.code == TOKEN_LPARENTHESIS) {
      check_token(TOKEN_LPARENTHESIS);
      lista_expressoes();
      check_token(TOKEN_RPARENTHESIS);
    }
  }

  void desvio() {
    check_token(TOKEN_GOTO);
    check_token(TOKEN_NUMBER);
  }

  void comando_condicional() {
    check_token(TOKEN_IF);
    expressao();
    check_token(TOKEN_THEN);
    comando_sem_rotulo();
    if (current.code == TOKEN_ELSE) {
      check_token(TOKEN_ELSE);
      comando_sem_rotulo();
    }
  }

  void comando_repetitivo() {
    check_token(TOKEN_WHILE);
    expressao();
    check_token(TOKEN_DO);
    comando_sem_rotulo();
  }

  void lista_expressoes() {
    expressao();
    while (current.code == TOKEN_COMMA) {
      next_token();
      expressao();
    }
  }

  void expressao() {
    expressao_simples();
    if (isRelacao(current.code)) {
      next_token();
      expressao_simples();
    }
  }

  void expressao_simples() {
    if (isArithmeticOp(current.code))
      next_token();
    termo();
    while (isArithmeticOp(current.code) || current.code == TOKEN_OR) {
      next_token();
      termo();
    }
  }

  void termo() {
    fator();
    while (current.code == TOKEN_ASTERISK || current.code == TOKEN_DIV ||
           current.code == TOKEN_AND) {
      next_token();
      fator();
    }
  }

  void fator() {
    // refatorar tabela de símbolos
    switch (current.code) {
    case TOKEN_NUMBER:
      next_token();
      break;
    case TOKEN_LPARENTHESIS:
      next_token();
      expressao();
      check_token(TOKEN_RPARENTHESIS);
      break;
    case TOKEN_NOT:
      next_token();
      fator();
      break;
    case TOKEN_IDENTIFIER:
      // Talvez o erro tenha a ver com isso aqui?
      if (tabela_simbolos[current.content] == SYMBOLTYPE_VARIABLE)
        variavel();
      else if (tabela_simbolos[current.content] == SYMBOLTYPE_FUNCTION)
        chamada_funcao();
      else

        rejeito("ESPERADO VARIÁVEL OU CHAMADA DE FUNÇÃO, RECEBIDO: " +
                current.content);
      break;
    default:
      rejeito("ESPERADO NUMERO, PARENTESE, NOT OU IDENTIFICADOR, RECEBIDO " +
              current.content);
      break;
    }
  }

  void variavel() { check_token(TOKEN_IDENTIFIER); }

  void chamada_funcao() {
    check_token(TOKEN_IDENTIFIER);
    if (current.code == TOKEN_LPARENTHESIS) {
      next_token();
      lista_expressoes();
      check_token(TOKEN_RPARENTHESIS);
    }
  }

  bool isArithmeticOp(TokenCode code) {
    return code == TOKEN_PLUS || code == TOKEN_MINUS;
  }

  bool isRelacao(TokenCode code) {
    return code == TOKEN_EQUAL || code == TOKEN_NOTEQUAL ||
           code == TOKEN_LESSTHAN || code == TOKEN_LESSEQUAL ||
           code == TOKEN_GREATEREQUAL || code == TOKEN_GREATERTHAN;
  }

  bool isSubrotina(TokenCode code) {
    return code == TOKEN_PROCEDURE || code == TOKEN_FUNCTION;
  }
};

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

int main(int argc, char *argv[]) {
  // argumento 0 é a própia chamada do executável
  std::string file_path(argv[1]);
  std::string source_code = read_source_file(file_path);
  Parser p(source_code);
  p.parse();

  return 0;
}
