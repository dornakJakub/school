/* IFJ 2023
 * Implementation of main
 * @author xdorna06
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "error.h"
#include "ast.h"
#include "symtable.h"

//AST build states
typedef enum{
    GRAMMAR_START,
    GRAMMAR_KEYWORD_DATATYPE,
    GRAMMAR_IDENTIFIER,
    GRAMMAR_NEWLINE,
    GRAMMAR_KEYWORD_FUNC,
    GRAMMAR_KEYWORD_IF,
    GRAMMAR_KEYWORD_RETURN,
    GRAMMAR_KEYWORD_WHILE,
    GRAMMAR_ASSIGMENT,
    GRAMMAR_LEFT_PARENTHESIS,
    GRAMMAR_RIGHT_PARENTHESIS,
    GRAMMAR_OPERATOR,
    GRAMMAR_NOT,
    GRAMMAR_VALUE,
    GRAMMAR_EOF
}LLGrammar;

int main() {
    Token* nextToken = malloc(sizeof(Token));
    LLGrammar state = GRAMMAR_START;
    SymTableNode* symTableRoot = NULL;
    bool flagPrevDataType = 0;
    bool flagPrevFunc = 0;
    int bracketHeight = 0;
    int parHeight = 0;
    int returnToken;

    AstNodeResult rootRes = astInit();
    if (rootRes.result != 0) {
        return rootRes.result;
    }
    AstNode* root = rootRes.node; 
    AstNode* parentNode = root;
    returnToken = getNextToken(nextToken);
    if (returnToken != 0) return returnToken;
    // printf("type %d, %d\n",nextToken->type, nextToken->Atribute.keyword);

    while (nextToken->type != TYPE_EOF) {
        switch (state) {
            case GRAMMAR_NEWLINE:
                if (nextToken->type == TYPE_IDENTIFIER) {
                    state = GRAMMAR_IDENTIFIER;
                }
                else if(nextToken->type == TYPE_UNDERSCORE) {
                    state = GRAMMAR_IDENTIFIER;
                }
                if (nextToken->Atribute.keyword == KEYWORD_DOUBLE) {
                    state = GRAMMAR_KEYWORD_DATATYPE;
                }
                else if (nextToken->type == TYPE_LEFT_BRACKET) {
                    state = GRAMMAR_NEWLINE;
                }
                else if (nextToken->type == TYPE_RIGHT_BRACKET) {
                    state = GRAMMAR_NEWLINE;
                }
                else if (nextToken->Atribute.keyword == KEYWORD_FUNC) {
                    state = GRAMMAR_KEYWORD_FUNC;
                }
                else if (nextToken->Atribute.keyword == KEYWORD_IF) {
                    state = GRAMMAR_KEYWORD_IF;
                }
                else if (nextToken->Atribute.keyword == KEYWORD_INT) {
                    state = GRAMMAR_KEYWORD_DATATYPE;
                }
                else if (nextToken->Atribute.keyword == KEYWORD_LET) {
                    state = GRAMMAR_KEYWORD_DATATYPE;
                }
                else if (nextToken->Atribute.keyword == KEYWORD_RETURN) {
                    state = GRAMMAR_KEYWORD_RETURN;
                }
                else if (nextToken->Atribute.keyword == KEYWORD_STRING) {
                    state = GRAMMAR_KEYWORD_DATATYPE;
                }
                else if (nextToken->Atribute.keyword == KEYWORD_VAR) {
                    state = GRAMMAR_KEYWORD_DATATYPE;
                }
                else if (nextToken->Atribute.keyword == KEYWORD_WHILE) {
                    state = GRAMMAR_KEYWORD_WHILE;
                }
                else if (nextToken->Atribute.keyword == KEYWORD_DOUBLE_OPTIONAL) {
                    state = GRAMMAR_KEYWORD_DATATYPE;
                }
                else if (nextToken->Atribute.keyword == KEYWORD_INT_OPTIONAL) {
                    state = GRAMMAR_KEYWORD_DATATYPE;
                }
                else if (nextToken->Atribute.keyword == KEYWORD_STRING_OPTIONAL) {
                    state = GRAMMAR_KEYWORD_DATATYPE;
                }
                else if (nextToken->type == TYPE_NEWLINE) {
                    state = GRAMMAR_START;
                }
                else {
                    fprintf(stderr, "Syntax error\n");
                    return SYNTACTIC_ANALYSIS_ERROR;
                }
                break;
            case GRAMMAR_START:
                if (nextToken->Atribute.keyword == KEYWORD_DOUBLE) {
                    state = GRAMMAR_KEYWORD_DATATYPE;
                }
                else if (nextToken->type == TYPE_LEFT_BRACKET) {
                    state = GRAMMAR_NEWLINE;
                }
                else if (nextToken->type == TYPE_RIGHT_BRACKET) {
                    state = GRAMMAR_NEWLINE;
                }
                else if (nextToken->Atribute.keyword == KEYWORD_FUNC) {
                    state = GRAMMAR_KEYWORD_FUNC;
                }
                else if (nextToken->Atribute.keyword == KEYWORD_IF) {
                    state = GRAMMAR_KEYWORD_IF;
                }
                else if (nextToken->Atribute.keyword == KEYWORD_INT) {
                    state = GRAMMAR_KEYWORD_DATATYPE;
                }
                else if (nextToken->Atribute.keyword == KEYWORD_LET) {
                    state = GRAMMAR_KEYWORD_DATATYPE;
                }
                else if (nextToken->Atribute.keyword == KEYWORD_RETURN) {
                    state = GRAMMAR_KEYWORD_RETURN;
                }
                else if (nextToken->Atribute.keyword == KEYWORD_STRING) {
                    state = GRAMMAR_KEYWORD_DATATYPE;
                }
                else if (nextToken->Atribute.keyword == KEYWORD_VAR) {
                    state = GRAMMAR_KEYWORD_DATATYPE;
                }
                else if (nextToken->Atribute.keyword == KEYWORD_WHILE) {
                    state = GRAMMAR_KEYWORD_WHILE;
                }
                else if (nextToken->Atribute.keyword == KEYWORD_DOUBLE_OPTIONAL) {
                    state = GRAMMAR_KEYWORD_DATATYPE;
                }
                else if (nextToken->Atribute.keyword == KEYWORD_INT_OPTIONAL) {
                    state = GRAMMAR_KEYWORD_DATATYPE;
                }
                else if (nextToken->Atribute.keyword == KEYWORD_STRING_OPTIONAL) {
                    state = GRAMMAR_KEYWORD_DATATYPE;
                }
                else if (nextToken->type == TYPE_NEWLINE) {
                    state = GRAMMAR_START;
                }
                else {
                    fprintf(stderr, "Syntax error\n");
                    return SYNTACTIC_ANALYSIS_ERROR;
                }
                break;
            case GRAMMAR_KEYWORD_DATATYPE:
                if (nextToken->type == TYPE_IDENTIFIER) {
                    state = GRAMMAR_IDENTIFIER;
                }
                else if(nextToken->type == TYPE_UNDERSCORE) {
                    state = GRAMMAR_IDENTIFIER;
                }
                else if (nextToken->type == TYPE_LEFT_BRACKET) {
                    state = GRAMMAR_NEWLINE;
                }
                else {
                    fprintf(stderr, "Syntax error\n");
                    return SYNTACTIC_ANALYSIS_ERROR;
                }
                break;
            case GRAMMAR_IDENTIFIER:
                if (nextToken->type == TYPE_ASSINGMENT) {
                    state = GRAMMAR_ASSIGMENT;
                }
                else if (nextToken->type == TYPE_NEWLINE) {
                    state = GRAMMAR_NEWLINE;
                }
                else if (nextToken->type == TYPE_LEFT_PARENTHESIS) {
                    state = GRAMMAR_LEFT_PARENTHESIS;
                }
                else if (nextToken->type == TYPE_RIGHT_PARENTHESIS) {
                    state = GRAMMAR_RIGHT_PARENTHESIS;
                }
                else if (nextToken->type == TYPE_LEFT_BRACKET) {
                    state = GRAMMAR_NEWLINE;
                }
                else if (nextToken->type == TYPE_RIGHT_BRACKET) {
                    state = GRAMMAR_NEWLINE;
                }
                else if (nextToken->type == TYPE_SEMICOLON) {
                    state = GRAMMAR_NEWLINE;
                }
                else if (nextToken->type == TYPE_EQUAL) {
                    state = GRAMMAR_OPERATOR;
                }
                else if (nextToken->type == TYPE_NOT_EQUAL) {
                    state = GRAMMAR_OPERATOR;
                }
                else if (nextToken->type == TYPE_GREATER) {
                    state = GRAMMAR_OPERATOR;
                }
                else if (nextToken->type == TYPE_LESS) {
                    state = GRAMMAR_OPERATOR;
                }
                else if (nextToken->type == TYPE_GREATER_OR_EQUAL) {
                    state = GRAMMAR_OPERATOR;
                }
                else if (nextToken->type == TYPE_LESS) {
                    state = GRAMMAR_OPERATOR;
                }
                else if (nextToken->type == TYPE_ADD) {
                    state = GRAMMAR_OPERATOR;
                }
                else if (nextToken->type == TYPE_SUB) {
                    state = GRAMMAR_OPERATOR;
                }
                else if (nextToken->type == TYPE_MUL) {
                    state = GRAMMAR_OPERATOR;
                }
                else if (nextToken->type == TYPE_DIV) {
                    state = GRAMMAR_OPERATOR;
                }
                else if (nextToken->type == TYPE_NOT) {
                    state = GRAMMAR_IDENTIFIER;
                }
                else if (nextToken->type == TYPE_NCO) {
                    state = GRAMMAR_IDENTIFIER;
                }
                else {
                    fprintf(stderr, "Syntax error\n");
                    return SYNTACTIC_ANALYSIS_ERROR;
                }
                break;
            case GRAMMAR_ASSIGMENT:
                if (nextToken->type == TYPE_IDENTIFIER) {
                    state = GRAMMAR_IDENTIFIER;
                }
                else if(nextToken->type == TYPE_UNDERSCORE) {
                    state = GRAMMAR_IDENTIFIER;
                }
                else if (nextToken->type == TYPE_LEFT_PARENTHESIS) {
                    state = GRAMMAR_LEFT_PARENTHESIS;
                }
                else if (nextToken->type == TYPE_INTEGER) {
                    state = GRAMMAR_VALUE;
                }
                else if (nextToken->type == TYPE_DOUBLE) {
                    state = GRAMMAR_VALUE;
                }
                else if (nextToken->type == TYPE_STRING) {
                    state = GRAMMAR_VALUE;
                }
                else {
                    fprintf(stderr, "Syntax error\n");
                    return SYNTACTIC_ANALYSIS_ERROR;
                }
                break;
            case GRAMMAR_LEFT_PARENTHESIS:
                if (nextToken->type == TYPE_LEFT_PARENTHESIS) {
                    state = GRAMMAR_LEFT_PARENTHESIS;
                }
                else if (nextToken->type == TYPE_RIGHT_PARENTHESIS) {
                    state = GRAMMAR_RIGHT_PARENTHESIS;
                }
                else if (nextToken->type == TYPE_IDENTIFIER) {
                    state = GRAMMAR_IDENTIFIER;
                }
                else if(nextToken->type == TYPE_UNDERSCORE) {
                    state = GRAMMAR_IDENTIFIER;
                }
                else if (nextToken->type == TYPE_INTEGER) {
                    state = GRAMMAR_VALUE;
                }
                else if (nextToken->type == TYPE_DOUBLE) {
                    state = GRAMMAR_VALUE;
                }
                else if (nextToken->type == TYPE_STRING) {
                    state = GRAMMAR_VALUE;
                }
                else {
                    fprintf(stderr, "Syntax error\n");
                    return SYNTACTIC_ANALYSIS_ERROR;
                }
                break;
            case GRAMMAR_RIGHT_PARENTHESIS:
                if (nextToken->type == TYPE_RIGHT_PARENTHESIS) {
                    state = GRAMMAR_RIGHT_PARENTHESIS;
                }
                else if (nextToken->type == TYPE_EQUAL) {
                    state = GRAMMAR_OPERATOR;
                }
                else if (nextToken->type == TYPE_NOT_EQUAL) {
                    state = GRAMMAR_OPERATOR;
                }
                else if (nextToken->type == TYPE_GREATER) {
                    state = GRAMMAR_OPERATOR;
                }
                else if (nextToken->type == TYPE_LESS) {
                    state = GRAMMAR_OPERATOR;
                }
                else if (nextToken->type == TYPE_GREATER_OR_EQUAL) {
                    state = GRAMMAR_OPERATOR;
                }
                else if (nextToken->type == TYPE_LESS) {
                    state = GRAMMAR_OPERATOR;
                }
                else if (nextToken->type == TYPE_ADD) {
                    state = GRAMMAR_OPERATOR;
                }
                else if (nextToken->type == TYPE_SUB) {
                    state = GRAMMAR_OPERATOR;
                }
                else if (nextToken->type == TYPE_MUL) {
                    state = GRAMMAR_OPERATOR;
                }
                else if (nextToken->type == TYPE_DIV) {
                    state = GRAMMAR_OPERATOR;
                }
                else if (nextToken->type == TYPE_NEWLINE || nextToken->type == TYPE_SEMICOLON) {
                    state = GRAMMAR_NEWLINE;
                }
                else if (nextToken->type == TYPE_RIGHT_BRACKET) {
                    state = GRAMMAR_NEWLINE;
                }
                else if (nextToken->type == TYPE_LEFT_BRACKET) {
                    state = GRAMMAR_NEWLINE;
                }
                else {
                    fprintf(stderr, "Syntax error\n");
                    return SYNTACTIC_ANALYSIS_ERROR;
                }
                break;
            case GRAMMAR_OPERATOR:
                if (nextToken->type == TYPE_IDENTIFIER) {
                    state = GRAMMAR_IDENTIFIER;
                }
                else if(nextToken->type == TYPE_UNDERSCORE) {
                    state = GRAMMAR_IDENTIFIER;
                }
                else if (nextToken->type == TYPE_INTEGER) {
                    state = GRAMMAR_VALUE;
                }
                else if (nextToken->type == TYPE_DOUBLE) {
                    state = GRAMMAR_VALUE;
                }
                else if (nextToken->type == TYPE_STRING) {
                    state = GRAMMAR_VALUE;
                }
                else if (nextToken->type == TYPE_LEFT_PARENTHESIS) {
                    state = GRAMMAR_LEFT_PARENTHESIS;
                }
                else if (nextToken->type == TYPE_LEFT_BRACKET) {
                    state = GRAMMAR_NEWLINE;
                }
                else {
                    fprintf(stderr, "Syntax error\n");
                    return SYNTACTIC_ANALYSIS_ERROR;
                }
                break;
            case GRAMMAR_VALUE:
                if (nextToken->type == TYPE_NEWLINE || nextToken->type == TYPE_SEMICOLON) {
                    state = GRAMMAR_NEWLINE;
                }
                else if (nextToken->type == TYPE_EQUAL) {
                    state = GRAMMAR_OPERATOR;
                }
                else if (nextToken->type == TYPE_NOT_EQUAL) {
                    state = GRAMMAR_OPERATOR;
                }
                else if (nextToken->type == TYPE_GREATER) {
                    state = GRAMMAR_OPERATOR;
                }
                else if (nextToken->type == TYPE_LESS) {
                    state = GRAMMAR_OPERATOR;
                }
                else if (nextToken->type == TYPE_GREATER_OR_EQUAL) {
                    state = GRAMMAR_OPERATOR;
                }
                else if (nextToken->type == TYPE_LESS) {
                    state = GRAMMAR_OPERATOR;
                }
                else if (nextToken->type == TYPE_ADD) {
                    state = GRAMMAR_OPERATOR;
                }
                else if (nextToken->type == TYPE_SUB) {
                    state = GRAMMAR_OPERATOR;
                }
                else if (nextToken->type == TYPE_MUL) {
                    state = GRAMMAR_OPERATOR;
                }
                else if (nextToken->type == TYPE_DIV) {
                    state = GRAMMAR_OPERATOR;
                }
                else {
                    fprintf(stderr, "Syntax error\n");
                    return SYNTACTIC_ANALYSIS_ERROR;
                }
                break;
            case GRAMMAR_KEYWORD_FUNC:
                    if (nextToken->type == TYPE_IDENTIFIER) {
                    state = GRAMMAR_IDENTIFIER;
                }
                else if(nextToken->type == TYPE_UNDERSCORE) {
                    state = GRAMMAR_IDENTIFIER;
                }
                else {
                    fprintf(stderr, "Syntax error\n");
                    return SYNTACTIC_ANALYSIS_ERROR;
                }
                break;
            case GRAMMAR_KEYWORD_IF:
                    if (nextToken->type == TYPE_LEFT_PARENTHESIS) {
                    state = GRAMMAR_LEFT_PARENTHESIS;
                }
                else {
                    fprintf(stderr, "Syntax error\n");
                    return SYNTACTIC_ANALYSIS_ERROR;
                }
                break;
            case GRAMMAR_KEYWORD_RETURN:
                if (nextToken->type == TYPE_IDENTIFIER) {
                    state = GRAMMAR_IDENTIFIER;
                }
                else if(nextToken->type == TYPE_UNDERSCORE) {
                    state = GRAMMAR_IDENTIFIER;
                }
                else if (nextToken->type == TYPE_INTEGER) {
                    state = GRAMMAR_VALUE;
                }
                else if (nextToken->type == TYPE_DOUBLE) {
                    state = GRAMMAR_VALUE;
                }
                else if (nextToken->type == TYPE_STRING) {
                    state = GRAMMAR_VALUE;
                }
                else {
                    fprintf(stderr, "Syntax error\n");
                    return SYNTACTIC_ANALYSIS_ERROR;
                }
                break;
            case GRAMMAR_KEYWORD_WHILE:
                if (nextToken->type == TYPE_LEFT_PARENTHESIS) {
                    state = GRAMMAR_LEFT_PARENTHESIS;
                }
                else {
                    fprintf(stderr, "Syntax error\n");
                    return SYNTACTIC_ANALYSIS_ERROR;
                }
                break;
            default:
                fprintf(stderr, "Syntax  analysis state error\n");
                return INTERNAL_COMPILER_ERROR;
            break;
        }

        AstNodeResult insertNodeRes;
        switch (nextToken->type) {
            case TYPE_KEYWORD:  //Go down one level
            case TYPE_ASSINGMENT:
            case TYPE_EQUAL:
            case TYPE_NOT_EQUAL:
            case TYPE_GREATER:
            case TYPE_LESS:
            case TYPE_GREATER_OR_EQUAL:
            case TYPE_LESS_OR_EQUAL:
            case TYPE_ADD:
            case TYPE_SUB:
            case TYPE_MUL:
            case TYPE_DIV:
            case TYPE_NOT:
            case TYPE_NCO:
                insertNodeRes = insertAstNode(parentNode, nextToken);
                if (insertNodeRes.result != 0) {
                    return insertNodeRes.result;
                }
                parentNode = insertNodeRes.node;
                break;
            case TYPE_LEFT_BRACKET:
                bracketHeight++;
                insertNodeRes = insertAstNode(parentNode, nextToken);
                if (insertNodeRes.result != 0) {
                    return insertNodeRes.result;
                }
                parentNode = insertNodeRes.node;
                break;
            case TYPE_LEFT_PARENTHESIS:
                parHeight++;
                AstNodeResult insertNodeRes;
                insertNodeRes = insertAstNode(parentNode, nextToken);
                if (insertNodeRes.result != 0) {
                    return insertNodeRes.result;
                }
                parentNode = insertNodeRes.node;
                break;
            case TYPE_IDENTIFIER: //Stay at the same level
            case TYPE_INTEGER:
            case TYPE_DOUBLE:
            case TYPE_STRING:
            case TYPE_UNDERSCORE:
            case TYPE_RETURN_TYPE:
            case TYPE_SEMICOLON:
            case TYPE_NEWLINE:
                insertNodeRes = insertAstNode(parentNode, nextToken);
                if (insertNodeRes.result != 0) {
                    return insertNodeRes.result;
                }
                break;
            case TYPE_RIGHT_BRACKET: //go up until }
                bracketHeight--;
                if (bracketHeight < 0) {
                    fprintf(stderr, "Syntax  analysis state error\n");
                    return INTERNAL_COMPILER_ERROR;
                }
                while (parentNode->token->type != TYPE_LEFT_BRACKET) {
                    parentNode = parentNode->parentNode;
                }
                insertNodeRes = insertAstNode(parentNode, nextToken);
                if (insertNodeRes.result != 0) {
                    return insertNodeRes.result;
                }
                break;
            case TYPE_RIGHT_PARENTHESIS:    //go up until )
                parHeight--;
                if (parHeight < 0) {
                    fprintf(stderr, "Syntax  analysis state error\n");
                    return INTERNAL_COMPILER_ERROR;
                }
                while (parentNode->token->type != TYPE_LEFT_PARENTHESIS) {
                    parentNode = parentNode->parentNode;
                }
                insertNodeRes = insertAstNode(parentNode, nextToken);
                if (insertNodeRes.result != 0) {
                    return insertNodeRes.result;
                }
                break;
            default:
            break;
        }


        if (state == GRAMMAR_IDENTIFIER && flagPrevFunc) { //zkontrolovat ze funkce jeste neni definovana a nastavit isDefined true isFunction true
            SymTableNode* nodeExists = searchNode(symTableRoot, nextToken->Atribute.string);
            if (bracketHeight != 0) {
                fprintf(stderr, "Definition of function in scope\n");
                return OTHER_SEMANTIC_ERRORS;
            }
            if (nodeExists && nodeExists->data.isDefined) {
                fprintf(stderr, "Redefinition of function\n");
                return VARIABLE_REDEFINITION_ERROR;
            }
            else if (nodeExists && !nodeExists->data.isFunction) {
                fprintf(stderr, "Definition of function with a name of already declared variable\n");
                return VARIABLE_REDEFINITION_ERROR;
            }
            else if (nodeExists && !nodeExists->data.isDefined) {
                nodeExists->data.isDefined = true;
            }
            else {
                nodeExists = insertNode(symTableRoot, nextToken->Atribute.string);
                nodeExists->data.isDefined = true;
                nodeExists->data.isFunction = true;
            }
            flagPrevFunc = 0;
            flagPrevDataType = 0;
        }
        else if (state == GRAMMAR_IDENTIFIER && flagPrevDataType) { //Zkontrolovat ze promenna jeste neni definovana a nastavit isDefined na true isFunction false
            SymTableNode* nodeExists = searchNode(symTableRoot, nextToken->Atribute.string);
            if (nodeExists && nodeExists->data.isDefined) {
                fprintf(stderr, "Redefinition of variable\n");
                return VARIABLE_REDEFINITION_ERROR;
            }
            else if (nodeExists && nodeExists->data.isFunction) {
                fprintf(stderr, "Variable definition with name of already defined function\n");
                return VARIABLE_REDEFINITION_ERROR;
            }
            else {
                nodeExists = insertNode(symTableRoot, nextToken->Atribute.string);
                nodeExists->data.isDefined = true;
                nodeExists->data.isFunction = false;
                nodeExists->height = bracketHeight;
            }
            flagPrevDataType = 0;
            flagPrevFunc = 0;
        }
        else if (state == GRAMMAR_IDENTIFIER) {  //Je promenna definovana. Najdi symnode, ?IsFunction
            SymTableNode* nodeExists = searchNode(symTableRoot, nextToken->Atribute.string);
            if (!nodeExists) {
                SymTableNode* newNode = insertNode(symTableRoot, nextToken->Atribute.string);
                newNode->data.isFunction = true;
                newNode->data.isDefined = false;
            }
            else if (nodeExists->height > bracketHeight) {
                fprintf(stderr, "Undefined function\n");
                return NON_INITIALIZED_VARIABLE;
            }
        }
        else if (state == GRAMMAR_KEYWORD_DATATYPE) {
            flagPrevDataType = 1;
        }
        else if (state == GRAMMAR_KEYWORD_FUNC) {
            flagPrevFunc = 1;
        }
        else {
            flagPrevFunc = 0;
            flagPrevDataType = 0;
        }

        returnToken = getNextToken(nextToken);
        if (returnToken != 0) return returnToken;
        // printf("type %d, %d\n",nextToken->type, nextToken->Atribute.keyword);
    }
    state = GRAMMAR_EOF;

    free(nextToken);
    freeAstChildren(root);
    // freeSymtable(symTableRoot);
}