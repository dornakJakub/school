/* IFJ 2023
 * Lexical analysis implementation
 * @author xkuril03
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "scanner.h"
#include "error.h"

// Scanner states
typedef enum{
    STATE_START,
    STATE_EOF,
    STATE_FORWARDSLASH,
    STATE_SINGLE_LINE_COMMENT,
    STATE_MULTI_LINE_COMMENT_START,
    STATE_MULTI_LINE_COMMENT_ASTERISK,
    STATE_UNDERSCORE,
    STATE_DASH,
    STATE_IDENTIFIER,
    STATE_DIGIT,
    STATE_DECIMAL,
    STATE_DECIMAL_START,
    STATE_EXPONENT,
    STATE_EXPONENT_SIGN,
    STATE_EXPONENT_START,
    STATE_STRING,
    STATE_STRING_SPECIAL,
    STATE_EQUAL,
    STATE_GREATER,
    STATE_LESS,
    STATE_NOT
}ScannerState;

/* Checks if string is keyword
 *
 * @param token - Token for analyzed lexeme
 * @param str - Lexeme
 * @return - Is or is not keyword
 */
bool checkKeyword(Token *token, char *str){
    if(!strcmp(str, "Double")){
        token->Atribute.keyword = KEYWORD_DOUBLE;
    }else if(!strcmp(str, "else")){
        token->Atribute.keyword = KEYWORD_ELSE;
    }else if(!strcmp(str, "func")){
        token->Atribute.keyword = KEYWORD_FUNC;
    }else if(!strcmp(str, "if")){
        token->Atribute.keyword = KEYWORD_IF;
    }else if(!strcmp(str, "Int")){
        token->Atribute.keyword = KEYWORD_INT;
    }else if(!strcmp(str, "let")){
        token->Atribute.keyword = KEYWORD_LET;
    }else if(!strcmp(str, "nil")){
        token->Atribute.keyword = KEYWORD_NIL;
    }else if(!strcmp(str, "return")){
        token->Atribute.keyword = KEYWORD_RETURN;
    }else if(!strcmp(str, "String")){
        token->Atribute.keyword = KEYWORD_STRING;
    }else if(!strcmp(str, "var")){
        token->Atribute.keyword = KEYWORD_VAR;
    }else if(!strcmp(str, "while")){
        token->Atribute.keyword = KEYWORD_WHILE;
    }else if(!strcmp(str, "Double?")){
        token->Atribute.keyword = KEYWORD_DOUBLE_OPTIONAL;
    }else if(!strcmp(str, "Int?")){
        token->Atribute.keyword = KEYWORD_INT_OPTIONAL;
    }else if(!strcmp(str, "String?")){
        token->Atribute.keyword = KEYWORD_STRING_OPTIONAL;
    }else{
        return false;
    }

    token->type = TYPE_KEYWORD;
    return true;
}

/* Reallocates string and appends character
 *
 * @param str - Pointer to a string
 * @param c - Char to append
 * @param returnCode - Return code
 */
void appendCharacter(char **str, char c, int *returnCode) {
    size_t len = strlen(*str);
    *str = (char *)realloc(*str, len + 2);
    if(*str == NULL){
        fprintf(stderr, "Memory allocation error\n");
        *returnCode = INTERNAL_COMPILER_ERROR;
        return;
    }
    (*str)[len] = c;
    (*str)[len + 1] = '\0';
}

/* Pushes char back to stdin and ends lexical analyses
 *
 * @param c - Char to push back
 * @param flag - Flag to stop analysis
 */
void tokenEnd(char c, bool *flag){
    ungetc(c, stdin);
    *flag = false;
}

/* Sets token attribute
 * 
 * @param token - Token for analyzed lexeme
 * @param str - Lexeme that might be set in attribute
 * @param returnCode - Return Code
 */
