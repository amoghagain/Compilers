/**
 * @file lexer_parser.cpp
 * @brief Lexer and Parser implementation for a custom language following specified lexical and syntactic rules.
 *
 * This program implements a Lexer to tokenize an input string and a Parser to validate the syntactic correctness
 * of the token sequence based on predefined grammar. The Parser builds an Abstract Syntax Tree (AST) if the input
 * is valid.
 *
 * Lexical tokens include words, startwords, punctuation (comma, hyphen, stop), and quotations.
 * Errors are generated for invalid tokens and incorrect syntax.
 */


#include <iostream>
#include <cctype>
#include <string>
#include <vector>
#include <queue>        // For level-order traversal
#include <memory>       // For smart pointers
#include <unordered_map>


std::vector<std::string> lexicalErrors;     ///< List of lexical errors (Defining it globally because it has multiple uses)



// Token types enumeration
/**
 * @enum TokenType
 * @brief Enumerates the various types of tokens in the input string.
 */
enum TokenType { STARTWORD, WORD, COMMA, HYPHEN, STOP, QUOTATION, INVALID, END };


// Token structure
/**
 * @struct Token
 * @brief Represents a token with a type and its corresponding string value.
 */
struct Token {
    TokenType type;
    std::string value;
};


// AST Node structure
/**
 * @struct ASTNode
 * @brief Represents a node in the Abstract Syntax Tree (AST).
 * 
 * Each ASTNode holds a value (e.g., token value or type) and a list of child nodes, forming the tree structure.
 */
struct ASTNode {
    std::string value;                                      ///< Value of the node (e.g., token or expression)
    std::vector<std::shared_ptr<ASTNode>> children;         ///< List of child nodes

    // Constructor
    /**
     * @brief Constructs an ASTNode with the given value.
     * @param val The value of the AST node.
     */
    ASTNode(const std::string& val) : value(val) {}

    // Method to add a child node
    /**
     * @brief Adds a child node to the current node.
     * @param child Shared pointer to the child ASTNode.
     */
    void addChild(std::shared_ptr<ASTNode> child) {
        children.push_back(child);
    }
};



// Lexer class to tokenize input strings
/**
 * @class Lexer
 * @brief The Lexer class tokenizes input strings based on lexical specifications.
 *
 * It processes input character by character and generates tokens such as words, punctuation, and quotations.
 * Errors are raised for invalid tokens, and a symbol table is built for valid words.
 */
class Lexer {
public:
    // Constructor
    /**
     * @brief Constructs a Lexer with the provided input string.
     * @param input The input string to tokenize.
     */
    Lexer(const std::string &input) : input(input), pos(0) {}

    // Method to get the next token from input
    /**
     * @brief Retrieves the next token from the input string.
     * @return Token The next token in the input stream.
     *
     * This function skips whitespaces, identifies valid tokens such as words, punctuation, or quotations, 
     * and splits words longer than 26 characters. Invalid tokens are also handled and returned.
     */
    Token nextToken() {
        // Skip whitespace
        while (pos < input.length() && std::isspace(input[pos])) {
            pos++;
        }

        if (pos >= input.length()) return { END, "" };

        char current = input[pos];

        // Handle punctuation and stop tokens
        if (current == ',') {
            pos++;
            return { COMMA, "," };
        }
        if (current == '-') {
            pos++;
            return { HYPHEN, "-" };
        }
        if (current == '.') {
            pos++;
            return { STOP, "." };
        }

        // Handle quotations
        if (current == '\'') {
            pos++;
            std::string quotedText;
            while (pos < input.length() && input[pos] != '\'') {
                quotedText += input[pos++];
            }
            if (pos < input.length()) pos++; // Skip closing quote
            return { QUOTATION, quotedText };
        }

        // Identify words (valid and split longer words)
        std::string word;
        while (pos < input.length() && std::isalpha(input[pos])) {
            word += input[pos++];
        }

        if (!word.empty()) {
            if (word.length() > 26) {
                // Split long words
                std::string firstPart = word.substr(0, 26);
                std::string remainingPart = word.substr(26);
                pos -= remainingPart.length();              // Adjust position to tokenize remaining part later
                return { WORD, firstPart };
            }
            if (word.length() >= 3 && word.length() <= 26) {
                if (std::isupper(word[0])) {
                    symbolTable.push_back(word);            // Add to symbol table
                    return { STARTWORD, word };
                } else {
                    symbolTable.push_back(word);            // Add to symbol table
                    return { WORD, word };
                }
            } else {
                return { INVALID, word };                   // Invalid token if word length is wrong
            }
        }

        // If we reach here, it's an invalid token
        std::string invalidToken;
        while (pos < input.length() && !std::isspace(input[pos]) && input[pos] != ',' && input[pos] != '-' && input[pos] != '.') {
            invalidToken += input[pos++];
        }
        return { INVALID, invalidToken };
    }

    
    // Display symbol table
    /**
     * @brief Prints the symbol table generated during tokenization.
     */
    void printSymbolTable() const {
        std::cout << "\nSymbol Table: \n";
        for (const auto& symbol : symbolTable) {
            std::cout << symbol << std::endl;
        }
    }

private:
    std::string input;                     ///< Input string to be tokenized
    size_t pos;                            ///< Current position in the input string
    std::vector<std::string> symbolTable;  ///< Symbol table to store valid words

};




