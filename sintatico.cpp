#include <cctype>
#include <fstream>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

// -- DEFINIÇõES --
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
  int line_num;
};

// -- PROTÓTIPOS DE FUNÇÃO --
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
  int current_line = 1;

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
    while (prox != end && isspace(*prox)) {
      prox++;
      if (*prox == '\n')
        ++current_line;
    }

    if (prox == end)
      return {"#", TOKEN_EOF, current_line};

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
            return {s + "*)", TOKEN_COMMENTS,
                    current_line}; // EXPLODE COMENTARIO
          }
          s.push_back(*temp);
          temp++;
        }
        prox--;
        return {"(", TOKEN_LPARENTHESIS, current_line};
      }
      return {s, simbolos_especiais.find(s)->second, current_line};
    }

    if (islower(*prox)) {
      while (prox != end && (islower(*prox) || isdigit(*prox))) {
        atom.push_back(*prox);
        prox++;
      }
      if (palavras_chave.find(atom) != palavras_chave.end())
        return {atom, palavras_chave.find(atom)->second, current_line};
      else
        return {atom, TOKEN_IDENTIFIER, current_line};
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
      return {atom, TOKEN_NUMBER, current_line};
    }

    prox++;
    return {s, TOKEN_UNKNOWN, current_line};
  }
};
// -- ANÁLISE SINTÁTICA

// Tabela de símbolos

enum SymbolType {
  SYMBOLTYPE_VARIABLE,
  SYMBOLTYPE_FUNCTION,
  SYMBOLTYPE_PROCEDURE,
  SYMBOLTYPE_TYPE,
  SYMBOLTYPE_PARAMETER,
  SYMBOLTYPE_CONSTANT,
  SYMBOLTYPE_NOTFOUND,
  SYMBOLTYPE_LABEL,
};

struct SymbolProperties {
  SymbolType type;
  int depth;
};

class SymbolTable {
private:
  std::vector<std::unordered_map<std::string, SymbolProperties>>
      declaration_stack;
  int current_depth = 0;

public:
  SymbolTable() {
    declaration_stack.push_back(
        std::unordered_map<std::string, SymbolProperties>());
  }

  int get_depth() const { return current_depth; }

  void push_stack() {
    current_depth++;
    declaration_stack.push_back(
        std::unordered_map<std::string, SymbolProperties>());
  }

  void pop_stack() {
    if (current_depth == 0)
      throw std::runtime_error("Trying to pop while call stack empty");

    current_depth--;
    declaration_stack.pop_back();
  }

  SymbolProperties searchSymbol(std::string &identifier) {
    int search_depth = current_depth;
    while (search_depth >= 0) {
      auto found = declaration_stack[search_depth].find(identifier);
      if (found != declaration_stack[search_depth].end())
        return found->second;
      search_depth--;
    }
    return {SYMBOLTYPE_NOTFOUND, -1};
  }

  void insertSymbol(std::string &identifier, SymbolType type) {
    declaration_stack[current_depth][identifier] = {type, current_depth};
  }

  void deleteSymbol(std::string &identifier) {
    declaration_stack[current_depth].erase(identifier);
  }
};

// Nessa casa não fazemos checagem de tipo
// TODO: se só houverem os tipos integer e boolean
// acho q é possível declarar eles ao iniciar o compilador
// talvez devamos fazer o mesmo para read e write
class Parser {
public:
  explicit Parser(std::string &source_code)
      : lexer(source_code), current(lexer.getNextToken()),
        next(lexer.getNextToken()) {}

  void parse() {
    programa();
    if (current.code == TOKEN_EOF) {
      std::cout << "Aceito\n";
      exit(0);
    } else {
      rejeito("EOF not found");
    }
  }

private:
  // const std::vector<std::string> symbolTypeNames = {"VARIABLE", "FUNCTION",
  // "PROCEDURE", "TYPE"};
  Lexer lexer;
  token current, next;
  // SymbolTable symbolTable;

  void rejeito(std::string msg) {
    std::cout << "Rejeito\n";
    std::cerr << "Error [" << msg << "] at line " << current.line_num << ".\n";
    exit(0);
  }

  void next_token() {
    do {
      current = next;
      next = lexer.getNextToken();
    } while (current.code == TOKEN_COMMENTS);
  }