void setTokenAttribute(Token *token, char *str, int *returnCode) {
    switch(token->type){
        case TYPE_IDENTIFIER:
        case TYPE_STRING:
            token->Atribute.string = (char *)malloc(strlen(str) + 1);
            if(token->Atribute.string != NULL) {
                strcpy(token->Atribute.string, str);
            }else{
                fprintf(stderr, "Memory allocation error\n");
                *returnCode = INTERNAL_COMPILER_ERROR;
            }
            break;
        case TYPE_INTEGER:
            token->Atribute.integer = atoi(str);
            break;
        case TYPE_DOUBLE:
            char *endPtr;
            token->Atribute.decimal = strtod(str, &endPtr);
            if(*endPtr != '\0') {
                fprintf(stderr, "Conversion to double failed\n");
                *returnCode = INTERNAL_COMPILER_ERROR;
            }
            break;
        default:
            fprintf(stderr, "TOKEN TYPE ERROR\n");
            *returnCode = INTERNAL_COMPILER_ERROR;
            break;
    }
}

/* Analyzes and sets token variables
 *
 * @param token - Token for analyzed lexeme
 * @return - Return code
 */
int getNextToken(Token* token) {
    int returnCode = 0;
    bool flag = true;
    char c;
    ScannerState state = STATE_START;
    token->Atribute.string = NULL;
    
    char *str = NULL;

    str = (char *)malloc(sizeof(char));
    if (str == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        return INTERNAL_COMPILER_ERROR;
    }
    str[0] = '\0';
    while(flag){
        c = getchar();
        switch (state){
            case STATE_START:
                if(c == EOF) state = STATE_EOF;
                else if(c == '/') state = STATE_FORWARDSLASH;
                else if(c == '_'){
                    state = STATE_UNDERSCORE;
                    appendCharacter(&str, c, &returnCode);
                }else if(isdigit(c)){
                    state = STATE_DIGIT;
                    appendCharacter(&str, c, &returnCode);
                }else if(isalpha(c)){
                    state = STATE_IDENTIFIER;
                    appendCharacter(&str, c, &returnCode);
                }else if(c == '('){
                    token->type = TYPE_LEFT_PARENTHESIS;
                    flag = false;
                }else if(c == ')'){
                    token->type = TYPE_RIGHT_PARENTHESIS;
                    flag = false;
                }else if(c == '{'){
                    token->type = TYPE_LEFT_BRACKET;
                    flag = false;
                }else if(c == '}'){
                    token->type = TYPE_RIGHT_BRACKET;
                    flag = false;
                }else if(c == '"'){
                    state = STATE_STRING;
                    appendCharacter(&str, c, &returnCode);
                }else if(c == '+'){
                    token->type = TYPE_ADD;
                    flag = false;
                }else if(c == '-'){
                    state = STATE_DASH;
                }else if(c == '*'){
                    token->type = TYPE_MUL;
                    flag = false;
                }else if(c == '!'){
                    state = STATE_NOT;
                }else if(c == '='){
                    state = STATE_EQUAL;
                }else if(c == '>'){
                    state = STATE_GREATER;
                }else if(c == '<'){
                    state = STATE_LESS;
                }else if(c == '\n'){
                    token->type = TYPE_NEWLINE;
                    flag = false;
                }else if(c == ';'){
                    token->type = TYPE_SEMICOLON;
                    flag = false;
                }else if(c == ' ' || c == '\n'){

                }else{
                    fprintf(stderr, "Lexeme error\n");
                    returnCode = LEXICAL_ANALYSIS_ERROR;
                    flag = false;
                }
                break;
            
            case STATE_FORWARDSLASH:
                if(c == EOF) {
                    fprintf(stderr, "Lexeme error\n");
                    returnCode = LEXICAL_ANALYSIS_ERROR;
                    flag = false;
                }
                else if(c == '/') state = STATE_SINGLE_LINE_COMMENT;
                else if(c == '*') state = STATE_MULTI_LINE_COMMENT_START;
                else {
                    token->type = TYPE_DIV;
                    tokenEnd(c, &flag);
                }
                break;

            case STATE_SINGLE_LINE_COMMENT:
                if(c == EOF) state = STATE_EOF;
                else if(c == '\n') state = STATE_START;
                break;

            case STATE_MULTI_LINE_COMMENT_START:
                if(c == EOF) {
                    fprintf(stderr, "Lexeme error\n");
                    returnCode = LEXICAL_ANALYSIS_ERROR;
                    flag = false;
                }else if(c == '*') state = STATE_MULTI_LINE_COMMENT_ASTERISK;
                break;

            case STATE_MULTI_LINE_COMMENT_ASTERISK:
                if(c == EOF){
                    fprintf(stderr, "Lexeme error\n");
                    returnCode = LEXICAL_ANALYSIS_ERROR;
                    flag = false;
                }else if(c == '/') state = STATE_START;
                else if(c != '*') state = STATE_MULTI_LINE_COMMENT_START; 
                break;

            case STATE_UNDERSCORE:
                if(isalpha(c) || isdigit(c)){
                    state = STATE_IDENTIFIER;
                    appendCharacter(&str, c, &returnCode);
                }else if(c == ' '){
                    token->type = TYPE_UNDERSCORE;
                    flag = false;
                }else{
                    fprintf(stderr, "Lexeme structure error\n");
                    returnCode = LEXICAL_ANALYSIS_ERROR;
                    flag = false;
                }
                break;

            case STATE_DIGIT:
                if(isdigit(c)){
                    appendCharacter(&str, c, &returnCode);
                }else if(c == '.'){
                    appendCharacter(&str, c, &returnCode);
                    state = STATE_DECIMAL_START;
                }else if(c == 'e' || c == 'E'){
                    appendCharacter(&str, c, &returnCode);
                    state = STATE_EXPONENT_SIGN;
                }else{
                    tokenEnd(c, &flag);
                    token->type = TYPE_INTEGER;
                    setTokenAttribute(token, str, &returnCode);
                }
                break;

            case STATE_DECIMAL_START:
                if(isdigit(c)){
                    appendCharacter(&str, c, &returnCode);
                    state = STATE_DECIMAL;
                }else{
                    fprintf(stderr, "Lexeme structure error - decimal\n");
                    returnCode = LEXICAL_ANALYSIS_ERROR;
                    flag = false;
                }
                break;

            case STATE_DECIMAL:
                if(isdigit(c)){
                    appendCharacter(&str, c, &returnCode);
                }else if(c == 'e' || c == 'E'){
                    appendCharacter(&str, c, &returnCode);
                    state = STATE_EXPONENT_SIGN;
                }else{
                    tokenEnd(c, &flag);
                    token->type = TYPE_DOUBLE;
                    setTokenAttribute(token, str, &returnCode);
                }
                break;

            case STATE_EXPONENT_SIGN:
                if(c == '+' || c == '-'){
                    appendCharacter(&str, c, &returnCode);
                    state = STATE_EXPONENT_START;
                }else if(isdigit(c)){
                    appendCharacter(&str, c, &returnCode);
                    state = STATE_EXPONENT;
                }else{
                    fprintf(stderr, "Lexeme structure error - exponent\n");
                    returnCode = LEXICAL_ANALYSIS_ERROR;
                    flag = false;
                }
                break;

            case STATE_EXPONENT_START:
                if(isdigit(c)){
                    appendCharacter(&str, c, &returnCode);
                    state = STATE_EXPONENT;
                }else{
                    fprintf(stderr, "Lexeme structure error - exponent\n");
                    returnCode = LEXICAL_ANALYSIS_ERROR;
                    flag = false;
                }
                break;

            case STATE_EXPONENT:
                if(isdigit(c)){
                    appendCharacter(&str, c, &returnCode);
                }else{
                    tokenEnd(c, &flag);
                    token->type = TYPE_DOUBLE;
                    setTokenAttribute(token, str, &returnCode);
                }
                break;
            
            case STATE_IDENTIFIER:
                if(c == EOF){
                    tokenEnd(c, &flag);
                }else if(isalpha(c) || isdigit(c)){
                    appendCharacter(&str, c, &returnCode);
                }else if(c == '?'){
                    appendCharacter(&str, c, &returnCode);
                    if(!checkKeyword(token, str)){
                        fprintf(stderr, "Lexeme structure error - identifier\n");
                        returnCode = LEXICAL_ANALYSIS_ERROR;
                        flag = false;
                    }
                }else{
                    tokenEnd(c, &flag);
                    if(!checkKeyword(token, str)){
                        token->type = TYPE_IDENTIFIER;
                        setTokenAttribute(token, str, &returnCode);
                    }
                }

                break;

            case STATE_STRING:
                if(c == EOF){
                    fprintf(stderr, "Lexeme error\n");
                    returnCode = LEXICAL_ANALYSIS_ERROR;
                    flag = false;
                }else if(c == '\\'){
                    state = STATE_STRING_SPECIAL;
                    appendCharacter(&str, c, &returnCode);
                }else if(c == '\"'){
                    appendCharacter(&str, c, &returnCode);
                    flag = false;
                    token->type = TYPE_STRING;
                    setTokenAttribute(token, str, &returnCode);
                }else{
                    appendCharacter(&str, c, &returnCode);
                }
                break;

            case STATE_STRING_SPECIAL:
                if(c == EOF){
                    fprintf(stderr, "Lexeme error\n");
                    returnCode = LEXICAL_ANALYSIS_ERROR;
                    flag = false;
                }else{
                    state = STATE_STRING;
                    appendCharacter(&str, c, &returnCode);
                }
                state = STATE_STRING;
                break;

            case STATE_DASH:
                if(c == EOF){
                    fprintf(stderr, "Lexeme error\n");
                    returnCode = LEXICAL_ANALYSIS_ERROR;
                    flag = false;
                }else if(c == '>'){
                    token->type = TYPE_NCO;
                    flag = false;
                }else{
                    token->type = TYPE_SUB;
                    tokenEnd(c, &flag);
                }
                break;
            case STATE_EQUAL:
                if(c == EOF){
                    fprintf(stderr, "Lexeme error\n");
                    returnCode = LEXICAL_ANALYSIS_ERROR;
                    flag = false;
                }else if(c == '='){
                    token->type = TYPE_EQUAL;
                    flag = false;
                }else{
                    token->type = TYPE_ASSINGMENT;
                    tokenEnd(c, &flag);
                }
                break;
            case STATE_GREATER:
                if(c == EOF){
                    fprintf(stderr, "Lexeme error\n");
                    returnCode = LEXICAL_ANALYSIS_ERROR;
                    flag = false;
                }else if(c == '='){
                    token->type = TYPE_GREATER_OR_EQUAL;
                    flag = false;
                }else{
                    token->type = TYPE_GREATER;
                    tokenEnd(c, &flag);
                }
                break;
            case STATE_LESS:
                if(c == EOF){
                    fprintf(stderr, "Lexeme error\n");
                    returnCode = LEXICAL_ANALYSIS_ERROR;
                    flag = false;
                }else if(c == '='){
                    token->type = TYPE_LESS_OR_EQUAL;
                    flag = false;
                }else{
                    token->type = TYPE_LESS;
                    tokenEnd(c, &flag);
                }
                break;
            case STATE_NOT:
                if(c == EOF){
                    fprintf(stderr, "Lexeme error\n");
                    returnCode = LEXICAL_ANALYSIS_ERROR;
                    flag = false;
                }else if(c == '='){
                    token->type = TYPE_NOT_EQUAL;
                    flag = false;
                }else{
                    token->type = TYPE_NOT;
                    tokenEnd(c, &flag);
                }
                break;
            case STATE_EOF:
                token->type = TYPE_EOF;
                flag = false;
                break;
        }
    }
    
    free(str);
    return 0;
}