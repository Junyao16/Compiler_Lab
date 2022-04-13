#include <stdio.h>

#include "syntax.tab.h"
#include "semanticAnalyzer.h"
#include "syntaxTree.h"

extern FILE* yyin;
extern struct TreeNode* treeRoot;
extern int haveError;

extern int yyrestart(FILE*);
extern int yyparse();
int yylex();

int main(int argc, char** argv)
{
    if (argc <= 1) return 1;
    FILE* f = fopen(argv[1], "r");
    if (!f)
    {
        perror(argv[1]);
        return 1;
    }
    yyrestart(f);
    yyparse();

    if(!haveError) semanticAnalyzer();


    return 0;
}