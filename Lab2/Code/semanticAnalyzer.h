#include "symbolTable.h"
#include "syntaxTree.h"

extern struct TreeNode* treeRoot;

void semanticAnalyzer();

void ExtDefList (struct TreeNode* curNode);