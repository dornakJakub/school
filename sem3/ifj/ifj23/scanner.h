/* IFJ 2023
 * Header for lexical analysis
 * @author xkuril03
 */

#ifndef SCANNER_H
#define SCANNER_H

//Enum for keywords
typedef enum{
    KEYWORD_DOUBLE,
    KEYWORD_ELSE,
    KEYWORD_FUNC,
    KEYWORD_IF,
    KEYWORD_INT,
    KEYWORD_LET,
    KEYWORD_NIL,
    KEYWORD_RETURN,
    KEYWORD_STRING,
    KEYWORD_VAR,
    KEYWORD_WHILE,
    KEYWORD_DOUBLE_OPTIONAL,    // -> Double?
    KEYWORD_INT_OPTIONAL,       // -> Int?
    KEYWORD_STRING_OPTIONAL,    // -> String?
} Keyword;

// Enum for token type
typedef enum{
    TYPE_KEYWORD,
    TYPE_IDENTIFIER,
    TYPE_INTEGER,
    TYPE_DOUBLE,
    TYPE_STRING,
    TYPE_ASSINGMENT,
    TYPE_EQUAL,
    TYPE_NOT_EQUAL,
    TYPE_GREATER,
    TYPE_LESS,
    TYPE_GREATER_OR_EQUAL,
    TYPE_LESS_OR_EQUAL,
    TYPE_ADD,
    TYPE_SUB,
    TYPE_MUL,
    TYPE_DIV,
    TYPE_NOT,
    TYPE_NCO,                   //nil-coalescing operator -> "??"
    TYPE_LEFT_BRACKET,          // -> "{"
    TYPE_RIGHT_BRACKET,         // -> "}"
    TYPE_LEFT_PARENTHESIS,      // -> "("
    TYPE_RIGHT_PARENTHESIS,     // -> ")"
    TYPE_COLON,
    TYPE_COMMA,
    TYPE_SEMICOLON,
    TYPE_RETURN_TYPE,           // -> "->"
    TYPE_UNDERSCORE,
    TYPE_NEWLINE,
    TYPE_EOF,
} Type;

// Union for token attribute
typedef union{
    char *string;
    int integer;
    double decimal;
    Keyword keyword;
} Atribute;

// Struct for token variables
typedef struct{
    Type type;         
    Atribute Atribute;
} Token;

/* Analyzes and sets token variables
 *
 * @param token - Token for analyzed lexeme
 * @return - Return code
 */
int getNextToken(Token* token);

#endif