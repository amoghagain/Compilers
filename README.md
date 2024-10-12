
# Compilers - A compiler for an English-like language

The following code explains how a Compiler works in real world, acting upon strings(in this case), and many other entities to simplify and process things.

## About the process:

I have designed a simple English-like language compiler that performs both lexical and syntactic analysis based on the specifications provided in the task. The goal of the compiler is to translate high-level, human-readable English-like syntax into a more structured format for further processing, such as generating executable code or producing an intermediate representation.

The approach I took is a handwritten implementation, where I manually wrote the functions and methods required for lexical analysis (also known as tokenization) and syntactic analysis (also known as parsing). The decision to use a handwritten approach allowed for more control over the behavior of each phase of the compilation process, offering greater flexibility and a deeper understanding of how the components interact. This decision also aligns with learning objectives, as it provides insight into how compilers operate at a fundamental level.

### Lexical Analysis (Tokenization)
The first step in the compiler is lexical analysis, which involves breaking down the input string into a sequence of tokens. A token is a meaningful unit in the source code, such as keywords, identifiers, literals, operators, and punctuation. The Symbol Table is constructed during this phase to map tokens to their corresponding token types.

For the lexical analyzer, I implemented various methods to:

**Identify keywords:** Keywords are reserved words in the language (e.g., if, while, begin, end) that have predefined meanings.

**Detect identifiers**: Identifiers represent variables or functions and must adhere to specific naming rules (e.g., starting with a letter, followed by alphanumeric characters).

**Handle literals**: The lexer also identifies numerical literals (e.g., integers or floating-point numbers) and string literals.
Process operators and delimiters: Operators (e.g., +, -, =, ==) and delimiters (e.g., ,, ;, ()) are also recognized as tokens.

**Error handling**: If an invalid character or sequence is encountered, a Lexical Error is reported, providing details about the position of the error and its cause.
The result of lexical analysis is a stream of tokens, along with the Symbol Table that stores information about the token types. This output is then passed to the syntactic analysis phase for further validation.

### Syntactic Analysis (Parsing):
Once the token stream is generated, the compiler proceeds to the syntactic analysis phase. The role of the parser is to ensure that the tokens follow the correct grammatical structure as defined by the language's grammar rules. For this, I created a recursive descent parser, which uses multiple functions to parse different components of the grammar, such as expressions, statements, and blocks.

The key features of the parser include:

**Expression Parsing:** The parser checks that mathematical and logical expressions are structured correctly, ensuring proper operator precedence and associativity. Parentheses are handled correctly to group sub-expressions as needed.

**Statement Parsing:** Statements such as assignments, conditionals (if-else), loops (while), and function calls are validated according to the grammar.
Block Handling: Blocks of code enclosed in begin and end keywords are parsed to ensure correct nesting and scope.

**Error Detection:** If a syntactic error is encountered (e.g., a missing operator or mismatched parentheses), the parser provides a detailed Parsing Error message, including the line number and a brief description of the issue.
The parser constructs an Abstract Syntax Tree (AST), a tree-like structure that represents the hierarchical relationships between different components of the input. The AST is an essential intermediate representation, as it is used in later stages of the compiler, such as optimization and code generation. The structure of the AST makes it easier to perform semantic checks and transformations.

The features can be accessed by running the above code in you respective terminals.

**Tests:**
1. A string:
 It parses through every token and analyzes it syntactically and gives the accepted string along with the Abstract Syntax Tree(AST).

![img alt](https://github.com/amoghagain/Compilers/blob/cfb4bdb7c0da0a8093c763badff2ad180a25db70/comp1.PNG)

2. If we add a word of length > 26:
It gives an invalid token as the length is greater to parse.

![img alt](https://github.com/amoghagain/Compilers/blob/f407776c212af539263f3787b51da287e7c56c32/comp5.PNG)


3. If we add a word with length < 3:

4. If we add a number

5. If we remove fullstop

6. If 1st word isn't capital

7. If we add a quoted word

8. If we include multiple punctuation marks(already multiple like 2 hyphens and two commas)


It parses the commas first.

9. If we have words or strings after fullstops.
