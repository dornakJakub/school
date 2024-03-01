/* IFJ 2023
 * Header for symtable
 * @author xkuril03
 * @author xdorna06
 */

#ifndef SYMTABLE_H
#define SYMTABLE_H
#include <stdbool.h>

typedef enum{
    SYMTABLE_TYPE_INT,
    SYMTABLE_TYPE_OPTIONAL_INT,
    SYMTABLE_TYPE_DOUBLE,
    SYMTABLE_TYPE_OPTIONAL_DOUBLE,
    SYMTABLE_TYPE_STRING,
    SYMTABLE_TYPE_OPTIONAL_STRING,
    SYMTABLE_TYPE_VOID,
} SymbolType;

typedef struct {
    SymbolType type;
    char *params;
    bool isFunction;
    bool isDefined;
} SymbolData;

typedef struct SymTableNode{
    char *key;
    SymbolData data;
    struct SymTableNode *left;
    struct SymTableNode *right;
    int height;
} SymTableNode;

SymTableNode* insertNode(SymTableNode* node, char* key);

SymTableNode* createNode(char* key);

SymTableNode* rotateRight(SymTableNode* node);

SymTableNode* rotateLeft(SymTableNode* node);

SymTableNode* searchNode(SymTableNode* node, char* key);

int max(int a, int b);

int balanceFactor(SymTableNode* node);

int height(SymTableNode* node);

void updateHeight(SymTableNode* node);

void setType(SymTableNode* node, SymbolType type);

bool isFunction(SymTableNode* node);

bool isDefined(SymTableNode* node);

void setFunction(SymTableNode* node);

void setDefined(SymTableNode* node);

void setParam(SymTableNode* node, char* param);

void freeSymtable(SymTableNode* node);

#endif