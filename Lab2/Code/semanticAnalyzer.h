#include "symbolTable.h"
#include "syntaxTree.h"

extern struct TreeNode* treeRoot;


void semanticAnalyzer();

void ExtDefList (struct TreeNode* curNode);

void ExtDef (struct TreeNode* curNode);

void ExtDecList (struct TreeNode* curNode, Type type);

Type Specifier (struct TreeNode* curNode);

Type StructSpecifier (struct TreeNode* curNode);

char* OptTag (struct TreeNode* curNode);

char* Tag (struct TreeNode* curNode);

FieldList VarDec (struct TreeNode* curNode, Type type, int IsStruct);

void FunDec (struct TreeNode* curNode, Type type);

FieldList VarList (struct TreeNode* curNode);

FieldList ParamDec (struct TreeNode* curNode);

void CompSt (struct TreeNode* curNode, Type retType);

void StmtList (struct TreeNode* curNode, Type retType);

void Stmt (struct TreeNode* curNode, Type retType);

FieldList DefList (struct TreeNode* curNode, int IsStruct);

FieldList Def (struct TreeNode* curNode, int IsStruct);

FieldList DecList (struct TreeNode* curNode, Type type, int IsStruct);

FieldList Dec (struct TreeNode* curNode, Type type, int IsStruct);

Type Exp (struct TreeNode* curNode);

int Args (struct TreeNode* curNode, FieldList fieldlist);

void ErrorOut(int errorType, int errorRow, char* msg);

int CompareType(Type type1, Type type2);

int CompareFieldList(FieldList fieldlist1, FieldList fieldlist2);

int InsertFieldList(FieldList fieldlist);

FieldList SearchFieldList(FieldList fieldlist, char* name);