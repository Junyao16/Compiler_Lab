#ifndef INTER_CODE
#define INTER_CODE

#include "syntaxTree.h"
#include "symbolTable.h"

extern struct TreeNode *treeRoot;

typedef struct Operand_ *Operand;

struct Operand_
{
    enum
    {
        OP_VARIABLE,
        OP_CONSTANT,
        OP_ADDRESS,
        OP_LABEL,
        OP_FUNCTION,
        OP_TMPVAR
    } kind;
    union {
        int var_no;
        int label_no;
        int tmp_no;
        int value;
        char funName[32];
    } u;
};

struct InterCode
{
    enum
    {
        IC_LABEL,
        IC_FUNCTION,
        IC_ASSIGN,
        IC_ADD,
        IC_SUB,
        IC_MUL,
        IC_DIV,
        IC_GET_CONTENT,
        IC_ASG_CONTENT,
        IC_GOTO,
        IC_IF,
        IC_RETURN,
        IC_DEC,
        IC_ARG,
        IC_CALL,
        IC_PARAM,
        IC_READ,
        IC_WRITE
    } kind;

    union {
        struct
        {
            Operand op;
        } single;

        struct
        {
            Operand right, left;
        } assign;

        struct
        {
            Operand result, op1, op2;
        } binop;
        struct
        {
            Operand op1, op2, op3;
            char *relop;
        } ifStmt;
    } u;
};

struct InterCodes
{
    struct InterCode code;
    struct InterCodes *prev, *next;
};

void PrintOp(Operand op, FILE *fout);

void PrintInterCodes(FILE *fout);

Operand CreateOperand(int kind, int val, char *funName);

struct InterCodes *CreateInterCode(int kind, Operand op1, Operand op2, Operand result, char *relop);

void InsertInterCode(struct InterCodes *newInterCode);

void GenerateInterCode();

//translate functions
void translate_ExtDefList(struct TreeNode *curNode);

void translate_ExtDef(struct TreeNode *curNode);

Operand translate_VarDec(struct TreeNode *curNode);

void translate_FunDec(struct TreeNode *curNode);

void translate_CompSt(struct TreeNode *curNode);

void translate_StmtList(struct TreeNode *curNode);

struct InterCodes *translate_Stmt(struct TreeNode *curNode);

void translate_DefList(struct TreeNode *curNode);

struct InterCodes *translate_Def(struct TreeNode *curNode);

struct InterCodes *translate_DecList(struct TreeNode *curNode);

struct InterCodes *translate_Dec(struct TreeNode *curNode);

Operand translate_Exp(struct TreeNode *curNode);

void translate_Args(struct TreeNode *curNode);

void translate_Cond(struct TreeNode *curNode, Operand label_true, Operand label_false);

#endif