/* IFJ 2023
 * Symtable implementation
 * @author xkuril03
 * @author xdorna06
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtable.h"

SymTableNode* insertNode(SymTableNode* node, char* key){
    if(node == NULL){
        return createNode(key);
    }

    int comparison = strcmp(node->key, key);
    if(comparison > 0){
        node->right = insertNode(node->right, key);
    }else if(comparison < 0){
        node->left = insertNode(node->left, key);
    }else{
        //error
    }

    node->height = max(height(node->right), height(node->left)) + 1;

    int balance = balanceFactor(node);
    if(balance > 1 && comparison < 0){
        return rotateRight(node);
    }
    if(balance < -1 && comparison > 0){
        return rotateLeft(node);
    }
    if(balance > 1 && comparison > 0){
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }
    if(balance < -1 && comparison < 0){
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }

    return node;
}

SymTableNode* createNode(char* key){
    SymTableNode* newNode = (SymTableNode*)malloc(sizeof(SymTableNode));
    if(newNode == NULL){
        //error
    }

    newNode->key = (char*)malloc(sizeof(char) * (strlen(key) + 1));
    if(newNode->key == NULL){
        //error
    }
    strcpy(newNode->key, key);
    newNode->right = NULL;
    newNode->left = NULL;
    newNode->height = 1;

    return newNode;
}

SymTableNode* rotateRight(SymTableNode* node){
    SymTableNode* newTop = node->left;
    SymTableNode* T2 = newTop->right;

    newTop->right = node;
    node->left = T2;
    node->height = max(height(node->right), height(node->left)) + 1;
    newTop->height = max(height(newTop->right), height(newTop->left)) + 1;
    return newTop;
}

SymTableNode* rotateLeft(SymTableNode* node){
    SymTableNode* newTop = node->right;
    SymTableNode* T2 = newTop->left;

    newTop->left = node;
    node->right = T2;
    node->height = max(height(node->right), height(node->left)) + 1;
    newTop->height = max(height(newTop->right), height(newTop->left)) + 1;
    return newTop;
}

SymTableNode* searchNode(SymTableNode* node, char* key){
    if(node == NULL){
        return NULL;
    }

    int comparison = strcmp(node->key, key);
    if(comparison > 0){
        return searchNode(node->right, key);
    }else if(comparison < 0){
        return searchNode(node->left, key);
    }else{
        return node;
    }
}

int max(int a, int b) {
    if(a > b) return a;
    return b;
}

int balanceFactor(SymTableNode* node){
    if(node == NULL) return 0;
    return height(node->left) - height(node->right);
}

int height(SymTableNode* node){
    if(node == NULL) return 0;
    return node->height;
}

void updateHeight(SymTableNode* node){
    if(node != NULL){
        node->height = max(height(node->left), height(node->right)) + 1;
    }
}

void setType(SymTableNode* node, SymbolType type) {
    if (node != NULL) {
        node->data.type = type;
    }
}

bool isFunction(SymTableNode* node) {
    if(node == NULL){
        //error
    }
    return node->data.isFunction;
}

bool isDefined(SymTableNode* node) {
    if(node == NULL){
        //error
    } 
    return node->data.isDefined;
}

void setFunction(SymTableNode* node) {
    if (node != NULL) {
        node->data.isFunction = true;
    }
}

void setDefined(SymTableNode* node) {
    if (node != NULL) {
        node->data.isDefined = true;
    }
}

void setParam(SymTableNode* node, char* param) {
    if (node != NULL) {
        size_t size = strlen(node->data.params) + strlen(param) + 2;
        node->data.params = realloc(node->data.params, size);

        strcat(node->data.params, " ");
        strcat(node->data.params, param);
    }
}

void freeSymtable(SymTableNode* node) {
    if (node->left != NULL) {
        freeSymtable(node->left);
    }
    if (node->right != NULL) {
        freeSymtable(node->right);
    }
    free(node);
}
