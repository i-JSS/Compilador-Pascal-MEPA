#include <cctype>
#include <exception>
#include <fstream>
#include <iostream>
#include <iterator>
#include <ostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <sstream>

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
            if (*prox == '\n')
                ++current_line;
            prox++;
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

enum SymbolType {
    SYMBOLTYPE_VARIABLE,
    SYMBOLTYPE_TYPE,
    SYMBOLTYPE_PROCEDURE,
    SYMBOLTYPE_PROGRAM,
    SYMBOLTYPE_CONSTANT,
    SYMBOLTYPE_FUNCTION,
    SYMBOLTYPE_PARAMETER,
    SYMBOLTYPE_LABEL,
    SYMBOLTYPE_NOTFOUND,
};

// TODO: adicionar propiedades diferentes dependendo do tipo de símbolo
struct SymbolProperties {
    SymbolType type;
    int depth;
};

// Como lidar com símbolos de mesmo nome e tipos diferentes?
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

    void insertSymbol(const std::string &identifier, SymbolType type) {
        declaration_stack[current_depth][identifier] = {type, current_depth};
    }

    void deleteSymbol(const std::string &identifier) {
        declaration_stack[current_depth].erase(identifier);
    }

};

class Parser {
public:
    explicit Parser(std::string &source_code)
            : lexer(source_code), current(lexer.getNextToken()),
              next(lexer.getNextToken()) {
        for (auto &symbol : preDeclaredSymbols)
            symbolTable.insertSymbol(symbol.first, symbol.second);
    }

    std::string parse() {
        programa();
        if (current.code != TOKEN_EOF) {
            rejeito("EOF not found");
        }

        return CodigoMepa;
    }

private:
    std::string CodigoMepa;
    // map pode ter melhor uso de memória
    const std::unordered_map<std::string, SymbolType> preDeclaredSymbols = {
            {"input", SYMBOLTYPE_VARIABLE}, {"output", SYMBOLTYPE_VARIABLE},
            {"integer", SYMBOLTYPE_TYPE},   {"boolean", SYMBOLTYPE_TYPE},
            {"read", SYMBOLTYPE_PROCEDURE}, {"write", SYMBOLTYPE_PROCEDURE},
            {"true", SYMBOLTYPE_CONSTANT},  {"false", SYMBOLTYPE_CONSTANT}};
    // I = IF  |  P = PROCEDURE  |  W = WHILE  |  E = ELSE / ELSEIF
    std::unordered_map<char, int> labels = {
            {'I', 0}, {'P', 0}, {'W', 0}, {'E', 0}};
    std::unordered_map<std::string, std::string> proceduresLabel = {
            {"read", "LEIT"}, {"write", "IMPR"}
    };
    // NOME VARIAVEL, CODIGO DELA E CONTEXTO INSERIDO
    std::unordered_map<std::string, std::pair<int, int>> variaveis;

    Lexer lexer;
    token current, next;
    SymbolTable symbolTable;
    int contexto = 0, contadorVariaveis = 0;

    std::string GERALABEL(char chave){
        if (labels.find(chave) != labels.end())
            return chave + std::to_string(labels[chave]++);
        return "";
    }

    void GERAPROGRAM(const std::string& key, const std::string& value){
        proceduresLabel[key] = value;
    }

    std::string GETPROGRAMNAME(const std::string& key) {
        return proceduresLabel.at(key);
    }

    void GERAVARIAVEL(const std::string& value){
        variaveis[value] = {contadorVariaveis++, contexto};
    }

    std::pair<int, int> GETVARIAVELDATA(const std::string& key){
        return variaveis.at(key);
    }

    int CONTAVARIAVEIS(int contextoAtual) {
        int contador = 0;
        for (auto& variavel : variaveis)
            if (variavel.second.second == contextoAtual) contador++;
        return contador;
    }

    void GERA(const std::string& linha, const std::vector<int>& params, const std::string& jump = "") {
        std::ostringstream oss;
        oss << linha;
        if (!params.empty()) {
            oss << " " << params[0];
            for (size_t i = 1; i < params.size(); ++i) {
                oss << ", " << params[i];
            }
        }
        if (!jump.empty()) oss << " " << jump;
        oss << " ;\n";

        std::cout << oss.str() ;
    }

    void rejeito(std::string msg) {
        std::string line_msg =
                msg + ", at line " + std::to_string(current.line_num);
        throw std::runtime_error(line_msg);
    }

