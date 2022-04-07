#include "syntaxTree.h"

struct TreeNode* createTreeNode(char* name, char* value, enum TYPE_NODE type, int row){
    struct TreeNode* newNode = (struct TreeNode*)malloc(sizeof(struct TreeNode));
    newNode->firstChild = NULL;
    newNode->nextSibling = NULL;
    newNode->nodeType = type;
    strcpy(newNode->nodeName, name);
    if(value == NULL){
        newNode->nodeValue.stringValue[0] = '\0';
    }
    else{
        switch (type){
            case TYPE_INT: {
                newNode->nodeValue.intValue = atoi(value);
                break;
            }
            case TYPE_FLOAT: {
                newNode->nodeValue.floatValue = atof(value);
                break;
            }
            case TYPE_OTHER: {
                strcpy(newNode->nodeValue.stringValue, value);
                break;
            }
            case TYPE_NONTERMINAL: {
                strcpy(newNode->nodeValue.stringValue, value);
                break;
            }
        }            
    }
    newNode->nodeRow = row;
    return newNode;
}




void addTreeNodeChild(struct TreeNode*parent, struct TreeNode*child){
    if(parent == NULL || child == NULL){
        return;
    }
    else{
        if(parent->firstChild == NULL){
            parent->firstChild = child;
        }
        else{
            struct TreeNode* tmp = parent->firstChild;
            while(tmp->nextSibling != NULL){
                tmp = tmp->nextSibling;
            }
            tmp->nextSibling = child;
        }
    }
}



void printTree(struct TreeNode* node, int nodeDepth){
    if(node == NULL){
        return;
    }
    for(int i = 0; i < nodeDepth; i++){
        printf("  ");
    }
    //printf("%d\n", node->nodeType);
    switch(node->nodeType){
        case TYPE_INT: {
            printf("%s: %d\n", node->nodeName, node->nodeValue.intValue);
            break;
        }
        case TYPE_FLOAT: {
            printf("%s: %f\n", node->nodeName, node->nodeValue.floatValue);
            break;
        }
        case TYPE_OTHER: {
            if(node->nodeValue.stringValue[0] == '\0'){
                printf("%s\n", node->nodeName);
            }
            else{
                printf("%s: %s\n", node->nodeName, node->nodeValue.stringValue);
            }
            break;
        }
        case TYPE_NONTERMINAL: {
            printf("%s (%d)\n", node->nodeName, node->nodeRow);
            break;
        }
    }
    if(node -> firstChild != NULL){
        printTree(node->firstChild, nodeDepth+1);
    }
    if(node -> nextSibling != NULL){
        printTree(node->nextSibling, nodeDepth);
    }
}
