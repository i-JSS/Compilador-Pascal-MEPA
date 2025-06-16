# Compilador de Pascal Simplificado para MEPA

|             Aluno             | Matrícula |
|:-----------------------------:|:---------:|
| André Emanuel Bispo da Silva  | 221007813 |
| João Antonio Ginuino carvalho | 221008150 |


---

## Sobre

Este projeto implementa um compilador completo para a linguagem **Pascal Simplificado**, gerando como saída código intermediário para a máquina hipotética **MEPA**.

O compilador é dividido em três etapas principais, cada uma implementada em um arquivo separado:

- **Análise Léxica** – `lexico.cpp`: Responsável por ler o código-fonte e identificar os tokens (palavras-chave, identificadores, números, símbolos, etc.).
- **Análise Sintática** – `sintatico.cpp`: Utiliza os tokens da análise léxica para verificar se a estrutura do programa segue as regras da gramática da linguagem.
- **Compilador** – `compilador.cpp`: Integra as etapas anteriores e gera o código MEPA correspondente ao programa Pascal Simplificado.

> O compilador possui suporte a estruturas de repetição (while, for), funções e procedimentos (function, procedure), conforme a sintaxe do Pascal Simplificado.

---

## Instalação

- **Linguagem:** C++11
- **Dependências:** Nenhuma biblioteca externa é necessária.

Para compilar os arquivos, utilize um compilador como o `g++`:

```bash
g++ -std=c++11 lexico.cpp -o lexico
g++ -std=c++11 sintatico.cpp -o sintatico
g++ -std=c++11 compilador.cpp -o compilador
````

---

## Uso

Cada arquivo pode ser executado de forma independente, permitindo realizar as etapas do processo de compilação separadamente.

Execute os binários gerados, passando o arquivo-fonte Pascal como argumento:

Análise Léxica
```bash
./lexico programa.pas
````

Análise Sintática
```bash
./sintatico programa.pas
````

Compilação (Geração de código MEPA)
```bash
./compilador programa.pas
````