    void next_token() {
        do {
            current = next;
            next = lexer.getNextToken();
        } while (current.code == TOKEN_COMMENTS);
    }

    void check_token(TokenCode expectedToken) {
        if (current.code != expectedToken) {
            std::string errorMsg = "Expected token { " +
                                   inverseIndex.find(expectedToken)->second +
                                   " }. got " + current.content;
            rejeito(errorMsg);
        }
        next_token();
    }

    SymbolType check_symbol_type() {
        SymbolProperties tryFind = symbolTable.searchSymbol(current.content);
        if (tryFind.type == SYMBOLTYPE_NOTFOUND)
            rejeito("Symbol " + current.content + " not found in this sccope");

        return tryFind.type;
    }

    void check_symbol(SymbolType type) {
        TokenCode expectedCode =
                type == SYMBOLTYPE_LABEL ? TOKEN_NUMBER : TOKEN_IDENTIFIER;
        if (current.code != expectedCode)
            rejeito("Expected symbol, got: " + current.content);

        SymbolProperties tryFind = symbolTable.searchSymbol(current.content);
        if (tryFind.type == SYMBOLTYPE_NOTFOUND)
            rejeito("Symbol " + current.content + " not found in this scope");
        if (tryFind.type != type)
            rejeito("Symbol " + current.content + " not of expected type");

        next_token();
    }

    void declare_symbol(SymbolType type) {
        TokenCode expectedCode =
                type == SYMBOLTYPE_LABEL ? TOKEN_NUMBER : TOKEN_IDENTIFIER;
        if (current.code != expectedCode)
            rejeito("Expected symbol, got: " + current.content);

        SymbolProperties tryFind = symbolTable.searchSymbol(current.content);
        if (tryFind.type != SYMBOLTYPE_NOTFOUND &&
            tryFind.depth == symbolTable.get_depth())
            rejeito("Symbol " + current.content + " not found in this scope");

        symbolTable.insertSymbol(current.content, type);
        next_token();
    }

    void programa() {
        check_token(TOKEN_PROGRAM);
        declare_symbol(SYMBOLTYPE_PROGRAM);
        check_token(TOKEN_LPARENTHESIS);
        lista_identificadores(SYMBOLTYPE_VARIABLE, false);
        check_token(TOKEN_RPARENTHESIS);
        check_token(TOKEN_SEMICOLON);
        GERA("INPP", {});
        bloco();
        check_token(TOKEN_PERIOD);
        if(CONTAVARIAVEIS(contexto))
            GERA("DMEM", {CONTAVARIAVEIS(contexto)});
    }

    //OK
    void bloco() {
        if (current.code == TOKEN_LABEL)
            parte_declaraco_rotulos();
        if (current.code == TOKEN_VAR) {
            parte_declaraco_variaveis();
        }
        std::string programLabel = GERALABEL('P');
        GERA("DSVS", {}, programLabel);
        if (current.code == TOKEN_PROCEDURE) parte_declaracao_subrotinas();
        GERA(programLabel+':', {}, "NADA");
        comando_composto();
    }

    void parte_declaraco_rotulos() {
        check_token(TOKEN_LABEL);
        declare_symbol(SYMBOLTYPE_LABEL);
        while (current.code == TOKEN_COMMA) {
            next_token();
            declare_symbol(SYMBOLTYPE_LABEL);
        }
        check_token(TOKEN_SEMICOLON);
    }

    void tipo() { check_symbol(SYMBOLTYPE_TYPE); }

    //OK
    void declaracao_variaveis() {
        lista_identificadores(SYMBOLTYPE_VARIABLE);
        check_token(TOKEN_COLON);
        tipo();
    }

    // OK
    void parte_declaraco_variaveis() {
        check_token(TOKEN_VAR);
        declaracao_variaveis();
        while (current.code == TOKEN_SEMICOLON && next.code == TOKEN_IDENTIFIER) {
            next_token();
            declaracao_variaveis();
        }
        check_token(TOKEN_SEMICOLON);
        if(contadorVariaveis)GERA("AMEM", {contadorVariaveis});
    }