// Parser class for parsing the tokens generated
/**
 * @class Parser
 * @brief Syntactic analyzer that parses a sequence of tokens based on grammer rules specified
 * 
 * The Parser class takes a sequence of tokens generated by the Lexer and checks whether 
 * they conform to the syntactic rules of the language. It builds an Abstract Syntax Tree (AST) 
 * to represent the structure of the input, and handles errors if the input is not valid.
 */
class Parser {
public:
    //Constructor
    /**
     * @brief Constructor that initializes the Parser with a sequence of tokens.
     * @param tokens A vector of tokens produced by the Lexer.
     */
    Parser(const std::vector<Token>& tokens) : tokens(tokens), currentPos(0) {}

    /**
     * @brief Parses the input tokens to build an AST.
     * 
     * The parse() method begins by parsing the first token as a start word, then continues 
     * parsing subsequent tokens based on the language's syntactic rules (handling words, commas, 
     * hyphens, and punctuation). If the input is valid, it returns a pointer to the root of the AST.
     * 
     * @return std::shared_ptr<ASTNode> The root node of the AST, or nullptr if there are errors.
     */
    std::shared_ptr<ASTNode> parse() {
        return parseSentence();
    }

    /**
     * @brief Checks if any errors were encountered during parsing.
     * @return bool True if errors exist, false otherwise.
     */
    bool hasErrors() const { return !errors.empty(); }

    /**
     * @brief Prints any errors encountered during the parsing process.
     */
    void printErrors() const {
        for (const auto& error : errors) {
            std::cout << error << std::endl;
        }
    }
    
    /**
     * @brief Prints the accepted string formed during parsing (skipping quotations).
     */
    void printAcceptedString() const {
        for (size_t i = 0; i < acceptedTokens.size(); ++i) {
            if (acceptedTokens[i].type != QUOTATION) {
                std::cout << acceptedTokens[i].value;
                if (i != acceptedTokens.size() - 1 ) {
                    std::cout << " ";                   // Add space between tokens 
                }
            }
        }
        std::cout << std::endl;
    }

private:
    std::vector<Token> tokens;          ///< The input tokens produced by the Lexer.
    size_t currentPos;                  ///< Current position in the token list.
    std::vector<std::string> errors;    ///< List of errors encountered during parsing.
    std::vector<Token> acceptedTokens;  ///< List of accepted tokens that form the valid string.

    /**
    * @brief Retrieves the current token.
    * @return Token The current token in the token sequence.
    */
    Token currentToken() {
        if (currentPos >= tokens.size()) return { END, "" };
        return tokens[currentPos];
    }

    /**
     * @brief Advances to the next token.
     */
    void advanceToken() {
        if (currentPos < tokens.size()) {
            currentPos++;
        }
    }