  void check_token(TokenCode expectedToken) {
    if (current.code != expectedToken) {
      std::string errorMsg = "Expected token " +
                             inverseIndex.find(expectedToken)->second +
                             ". got " + current.content;
      rejeito(errorMsg);
    }
    next_token();
  }
  //
  // void declare_identifier(SymbolType type) {
  //   if (current.code != TOKEN_IDENTIFIER)
  //     rejeito("Expected identifier, got: " + current.content);
  //
  //   SymbolProperties tryFind = symbolTable.searchSymbol(current.content);
  //   if (tryFind.type != SYMBOLTYPE_NOTFOUND &&
  //       tryFind.depth == symbolTable.get_depth())
  //     rejeito("Identificador " + current.content + " Já declarado em
  //     escopo");
  //
  //   symbolTable.insertSymbol(current.content, type);
  // }
  //
  // void declare_label() {
  //   if (current.code != TOKEN_NUMBER)
  //     rejeito("Expected identifier, got: " + current.content);
  //
  //   SymbolProperties tryFind = symbolTable.searchSymbol(current.content);
  //   if (tryFind.type != SYMBOLTYPE_NOTFOUND &&
  //       tryFind.depth == symbolTable.get_depth())
  //     rejeito("Identificador " + current.content + " Já declarado em
  //     escopo");
  //
  //   symbolTable.insertSymbol(current.content, SYMBOLTYPE_LABEL);
  // }
  //
  void programa() {
    check_token(TOKEN_PROGRAM);
    check_token(TOKEN_IDENTIFIER);
    check_token(TOKEN_LPARENTHESIS);
    // Parâmetro mesmo?
    lista_identificadores(SYMBOLTYPE_PARAMETER);
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
    if (current.code == TOKEN_PROCEDURE /*|| current.code == TOKEN_FUNCTION*/)
      parte_declaracao_subrotinas();
    comando_composto();
  }

  void parte_declaraco_rotulos() {
    check_token(TOKEN_LABEL);
    // declare_label();
    check_token(TOKEN_NUMBER);
    while (current.code == TOKEN_COMMA) {
      next_token();
      // declare_label();
      check_token(TOKEN_NUMBER);
    }
    check_token(TOKEN_SEMICOLON);
  }

  void tipo() { check_token(TOKEN_IDENTIFIER); }

  void declaracao_variaveis() {
    lista_identificadores(SYMBOLTYPE_VARIABLE);
    check_token(TOKEN_COLON);
    tipo();
  }

  void parte_declaraco_variaveis() {
    check_token(TOKEN_VAR);
    declaracao_variaveis();
    // Refatorar isso aqui pode dar erro
    while (current.code == TOKEN_SEMICOLON && next.code == TOKEN_IDENTIFIER) {
      next_token();
      declaracao_variaveis();
    }
    check_token(TOKEN_SEMICOLON);
  }

  void lista_identificadores(SymbolType type) {
    check_token(TOKEN_IDENTIFIER);
    // declare_identifier(type);
    while (current.code == TOKEN_COMMA) {
      next_token();
      check_token(TOKEN_IDENTIFIER);
      // declare_identifier(type);
    }
  }

  void parte_declaracao_subrotinas() {
    while (isSubrotina(current.code)) {
      if (current.code == TOKEN_PROCEDURE)
        declaracao_procedimento();
      else
        declaracao_funcao();
      check_token(TOKEN_SEMICOLON);
    }
  }

  void declaracao_procedimento() {
    check_token(TOKEN_PROCEDURE);
    // declare_identifier(SYMBOLTYPE_PROCEDURE);
    check_token(TOKEN_IDENTIFIER);
    if (current.code == TOKEN_LPARENTHESIS)
      parametros_formais();
    check_token(TOKEN_SEMICOLON);
    // symbolTable.push_stack();
    bloco();
    // symbolTable.pop_stack();
  }

  void declaracao_funcao() {
    check_token(TOKEN_FUNCTION);
    // declare_identifier(SYMBOLTYPE_FUNCTION);
    check_token(TOKEN_IDENTIFIER);
    if (current.code == TOKEN_LPARENTHESIS)
      parametros_formais();
    check_token(TOKEN_COLON);
    check_token(TOKEN_IDENTIFIER);
    check_token(TOKEN_SEMICOLON);
    // symbolTable.push_stack();
    bloco();
    // symbolTable.pop_stack();
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
    lista_identificadores(SYMBOLTYPE_PARAMETER);
    check_token(TOKEN_COLON);
    check_token(TOKEN_IDENTIFIER); // Não vamos fazer checagem de tipo aqui
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
    switch (current.code) {
    case TOKEN_IF:
      comando_condicional();
      break;
    case TOKEN_WHILE:
      comando_repetitivo();
      break;
    case TOKEN_BEGIN:
      comando_composto();
      break;
    case TOKEN_GOTO:
      desvio();
      break;
    case TOKEN_IDENTIFIER:
      switch (next.code) {
      case TOKEN_ASSIGNMENT:
        atribuicao();
        break;
      default:
        chamada_procedimento();
        break;
      }
      break;
    default:
      rejeito(
          "Expected token if, while, begin, goto, variable or procedure. got " +
          current.content);
      break;
    }
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
    case TOKEN_IDENTIFIER: {
      // auto result = symbolTable.searchSymbol(current.content);
      // if (result.type == SYMBOLTYPE_VARIABLE)
      variavel();
      // else if (result.type == SYMBOLTYPE_FUNCTION)
      //   chamada_funcao();
      // else
      //   rejeito("Expected variable or function call. got" +
      //           current.content);
      break;
    }
    default:
      rejeito("Expected number, (, not, variable or function call. got " +
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
    std::cerr << "Error while opening file " << file_path << std::endl;
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