    void lista_identificadores(SymbolType type, bool declaration = true) {
        if (declaration){
            GERAVARIAVEL(current.content);
            declare_symbol(type);
        }
        else check_symbol(type);
        while (current.code == TOKEN_COMMA) {
            next_token();
            if (declaration){
                GERAVARIAVEL(current.content);
                declare_symbol(type);
            }
            else check_symbol(type);
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

    // OK
    void declaracao_procedimento() {
        check_token(TOKEN_PROCEDURE);
        contadorVariaveis = 0;
        int contextoAtual = contexto;
        std::string comandoInicio = "ENPR " + std::to_string(++contexto),
                nomePrograma = GERALABEL('P');
        GERA(nomePrograma+':', {}, comandoInicio);
        GERAPROGRAM(current.content, nomePrograma);
        declare_symbol(SYMBOLTYPE_PROCEDURE);
        symbolTable.push_stack();
        if (current.code == TOKEN_LPARENTHESIS)
            parametros_formais();
        check_token(TOKEN_SEMICOLON);
        bloco();
        GERA("DMEM", {CONTAVARIAVEIS(contexto)});
        GERA("RTPR", {contexto--, contextoAtual});
        symbolTable.pop_stack();
    }

    void declaracao_funcao() {
        check_token(TOKEN_FUNCTION);
        declare_symbol(SYMBOLTYPE_FUNCTION);
        symbolTable.push_stack();
        if (current.code == TOKEN_LPARENTHESIS)
            parametros_formais();
        check_token(TOKEN_COLON);
        tipo(); // Não se se aqui seria um tipo ou identificador qualquer
        check_token(TOKEN_SEMICOLON);
        bloco();
        symbolTable.pop_stack();
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
        // TODO: ajeitar atribuição de parâmetros e outros piriris
        lista_identificadores(SYMBOLTYPE_PARAMETER);
        check_token(TOKEN_COLON);
        // aqui não se se seria tipo mesmo
        tipo();
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

    // OK
    void comando() {
        if (current.code == TOKEN_NUMBER) {
            GERA(current.content+':', {}, "NADA");
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

    // OK
    void atribuicao() {
        std::pair<int, int> codigo = GETVARIAVELDATA(current.content);
        variavel();
        check_token(TOKEN_ASSIGNMENT);
        expressao();
        GERA("ARMZ", {codigo.second, codigo.first});
    }

    // OK
    void chamada_procedimento() {
        std::string funcaoNome = GETPROGRAMNAME(current.content);
        check_symbol(SYMBOLTYPE_PROCEDURE);
        std::pair<int, int> codigoParametro;
        if (current.code == TOKEN_LPARENTHESIS) {
            check_token(TOKEN_LPARENTHESIS);
            codigoParametro = GETVARIAVELDATA(current.content);
            if(funcaoNome != "LEIT") lista_expressoes();
            else next_token();
            check_token(TOKEN_RPARENTHESIS);
        }
        if(funcaoNome == "LEIT") {
            GERA(funcaoNome, {});
            GERA("ARMZ", {codigoParametro.second, codigoParametro.first});
        }
        else if(funcaoNome == "IMPR") GERA(funcaoNome, {});
        else {
            std::string chamaProcedure = "CHPR " + funcaoNome + ',';
            GERA(chamaProcedure, {contexto});
        }
    }

    // OK
    void desvio() {
        check_token(TOKEN_GOTO);
        GERA("DSVS", {}, current.content);
        check_token(TOKEN_NUMBER);
    }

    // OK
    void comando_condicional() {
        check_token(TOKEN_IF);
        expressao();
        std::string ifLabel = GERALABEL('I'),
                elseLabel = GERALABEL('E'),
                fimLabel = elseLabel;
        GERA("DSVF", {}, ifLabel);
        GERA("DSVS", {}, elseLabel);
        check_token(TOKEN_THEN);
        GERA(ifLabel+':', {}, "NADA");
        comando_sem_rotulo();
        if (current.code == TOKEN_ELSE) {
            fimLabel = GERALABEL('E');
            GERA("DSVS", {}, fimLabel);
            GERA(elseLabel+':', {}, "NADA");
            check_token(TOKEN_ELSE);
            comando_sem_rotulo();
        }
        GERA(fimLabel+':', {}, "NADA");
    }

    // OK
    void comando_repetitivo() {
        check_token(TOKEN_WHILE);
        std::string inicioWhile = GERALABEL('W'),
                fimWhile = GERALABEL('W');
        GERA(inicioWhile+':', {}, "NADA");
        expressao();
        check_token(TOKEN_DO);
        GERA("DSVF", {}, fimWhile);
        comando_sem_rotulo();
        GERA("DSVS", {}, inicioWhile);
        GERA(fimWhile+':', {}, "NADA");
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
            std::string codeRelacao = getTokenNameRelacao(current.code);
            next_token();
            expressao_simples();
            GERA(codeRelacao, {});
        }
    }

    void expressao_simples() {
        if (isArithmeticOp(current.code))
            next_token();
        termo();
        while (isArithmeticOp(current.code) || current.code == TOKEN_OR) {
            std::string codeOperacao = getTokenNameOperacao(current.code);
            next_token();
            termo();
            GERA(codeOperacao, {});
        }
    }

    void termo() {
        fator();
        while (current.code == TOKEN_ASTERISK || current.code == TOKEN_DIV ||
               current.code == TOKEN_AND) {
            std::string codeOperacao = getTokenNameOperacao(current.code);
            next_token();
            fator();
            GERA(codeOperacao, {});
        }
    }

    void fator() {
        switch (current.code) {
            case TOKEN_NUMBER:
                GERA("CRCT "+ current.content, {});
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
                // TODO TRATAR A ORDEM DA NEGACAO
                GERA("NEGA", {});
                break;
            case TOKEN_IDENTIFIER: {
                auto type = check_symbol_type();
                if (type == SYMBOLTYPE_FUNCTION) {
                    chamada_funcao();
                }

                else if (type == SYMBOLTYPE_VARIABLE || type == SYMBOLTYPE_PARAMETER) {
                    variavel();
                }

                else
                    rejeito("Expected variable, parameter or function call. got " +
                            current.content);
                break;
            }
            default:
                rejeito("Expected number, (, not, variable or function call. got " +
                        current.content);
                break;
        }
    }

    void variavel() {
        auto type = check_symbol_type();
        if (type != SYMBOLTYPE_VARIABLE && type != SYMBOLTYPE_PARAMETER)
            rejeito("Expected variable or parameter. got " + current.content);
        if(next.code!=TOKEN_ASSIGNMENT){
            std::pair<int, int> codigo = GETVARIAVELDATA(current.content);
            GERA("CRVL", {codigo.second, codigo.first});
        }
        next_token();
    }

    void chamada_funcao() {
        check_symbol(SYMBOLTYPE_FUNCTION);
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

    std::string getTokenNameRelacao(TokenCode code) {
        switch (code) {
            case TOKEN_EQUAL:
                return "CMIG";
            case TOKEN_NOTEQUAL:
                return "CMDG";
            case TOKEN_LESSTHAN:
                return "CMME";
            case TOKEN_LESSEQUAL:
                return "CMEG";
            case TOKEN_GREATEREQUAL:
                return "CMAG";
            case TOKEN_GREATERTHAN:
                return "CMMA";
        }
        return "";
    }

    std::string getTokenNameOperacao(TokenCode code) {
        switch (code) {
            case TOKEN_OR:
                return "SOMA";
            case TOKEN_AND:
                return "MULT";
            case TOKEN_PLUS:
                return "SOMA";
            case TOKEN_MINUS:
                return "SUBT";
            case TOKEN_ASTERISK:
                return "MULT";
            case TOKEN_DIV:
                return "DIVI";
        }
        return "";
    }

    bool isSubrotina(TokenCode code) {
        return code == TOKEN_PROCEDURE || code == TOKEN_FUNCTION;
    }
};

// -- FUNÇÕES DE UTILIDADE --
std::string read_source_file(std::string file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        throw std::runtime_error("Error while opening file " + file_path);
    }
    std::string buffer;
    std::string source_code;
    while (std::getline(file, buffer))
        source_code += buffer + '\n';

    file.close();
    return source_code;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << "<source_file>" << std::endl;
        return 1;
    }

    std::vector<std::string> file_paths;
    for (int i = 1; i < argc; i++)
        file_paths.push_back(argv[i]);

    for (const auto &file_path : file_paths) {
        std::string source_code = read_source_file(file_path);
        Parser p(source_code);
        try {
            std::string CodigoMepa = p.parse();
            std::cout << "PARA ; Aceito\n";
        } catch (std::exception &e) {
            std::cout << "PARA ; Rejeito\n";
            std::cerr << e.what() << '\n';
        }
    }
    return 0;
}