    /**
     * @brief Parses a sentence according to the syntactic rules.
     * 
     * The parseSentence() method parses tokens as a valid sentence, ensuring that 
     * the input starts with a Startword and ends with a Stop. It also handles commas, 
     * hyphens, and quotations as per the defined grammar.
     * 
     * @return std::shared_ptr<ASTNode> The root node representing the sentence, or nullptr if there are errors.
     */
    std::shared_ptr<ASTNode> parseSentence() {
    std::shared_ptr<ASTNode> sentenceNode = std::make_shared<ASTNode>("Sentence");

    std::shared_ptr<ASTNode> startNode = parseStartword();
    if (startNode) {
        sentenceNode->addChild(startNode);
        acceptedTokens.push_back(tokens[currentPos - 1]);
    } else {
        return nullptr;
    }

    bool lastWasComma = false;
    bool lastWasHyphen = false;

    while (currentPos < tokens.size() && currentToken().type != STOP) {
        if (currentToken().type == COMMA) {
            if (lastWasComma) {
                errors.push_back("Error: Consecutive commas found.");
                return nullptr;
            }
            std::shared_ptr<ASTNode> commaNode = parseComma();
            if (commaNode) {
                sentenceNode->addChild(commaNode);
                acceptedTokens.push_back(tokens[currentPos - 1]);
            }
            lastWasComma = true;
            lastWasHyphen = false;
        } else if (currentToken().type == HYPHEN) {
            if (lastWasHyphen) {

                // Flags for tracking comma till end of string
                bool commaFound = false;
                bool commaError = false;

                // Checking for consecutive commas in rest of the string
                size_t tempPos = currentPos;
                while (tempPos < tokens.size() && tokens[tempPos].type != STOP) {
                    if (tokens[tempPos].type == COMMA) {
                        if (commaFound) {
                            commaError = true;
                            break;
                        }
                        commaFound = true;
                    }
                    tempPos++;
                }

                if (commaError) {
                    errors.push_back("Error: Consecutive commas found.");
                    return nullptr;
                } else if (!commaFound) {
                    errors.push_back("Error: Consecutive hyphens found.");
                    return nullptr;
                }
            }

            std::shared_ptr<ASTNode> hyphenNode = parseHyphen();
            if (hyphenNode) {
                sentenceNode->addChild(hyphenNode);
                acceptedTokens.push_back(tokens[currentPos - 1]);
            }
            lastWasHyphen = true;
            lastWasComma = false;
        } else if (currentToken().type == WORD) {
            std::shared_ptr<ASTNode> wordNode = parseWord();
            if (wordNode) {
                sentenceNode->addChild(wordNode);
                acceptedTokens.push_back(tokens[currentPos - 1]);
            }
            lastWasComma = false;
            lastWasHyphen = false;
        } else if (currentToken().type == QUOTATION) {
            std::shared_ptr<ASTNode> quotationNode = parseQuotation();
            if (quotationNode) {
                sentenceNode->addChild(quotationNode);
                acceptedTokens.push_back(tokens[currentPos - 1]);
            }
            lastWasComma = false;
            lastWasHyphen = false;
        } else {
            errors.push_back("Unexpected token: " + currentToken().value);
            return nullptr;
        }
    }

    // Check if we encountered the STOP token
    if (currentPos < tokens.size() && currentToken().type == STOP) {
        std::shared_ptr<ASTNode> stopNode = parseStop();
        if (stopNode) {
            sentenceNode->addChild(stopNode);
            acceptedTokens.push_back(tokens[currentPos - 1]);
            advanceToken();  // Move past the STOP token

            // DEBUG: Confirm advancing after STOP
            // std::cout << "Reached STOP token. Advancing token pointer.\n";
        }
    } else {
        errors.push_back("Expected STOP at the end");
        return nullptr;
    }

    // Check if any tokens remain after the STOP token
    if (currentPos <= tokens.size() && (tokens[currentPos-1].type == WORD || tokens[currentPos-1].type == COMMA || tokens[currentPos-1].type == HYPHEN || tokens[currentPos-1].type == QUOTATION || tokens[currentPos-1].type == STARTWORD || tokens[currentPos-1].type == INVALID)){
        // std::cout << "Extra tokens detected after STOP: " << currentToken().value << "\n";  // Debugging info
        errors.push_back("Error: Extra tokens found after full stop.");
        return nullptr;
    }

    // Check for lexical errors
    if (!lexicalErrors.empty()) {
        errors.push_back("Error: Lexical errors found. Invalid tokens in the sentence.");
        return nullptr;
    }

    return sentenceNode;
}


    /**
     * @brief Parses a Startword token.
     * @return std::shared_ptr<ASTNode> The node representing the Startword, or nullptr if parsing fails.
     */
    std::shared_ptr<ASTNode> parseStartword() {
        if (currentToken().type == STARTWORD) {
            std::shared_ptr<ASTNode> node = std::make_shared<ASTNode>("Startword: " + currentToken().value);
            advanceToken();  // Move to the next token
            return node;
        } else {
            errors.push_back("Expected Startword, got: " + currentToken().value);
            return nullptr;
        }
    }

    /**
     * @brief Parses a Word token.
     * @return std::shared_ptr<ASTNode> The node representing the Word, or nullptr if parsing fails.
     */
    std::shared_ptr<ASTNode> parseWord() {
        if (currentToken().type == WORD) {
            std::shared_ptr<ASTNode> node = std::make_shared<ASTNode>("Word: " + currentToken().value);
            advanceToken();  // Move to the next token
            return node;
        } else {
            errors.push_back("Expected Word, got: " + currentToken().value);
            return nullptr;
        }
    }

