/* IFJ 2023
 * Implementation of AST
 * @author xdorna06
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "scanner.h"
#include "error.h"
#include "ast.h"


AstNodeResult astInit() {
    AstNode* root = (AstNode*)malloc(sizeof(AstNode));
    if (root == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        AstNodeResult res = {.result = INTERNAL_COMPILER_ERROR, .node = NULL};
        return res;
    }

    root->parentNode = NULL;
    root->childNode = NULL;
    root->nextNode = NULL;
    root->prevNode = NULL;
    root->token = NULL;

    AstNodeResult res = {.result = 0, .node = root};
    return res;
}

AstNodeResult insertAstNode(AstNode* parentNode, Token* token) {
    AstNode* newNode = (AstNode*)malloc(sizeof(AstNode));
    if (newNode == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        AstNodeResult res = {.result = INTERNAL_COMPILER_ERROR, .node = NULL};
        return res;
    }
    newNode->token = (Token*)malloc(sizeof(Token));
    if (newNode->token == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        AstNodeResult res = {.result = INTERNAL_COMPILER_ERROR, .node = NULL};
        return res;
    }

    if (parentNode->childNode == NULL) {
        parentNode->childNode = newNode;
        newNode->prevNode = NULL;   
    }
    else {
        AstNode* iterateNode = parentNode->childNode;
        while (iterateNode->nextNode != NULL) {
            iterateNode = iterateNode->nextNode;
        }
        iterateNode->nextNode = newNode;
        newNode->prevNode = iterateNode;
    }

    newNode->parentNode = parentNode;
    newNode->childNode = NULL;
    newNode->token->Atribute = token->Atribute;
    newNode->token->type = token->type;
    newNode->nextNode = NULL;
    
    AstNodeResult res = {.result = 0, .node = newNode};
    return res;
}

void freeAstChildren(AstNode* node) {
    if (node->childNode) {
        freeAstChildren(node->childNode);
    }
    if(node->nextNode) {
        freeAstChildren(node->nextNode);
    }
    free(node);
}