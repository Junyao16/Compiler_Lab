#ifndef SYNTAX_TREE
#define SYNTAX_TREE

#include <stdio.h>
#include <string.h>

enum TYPE_NODE
{
    TYPE_INT = 0,
    TYPE_FLOAT = 1,
    TYPE_OTHER = 2,
    TYPE_NONTERMINAL = 3
};

/* syntax tree node */
struct TreeNode
{
    struct TreeNode *firstChild;
    struct TreeNode *nextSibling;
    char nodeName[32];
    union {
        int intValue;
        float floatValue;
        char stringValue[32];
    } nodeValue;
    enum TYPE_NODE nodeType;
    int nodeRow;
};

struct TreeNode *createTreeNode(char *name, char *value, enum TYPE_NODE type, int row);

void addTreeNodeChild(struct TreeNode *parent, struct TreeNode *child);

void printTree(struct TreeNode *node, int nodeDepth);

#endif