/* IFJ 2023
 * Header for AST
 * @author xdorna06
 */

#ifndef AST_H
#define AST_H
#include "scanner.h"


//Struct of AST node, childNode is next level of AST, nextNode is part of the same leaf
typedef struct AstNode {
    Token *token;
    struct AstNode* childNode;
    struct AstNode* nextNode;
    struct AstNode* prevNode;
    struct AstNode* parentNode;
} AstNode;

typedef struct {
    int result;
    AstNode* node;
} AstNodeResult;

//First function to call with first token, initializes AST
//To be changed to int to return error codes
AstNodeResult astInit();

//Inserts new node and returns it
AstNodeResult insertAstNode(AstNode* parentNode, Token* Token);

//Deletes node with all it's children
void freeAstChildren(AstNode* node);

#endif