    /**
     * @brief Parses a Comma token.
     * @return std::shared_ptr<ASTNode> The node representing the Comma, or nullptr if parsing fails.
     */
    std::shared_ptr<ASTNode> parseComma() {
        if (currentToken().type == COMMA) {
            std::shared_ptr<ASTNode> node = std::make_shared<ASTNode>("Comma");
            advanceToken();  // Move to the next token
            return node;
        } else {
            errors.push_back("Expected Comma, got: " + currentToken().value);
            return nullptr;
        }
    }
    
     /**
     * @brief Parses a Hyphen token.
     * @return std::shared_ptr<ASTNode> The node representing the Hyphen, or nullptr if parsing fails.
     */
    std::shared_ptr<ASTNode> parseHyphen() {
        if (currentToken().type == HYPHEN) {
            std::shared_ptr<ASTNode> node = std::make_shared<ASTNode>("Hyphen");
            advanceToken();  // Move to the next token
            return node;
        } else {
            errors.push_back("Expected Hyphen, got: " + currentToken().value);
            return nullptr;
        }
    }

    /**
     * @brief Parses a Quotation token.
     * @return std::shared_ptr<ASTNode> The node representing the Quotation, or nullptr if parsing fails.
     */
    std::shared_ptr<ASTNode> parseQuotation() {
        if (currentToken().type == QUOTATION) {
            std::shared_ptr<ASTNode> node = std::make_shared<ASTNode>("Quotation: " + currentToken().value);
            advanceToken();  // Move to the next token
            return node;
        } else {
            errors.push_back("Expected Quotation, got: " + currentToken().value);
            return nullptr;
        }
    }

    /**
     * @brief Parses a Stop token.
     * @return std::shared_ptr<ASTNode> The node representing the Stop, or nullptr if parsing fails.
     */
    std::shared_ptr<ASTNode> parseStop() {
        if (currentToken().type == STOP) {
            std::shared_ptr<ASTNode> node = std::make_shared<ASTNode>("Stop");
            advanceToken();  // Move to the next token
            return node;
        } else {
            errors.push_back("Expected Stop, got: " + currentToken().value);
            return nullptr;
        }
    }
};


// Helper function to print the AST structure in level-order format
void printASTLevelOrder(const std::shared_ptr<ASTNode>& root) {
    if (!root) return;

    std::queue<std::shared_ptr<ASTNode>> nodeQueue;
    nodeQueue.push(root);

    while (!nodeQueue.empty()) {
        size_t levelSize = nodeQueue.size();  // Number of nodes at current level

        // Process all nodes at the current level
        for (size_t i = 0; i < levelSize; ++i) {
            std::shared_ptr<ASTNode> currentNode = nodeQueue.front();
            nodeQueue.pop();

            // Print the value of the current node
            std::cout << currentNode->value << " ";

            // Enqueue all children of the current node
            for (const auto& child : currentNode->children) {
                nodeQueue.push(child);
            }
        }

        std::cout << std::endl;  // Print a newline after each level
    }
}



// Helper function to convert TokenType enum to a string
std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case STARTWORD: return "STARTWORD";
        case WORD: return "WORD";
        case COMMA: return "COMMA";
        case HYPHEN: return "HYPHEN";
        case STOP: return "STOP";
        case QUOTATION: return "QUOTATION";
        case INVALID: return "INVALID";
        case END: return "END";
        default: return "UNKNOWN";
    }
}



int main() {
    std::string input = "Hello, world-wide communication technologies.";

    Lexer lexer(input);
    std::vector<Token> tokens;

    Token token;
    while ((token = lexer.nextToken()).type != END) {
        if (token.type == INVALID) {
            lexicalErrors.push_back("Invalid token: " + token.value);

        } else {
            tokens.push_back(token);
        }
    }

    std::cout << "Symbol Table: \n";
    for (const auto& tok : tokens) {
        // Use the helper function to convert token type to string
        std::cout << "Token Type: " << tokenTypeToString(tok.type) << " ,Token Value: " << tok.value << '\n';
    }

    // Print errors from lexical phase
    if (!lexicalErrors.empty()) {
        std::cout << "\nLexical Errors: \n";
        for (const auto& err : lexicalErrors) {
            std::cout << err << '\n';
        }
    }


    // Parsing phase
    Parser parser(tokens);
    auto ast = parser.parse();

    if (parser.hasErrors()) {
        std::cout<< "\nThe string is invalid. \n";
        std::cout << "\nParsing Errors: \n";
        parser.printErrors();
    } else {
        std::cout<< "\nThe string is valid. \n";
        std::cout << "\nAccepted String: ";
        parser.printAcceptedString();
        std::cout << "\nAST Structure: \n";
        printASTLevelOrder(ast);            // Print the AST
    }

    return 0;
}
