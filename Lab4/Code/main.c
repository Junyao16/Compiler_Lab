#include <stdio.h>

#include "syntax.tab.h"
#include "syntaxTree.h"
#include "semanticAnalyzer.h"
#include "interCode.h"
#include "objectCode.h"

extern FILE *yyin;
extern struct TreeNode *treeRoot;
extern int haveError;

extern int yyrestart(FILE *);
extern int yyparse();
int yylex();

int main(int argc, char **argv)
{
    if (argc <= 1)
        return 1;
    FILE *f = fopen(argv[1], "r");
    if (!f)
    {
        perror(argv[1]);
        return 1;
    }

    yyrestart(f);
    yyparse();

    if (!haveError)
        semanticAnalyzer();

    if (!haveError)
    {
        GenerateInterCode();
    }
    if (!haveError)
    {
        FILE *fout = fopen(argv[2], "w+");
        if (fout == NULL)
        {
            perror(argv[2]);
            return 1;
        }
        GenerateObjectCode(fout);
        fclose(fout);
    }
    fclose(f);

    return 0;
}