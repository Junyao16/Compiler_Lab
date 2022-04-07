#include <stdio.h>
#include "semanticAnalyzer.h"

void semanticAnalyzer() {
    InitSymbolTable();
    if(treeRoot != NULL && strcmp(treeRoot->nodeName, "Program") == 0){
        ExtDefList(treeRoot->firstChild);
    }
}

void ExtDefList (struct TreeNode* curNode){

}