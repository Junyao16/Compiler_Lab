#include <stdio.h>
#include <stdlib.h>
#include "interCode.h"

int var_count;
int tmp_count;
int label_count;

struct InterCodes *codeHead = NULL;
struct InterCodes *codeTail = NULL;

void PrintOp(Operand op, FILE *fout)
{
    switch (op->kind)
    {
    case OP_VARIABLE:
    {
        fprintf(fout, "v%d", op->u.var_no);
        break;
    }
    case OP_CONSTANT:
    {
        fprintf(fout, "#%d", op->u.value);
        break;
    }
    case OP_ADDRESS:
    {
        fprintf(fout, "&v%d", op->u.var_no);
        break;
    }
    case OP_LABEL:
    {
        fprintf(fout, "label%d", op->u.label_no);
        break;
    }
    case OP_FUNCTION:
    {
        fprintf(fout, "%s", op->u.funName);
        break;
    }
    case OP_TMPVAR:
    {
        fprintf(fout, "t%d", op->u.tmp_no);
        break;
    }
    }
}

void PrintInterCodes(FILE *fout)
{
    struct InterCodes *tmp = codeHead->next;
    while (tmp != codeHead)
    {
        switch (tmp->code.kind)
        {
        case IC_LABEL:
        {
            fprintf(fout, "LABEL ");
            PrintOp(tmp->code.u.single.op, fout);
            fprintf(fout, " :\n");
            break;
        }
        case IC_FUNCTION:
        {
            fprintf(fout, "FUNCTION ");
            PrintOp(tmp->code.u.single.op, fout);
            fprintf(fout, " :\n");
            break;
        }
        case IC_ASSIGN:
        {
            PrintOp(tmp->code.u.assign.left, fout);
            fprintf(fout, " := ");
            PrintOp(tmp->code.u.assign.right, fout);
            fprintf(fout, "\n");
            break;
        }
        case IC_ADD:
        {
            PrintOp(tmp->code.u.binop.result, fout);
            fprintf(fout, " := ");
            PrintOp(tmp->code.u.binop.op1, fout);
            fprintf(fout, " + ");
            PrintOp(tmp->code.u.binop.op2, fout);
            fprintf(fout, "\n");
            break;
        }
        case IC_SUB:
        {
            PrintOp(tmp->code.u.binop.result, fout);
            fprintf(fout, " := ");
            PrintOp(tmp->code.u.binop.op1, fout);
            fprintf(fout, " - ");
            PrintOp(tmp->code.u.binop.op2, fout);
            fprintf(fout, "\n");
            break;
        }
        case IC_MUL:
        {
            PrintOp(tmp->code.u.binop.result, fout);
            fprintf(fout, " := ");
            PrintOp(tmp->code.u.binop.op1, fout);
            fprintf(fout, " * ");
            PrintOp(tmp->code.u.binop.op2, fout);
            fprintf(fout, "\n");
            break;
        }
        case IC_DIV:
        {
            PrintOp(tmp->code.u.binop.result, fout);
            fprintf(fout, " := ");
            PrintOp(tmp->code.u.binop.op1, fout);
            fprintf(fout, " / ");
            PrintOp(tmp->code.u.binop.op2, fout);
            fprintf(fout, "\n");
            break;
        }
        case IC_GET_CONTENT:
        {
            PrintOp(tmp->code.u.assign.left, fout);
            fprintf(fout, " := *");
            PrintOp(tmp->code.u.assign.right, fout);
            fprintf(fout, "\n");
            break;
        }
        case IC_ASG_CONTENT:
        {
            fprintf(fout, "*");
            PrintOp(tmp->code.u.assign.left, fout);
            fprintf(fout, " := ");
            PrintOp(tmp->code.u.assign.right, fout);
            fprintf(fout, "\n");
            break;
        }
        case IC_GOTO:
        {
            fprintf(fout, "GOTO ");
            PrintOp(tmp->code.u.single.op, fout);
            fprintf(fout, "\n");
            break;
        }
        case IC_IF:
        {
            fprintf(fout, "IF ");
            PrintOp(tmp->code.u.ifStmt.op1, fout);
            fprintf(fout, " %s ", tmp->code.u.ifStmt.relop);
            PrintOp(tmp->code.u.ifStmt.op2, fout);
            fprintf(fout, " GOTO ");
            PrintOp(tmp->code.u.ifStmt.op3, fout);
            fprintf(fout, "\n");
            break;
        }
        case IC_RETURN:
        {
            fprintf(fout, "RETURN ");
            PrintOp(tmp->code.u.single.op, fout);
            fprintf(fout, "\n");
            break;
        }
        case IC_DEC:
        {
            fprintf(fout, "DEC ");
            PrintOp(tmp->code.u.assign.left, fout);
            fprintf(fout, " [");
            PrintOp(tmp->code.u.assign.right, fout);
            fprintf(fout, "]\n");
            break;
        }
        case IC_ARG:
        {
            fprintf(fout, "ARG ");
            PrintOp(tmp->code.u.single.op, fout);
            fprintf(fout, "\n");
            break;
        }
        case IC_CALL:
        {
            PrintOp(tmp->code.u.assign.left, fout);
            fprintf(fout, " := CALL ");
            PrintOp(tmp->code.u.assign.right, fout);
            fprintf(fout, "\n");
            break;
        }
        case IC_PARAM:
        {
            fprintf(fout, "PARAM ");
            PrintOp(tmp->code.u.single.op, fout);
            fprintf(fout, "\n");
            break;
        }
        case IC_READ:
        {
            fprintf(fout, "READ ");
            PrintOp(tmp->code.u.single.op, fout);
            fprintf(fout, "\n");
            break;
        }
        case IC_WRITE:
        {
            fprintf(fout, "WRITE ");
            PrintOp(tmp->code.u.single.op, fout);
            fprintf(fout, "\n");
            break;
        }
        }
        tmp = tmp->next;
    }
}

Operand CreateOperand(int kind, int val, char *funName)
{
    Operand newOperand = (Operand)malloc(sizeof(struct Operand_));
    newOperand->kind = kind;
    switch (kind)
    {
    case OP_VARIABLE:
    {
        newOperand->u.var_no = var_count;
        var_count++;
        break;
    }
    case OP_CONSTANT:
    {
        newOperand->u.value = val;
        break;
    }
    case OP_ADDRESS:
    {
        newOperand->u.var_no = var_count;
        var_count++;
        break;
    }
    case OP_LABEL:
    {
        newOperand->u.label_no = label_count;
        label_count++;
        break;
    }
    case OP_FUNCTION:
    {
        strcpy(newOperand->u.funName, funName);
        break;
    }
    case OP_TMPVAR:
    {
        newOperand->u.tmp_no = tmp_count;
        tmp_count++;
        break;
    }
    }
    return newOperand;
}

struct InterCodes *CreateInterCode(int kind, Operand op1, Operand op2, Operand result, char *relop)
{
    struct InterCodes *newInterCode = (struct InterCodes *)malloc(sizeof(struct InterCodes));
    newInterCode->next = NULL;
    newInterCode->prev = NULL;
    newInterCode->code.kind = kind;
    switch (kind)
    {
    case IC_LABEL:
    case IC_FUNCTION:
    case IC_GOTO:
    case IC_RETURN:
    case IC_ARG:
    case IC_PARAM:
    case IC_READ:
    case IC_WRITE:
    {
        newInterCode->code.u.single.op = op1;
        break;
    }

    case IC_ASSIGN:
    case IC_GET_CONTENT:
    case IC_ASG_CONTENT:
    case IC_DEC:
    case IC_CALL:
    {
        newInterCode->code.u.assign.left = op1;
        newInterCode->code.u.assign.right = op2;
        break;
    }

    case IC_ADD:
    case IC_SUB:
    case IC_MUL:
    case IC_DIV:
    {
        newInterCode->code.u.binop.result = result;
        newInterCode->code.u.binop.op1 = op1;
        newInterCode->code.u.binop.op2 = op2;
        break;
    }

    case IC_IF:
    {
        newInterCode->code.u.ifStmt.op1 = op1;
        newInterCode->code.u.ifStmt.op2 = op2;
        newInterCode->code.u.ifStmt.op3 = result;
        newInterCode->code.u.ifStmt.relop = relop;
        break;
    }
    }

    return newInterCode;
}

void InsertInterCode(struct InterCodes *newInterCode)
{
    codeTail->next = newInterCode;
    newInterCode->next = codeHead;
    codeHead->prev = newInterCode;
    newInterCode->prev = codeTail;
    codeTail = newInterCode;
}

void GenerateInterCode()
{
    codeHead = (struct InterCodes *)malloc(sizeof(struct InterCodes));
    codeHead->next = codeHead;
    codeHead->prev = codeHead;
    codeTail = codeHead;

    var_count = 0;
    tmp_count = 0;
    label_count = 0;

    // Program → ExtDefList
    if (treeRoot != NULL && strcmp(treeRoot->nodeName, "Program") == 0)
    {
        translate_ExtDefList(treeRoot->firstChild);
    }
}

// ExtDefList → ExtDef ExtDefList
// | empty
void translate_ExtDefList(struct TreeNode *curNode)
{
    if (curNode != NULL && strcmp(curNode->nodeName, "ExtDefList") == 0)
    {
        if (curNode->firstChild != NULL)
        {
            translate_ExtDef(curNode->firstChild);
            if (curNode->firstChild->nextSibling != NULL)
            {
                translate_ExtDefList(curNode->firstChild->nextSibling);
            }
        }
    }
}

// ExtDef → Specifier ExtDecList SEMI
// | Specifier SEMI
// | Specifier FunDec CompSt
void translate_ExtDef(struct TreeNode *curNode)
{
    if (curNode != NULL && strcmp(curNode->nodeName, "ExtDef") == 0)
    {
        if (curNode->firstChild != NULL)
        {
            if (curNode->firstChild->nextSibling != NULL)
            {
                if (strcmp(curNode->firstChild->nextSibling->nodeName, "FunDec") == 0)
                {
                    translate_FunDec(curNode->firstChild->nextSibling);
                    if (curNode->firstChild->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nodeName, "CompSt") == 0)
                    {
                        translate_CompSt(curNode->firstChild->nextSibling->nextSibling);
                    }
                }
            }
        }
    }
}

// VarDec → ID
// | VarDec LB INT RB
Operand translate_VarDec(struct TreeNode *curNode)
{
    if (curNode != NULL && strcmp(curNode->nodeName, "VarDec") == 0)
    {
        if (curNode->firstChild != NULL)
        {
            if (strcmp(curNode->firstChild->nodeName, "ID") == 0)
            {
                Operand newOp = CreateOperand(OP_VARIABLE, -1, NULL);
                SearchSymbol(curNode->firstChild->nodeValue.stringValue)->var_no = newOp->u.var_no;
                return newOp;
            }
            else if (strcmp(curNode->firstChild->nodeName, "VarDec") == 0)
            {
                return translate_VarDec(curNode->firstChild);
            }
        }
    }
}

// FunDec → ID LP VarList RP
// | ID LP RP
void translate_FunDec(struct TreeNode *curNode)
{
    if (curNode != NULL && strcmp(curNode->nodeName, "FunDec") == 0)
    {
        if (curNode->firstChild != NULL && strcmp(curNode->firstChild->nodeName, "ID") == 0)
        {
            Operand newOp = CreateOperand(OP_FUNCTION, -1, curNode->firstChild->nodeValue.stringValue);
            struct InterCodes *newInterCode = CreateInterCode(IC_FUNCTION, newOp, NULL, NULL, NULL);
            InsertInterCode(newInterCode);
            SymbolNode tmpSymbol = SearchSymbol(newOp->u.funName);
            FieldList paralist = tmpSymbol->type->u.function.paraList;
            while (paralist != NULL)
            {
                Operand tmpOp = CreateOperand(OP_VARIABLE, -1, NULL);
                struct InterCodes *tmpInterCode = CreateInterCode(IC_PARAM, tmpOp, NULL, NULL, NULL);
                InsertInterCode(tmpInterCode);
                SearchSymbol(paralist->name)->var_no = tmpOp->u.var_no;
                paralist = paralist->tail;
            }
        }
    }
}

// CompSt → LC DefList StmtList RC
void translate_CompSt(struct TreeNode *curNode)
{
    if (curNode != NULL && strcmp(curNode->nodeName, "CompSt") == 0)
    {
        if (curNode->firstChild != NULL && strcmp(curNode->firstChild->nodeName, "LC") == 0)
        {
            if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "DefList") == 0)
            {
                translate_DefList(curNode->firstChild->nextSibling);
                if (curNode->firstChild->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nodeName, "StmtList") == 0)
                {
                    translate_StmtList(curNode->firstChild->nextSibling->nextSibling);
                }
            }
            if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "StmtList") == 0)
            {
                translate_StmtList(curNode->firstChild->nextSibling);
            }
        }
    }
}

// StmtList → Stmt StmtList
// | empty
void translate_StmtList(struct TreeNode *curNode)
{
    if (curNode != NULL && strcmp(curNode->nodeName, "StmtList") == 0)
    {
        if (curNode->firstChild != NULL && strcmp(curNode->firstChild->nodeName, "Stmt") == 0)
        {
            translate_Stmt(curNode->firstChild);
            if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "StmtList") == 0)
            {
                translate_StmtList(curNode->firstChild->nextSibling);
            }
        }
    }
}

// Stmt → Exp SEMI
// | CompSt
// | RETURN Exp SEMI
// | IF LP Exp RP Stmt
// | IF LP Exp RP Stmt ELSE Stmt
// | WHILE LP Exp RP Stmt
struct InterCodes *translate_Stmt(struct TreeNode *curNode)
{
    if (curNode != NULL && strcmp(curNode->nodeName, "Stmt") == 0)
    {
        if (curNode->firstChild != NULL)
        {
            if (strcmp(curNode->firstChild->nodeName, "Exp") == 0)
            {
                translate_Exp(curNode->firstChild);
            }
            else if (strcmp(curNode->firstChild->nodeName, "CompSt") == 0)
            {
                translate_CompSt(curNode->firstChild);
            }
            else if (strcmp(curNode->firstChild->nodeName, "RETURN") == 0)
            {
                if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "Exp") == 0)
                {
                    Operand newOp = translate_Exp(curNode->firstChild->nextSibling);
                    struct InterCodes *newInterCode = CreateInterCode(IC_RETURN, newOp, NULL, NULL, NULL);
                    InsertInterCode(newInterCode);
                }
            }
            else if (strcmp(curNode->firstChild->nodeName, "IF") == 0)
            {
                if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "LP") == 0)
                {
                    Operand label1 = CreateOperand(OP_LABEL, -1, NULL);
                    Operand label2 = CreateOperand(OP_LABEL, -1, NULL);
                    if (curNode->firstChild->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nodeName, "Exp") == 0)
                    {

                        translate_Cond(curNode->firstChild->nextSibling->nextSibling, label1, label2);
                        struct InterCodes *newInterCode = CreateInterCode(IC_LABEL, label1, NULL, NULL, NULL);
                        InsertInterCode(newInterCode);
                        if (curNode->firstChild->nextSibling->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nextSibling->nodeName, "RP") == 0)
                        {
                            if (curNode->firstChild->nextSibling->nextSibling->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nextSibling->nextSibling->nodeName, "Stmt") == 0)
                            {
                                translate_Stmt(curNode->firstChild->nextSibling->nextSibling->nextSibling->nextSibling);
                                if (curNode->firstChild->nextSibling->nextSibling->nextSibling->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nextSibling->nextSibling->nextSibling->nodeName, "ELSE") == 0)
                                {
                                    Operand label3 = CreateOperand(OP_LABEL, -1, NULL);
                                    newInterCode = CreateInterCode(IC_GOTO, label3, NULL, NULL, NULL);
                                    InsertInterCode(newInterCode);
                                    if (curNode->firstChild->nextSibling->nextSibling->nextSibling->nextSibling->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nextSibling->nextSibling->nextSibling->nextSibling->nodeName, "Stmt") == 0)
                                    {
                                        newInterCode = CreateInterCode(IC_LABEL, label2, NULL, NULL, NULL);
                                        InsertInterCode(newInterCode);
                                        translate_Stmt(curNode->firstChild->nextSibling->nextSibling->nextSibling->nextSibling->nextSibling->nextSibling);
                                        newInterCode = CreateInterCode(IC_LABEL, label3, NULL, NULL, NULL);
                                        InsertInterCode(newInterCode);
                                    }
                                }
                                else
                                {
                                    newInterCode = CreateInterCode(IC_LABEL, label2, NULL, NULL, NULL);
                                    InsertInterCode(newInterCode);
                                }
                            }
                        }
                    }
                }
            }
            else if (strcmp(curNode->firstChild->nodeName, "WHILE") == 0)
            {
                if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "LP") == 0)
                {
                    Operand label1 = CreateOperand(OP_LABEL, -1, NULL);
                    Operand label2 = CreateOperand(OP_LABEL, -1, NULL);
                    Operand label3 = CreateOperand(OP_LABEL, -1, NULL);
                    struct InterCodes *newInterCode = CreateInterCode(IC_LABEL, label1, NULL, NULL, NULL);
                    InsertInterCode(newInterCode);
                    if (curNode->firstChild->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nodeName, "Exp") == 0)
                    {

                        translate_Cond(curNode->firstChild->nextSibling->nextSibling, label2, label3);
                        newInterCode = CreateInterCode(IC_LABEL, label2, NULL, NULL, NULL);
                        InsertInterCode(newInterCode);
                        if (curNode->firstChild->nextSibling->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nextSibling->nodeName, "RP") == 0)
                        {
                            if (curNode->firstChild->nextSibling->nextSibling->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nextSibling->nextSibling->nodeName, "Stmt") == 0)
                            {
                                translate_Stmt(curNode->firstChild->nextSibling->nextSibling->nextSibling->nextSibling);
                                newInterCode = CreateInterCode(IC_GOTO, label1, NULL, NULL, NULL);
                                InsertInterCode(newInterCode);
                                newInterCode = CreateInterCode(IC_LABEL, label3, NULL, NULL, NULL);
                                InsertInterCode(newInterCode);
                            }
                        }
                    }
                }
            }
        }
    }
}

// DefList → Def DefList
// | empty
void translate_DefList(struct TreeNode *curNode)
{
    if (curNode != NULL && strcmp(curNode->nodeName, "DefList") == 0)
    {
        if (curNode->firstChild != NULL && strcmp(curNode->firstChild->nodeName, "Def") == 0)
        {
            translate_Def(curNode->firstChild);
            if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "DefList") == 0)
            {
                translate_DefList(curNode->firstChild->nextSibling);
            }
        }
    }
}

// Def → Specifier DecList SEMI
struct InterCodes *translate_Def(struct TreeNode *curNode)
{
    if (curNode != NULL && strcmp(curNode->nodeName, "Def") == 0)
    {
        if (curNode->firstChild != NULL && strcmp(curNode->firstChild->nodeName, "Specifier") == 0)
        {
            if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "DecList") == 0)
            {
                translate_DecList(curNode->firstChild->nextSibling);
            }
        }
    }
}

// DecList → Dec
// | Dec COMMA DecList
struct InterCodes *translate_DecList(struct TreeNode *curNode)
{
    if (curNode != NULL && strcmp(curNode->nodeName, "DecList") == 0)
    {
        if (curNode->firstChild != NULL && strcmp(curNode->firstChild->nodeName, "Dec") == 0)
        {
            translate_Dec(curNode->firstChild);
            if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "COMMA") == 0)
            {
                if (curNode->firstChild->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nodeName, "DecList") == 0)
                {
                    translate_DecList(curNode->firstChild->nextSibling->nextSibling);
                }
            }
        }
    }
}

// Dec → VarDec
// | VarDec ASSIGNOP Exp
struct InterCodes *translate_Dec(struct TreeNode *curNode)
{
    if (curNode != NULL && strcmp(curNode->nodeName, "Dec") == 0)
    {
        if (curNode->firstChild != NULL && strcmp(curNode->firstChild->nodeName, "VarDec") == 0)
        {
            Operand newOp1 = translate_VarDec(curNode->firstChild);
            if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "ASSIGNOP") == 0)
            {
                if (curNode->firstChild->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nodeName, "Exp") == 0)
                {
                    Operand newOp2 = translate_Exp(curNode->firstChild->nextSibling->nextSibling);
                    struct InterCodes *newInterCode = CreateInterCode(IC_ASSIGN, newOp1, newOp2, NULL, NULL);
                    InsertInterCode(newInterCode);
                }
            }
        }
    }
}

// Exp → Exp ASSIGNOP Exp
// | Exp AND Exp
// | Exp OR Exp
// | Exp RELOP Exp
// | Exp PLUS Exp
// | Exp MINUS Exp
// | Exp STAR Exp
// | Exp DIV Exp
// | LP Exp RP
// | MINUS Exp
// | NOT Exp
// | ID LP Args RP
// | ID LP RP
// | Exp LB Exp RB
// | Exp DOT ID
// | ID
// | INT
// | FLOAT
Operand translate_Exp(struct TreeNode *curNode)
{
    if (curNode != NULL && strcmp(curNode->nodeName, "Exp") == 0)
    {
        if (curNode->firstChild != NULL)
        {
            if (strcmp(curNode->firstChild->nodeName, "Exp") == 0)
            {
                Operand newOp1 = translate_Exp(curNode->firstChild);
                if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "ASSIGNOP") == 0)
                {
                    if (curNode->firstChild->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nodeName, "Exp") == 0)
                    {
                        Operand newOp2 = translate_Exp(curNode->firstChild->nextSibling->nextSibling);
                        if (strcmp(curNode->firstChild->firstChild->nodeName, "ID") == 0 && curNode->firstChild->firstChild->nextSibling == NULL)
                        {
                            struct InterCodes *newInterCode = CreateInterCode(IC_ASSIGN, newOp1, newOp2, NULL, NULL);
                            InsertInterCode(newInterCode);
                            Operand newOp = newOp1;
                            return newOp;
                        }
                        else if (strcmp(curNode->firstChild->firstChild->nodeName, "Exp") == 0 && curNode->firstChild->firstChild->nextSibling != NULL)
                        {
                            if (strcmp(curNode->firstChild->firstChild->nextSibling->nodeName, "LB") == 0 || strcmp(curNode->firstChild->firstChild->nextSibling->nodeName, "DOT") == 0)
                            {
                                //TODO:
                            }
                        }
                    }
                }
                else if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "AND") == 0)
                {
                    if (curNode->firstChild->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nodeName, "Exp") == 0)
                    {
                        Operand newLabel1 = CreateOperand(OP_LABEL, -1, NULL);
                        Operand newLabel2 = CreateOperand(OP_LABEL, -1, NULL);
                        Operand newOp = CreateOperand(OP_TMPVAR, -1, NULL);
                        Operand newConstant = CreateOperand(OP_CONSTANT, 0, NULL);
                        struct InterCodes *newInterCode = CreateInterCode(IC_ASSIGN, newOp, newConstant, NULL, NULL);
                        InsertInterCode(newInterCode);
                        translate_Cond(curNode, newLabel1, newLabel2);
                        newInterCode = CreateInterCode(IC_LABEL, newLabel1, NULL, NULL, NULL);
                        InsertInterCode(newInterCode);
                        newConstant = CreateOperand(OP_CONSTANT, 1, NULL);
                        newInterCode = CreateInterCode(IC_ASSIGN, newOp, newConstant, NULL, NULL);
                        InsertInterCode(newInterCode);
                        newInterCode = CreateInterCode(IC_LABEL, newLabel2, NULL, NULL, NULL);
                        InsertInterCode(newInterCode);
                        return newOp;
                    }
                }
                else if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "OR") == 0)
                {
                    if (curNode->firstChild->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nodeName, "Exp") == 0)
                    {
                        Operand newLabel1 = CreateOperand(OP_LABEL, -1, NULL);
                        Operand newLabel2 = CreateOperand(OP_LABEL, -1, NULL);
                        Operand newOp = CreateOperand(OP_TMPVAR, -1, NULL);
                        Operand newConstant = CreateOperand(OP_CONSTANT, 0, NULL);
                        struct InterCodes *newInterCode = CreateInterCode(IC_ASSIGN, newOp, newConstant, NULL, NULL);
                        InsertInterCode(newInterCode);
                        translate_Cond(curNode, newLabel1, newLabel2);
                        newInterCode = CreateInterCode(IC_LABEL, newLabel1, NULL, NULL, NULL);
                        InsertInterCode(newInterCode);
                        newConstant = CreateOperand(OP_CONSTANT, 1, NULL);
                        newInterCode = CreateInterCode(IC_ASSIGN, newOp, newConstant, NULL, NULL);
                        InsertInterCode(newInterCode);
                        newInterCode = CreateInterCode(IC_LABEL, newLabel2, NULL, NULL, NULL);
                        InsertInterCode(newInterCode);
                        return newOp;
                    }
                }
                else if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "RELOP") == 0)
                {
                    if (curNode->firstChild->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nodeName, "Exp") == 0)
                    {
                        Operand newLabel1 = CreateOperand(OP_LABEL, -1, NULL);
                        Operand newLabel2 = CreateOperand(OP_LABEL, -1, NULL);
                        Operand newOp = CreateOperand(OP_TMPVAR, -1, NULL);
                        Operand newConstant = CreateOperand(OP_CONSTANT, 0, NULL);
                        struct InterCodes *newInterCode = CreateInterCode(IC_ASSIGN, newOp, newConstant, NULL, NULL);
                        InsertInterCode(newInterCode);
                        translate_Cond(curNode, newLabel1, newLabel2);
                        newInterCode = CreateInterCode(IC_LABEL, newLabel1, NULL, NULL, NULL);
                        InsertInterCode(newInterCode);
                        newConstant = CreateOperand(OP_CONSTANT, 1, NULL);
                        newInterCode = CreateInterCode(IC_ASSIGN, newOp, newConstant, NULL, NULL);
                        InsertInterCode(newInterCode);
                        newInterCode = CreateInterCode(IC_LABEL, newLabel2, NULL, NULL, NULL);
                        InsertInterCode(newInterCode);
                        return newOp;
                    }
                }
                else if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "PLUS") == 0)
                {
                    if (curNode->firstChild->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nodeName, "Exp") == 0)
                    {
                        Operand newOp2 = translate_Exp(curNode->firstChild->nextSibling->nextSibling);
                        Operand newOp = CreateOperand(OP_TMPVAR, -1, NULL);
                        struct InterCodes *newInterCode = CreateInterCode(IC_ADD, newOp1, newOp2, newOp, NULL);
                        InsertInterCode(newInterCode);
                        return newOp;
                    }
                }
                else if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "MINUS") == 0)
                {
                    if (curNode->firstChild->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nodeName, "Exp") == 0)
                    {
                        Operand newOp2 = translate_Exp(curNode->firstChild->nextSibling->nextSibling);
                        Operand newOp = CreateOperand(OP_TMPVAR, -1, NULL);
                        struct InterCodes *newInterCode = CreateInterCode(IC_SUB, newOp1, newOp2, newOp, NULL);
                        InsertInterCode(newInterCode);
                        return newOp;
                    }
                }
                else if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "STAR") == 0)
                {
                    if (curNode->firstChild->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nodeName, "Exp") == 0)
                    {
                        Operand newOp2 = translate_Exp(curNode->firstChild->nextSibling->nextSibling);
                        Operand newOp = CreateOperand(OP_TMPVAR, -1, NULL);
                        struct InterCodes *newInterCode = CreateInterCode(IC_MUL, newOp1, newOp2, newOp, NULL);
                        InsertInterCode(newInterCode);
                        return newOp;
                    }
                }
                else if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "DIV") == 0)
                {
                    if (curNode->firstChild->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nodeName, "Exp") == 0)
                    {
                        Operand newOp2 = translate_Exp(curNode->firstChild->nextSibling->nextSibling);
                        Operand newOp = CreateOperand(OP_TMPVAR, -1, NULL);
                        struct InterCodes *newInterCode = CreateInterCode(IC_DIV, newOp1, newOp2, newOp, NULL);
                        InsertInterCode(newInterCode);
                        return newOp;
                    }
                }
                else if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "LB") == 0)
                {
                    if (curNode->firstChild->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nodeName, "Exp") == 0)
                    {
                        //TODO:
                        //Type type2 = Exp(curNode->firstChild->nextSibling->nextSibling);
                    }
                }
                else if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "DOT") == 0)
                {
                    if (curNode->firstChild->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nodeName, "ID") == 0)
                    {
                        //TODO:
                    }
                }
            }
            else if (strcmp(curNode->firstChild->nodeName, "LP") == 0)
            {
                if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "Exp") == 0)
                {
                    return translate_Exp(curNode->firstChild->nextSibling);
                }
            }
            else if (strcmp(curNode->firstChild->nodeName, "MINUS") == 0)
            {
                if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "Exp") == 0)
                {
                    Operand newTmp = translate_Exp(curNode->firstChild->nextSibling);
                    Operand newOp = CreateOperand(OP_TMPVAR, -1, NULL);
                    Operand newConstant = CreateOperand(OP_CONSTANT, 0, NULL);
                    struct InterCodes *newInterCode = CreateInterCode(IC_SUB, newConstant, newTmp, newOp, NULL);
                    InsertInterCode(newInterCode);
                    return newOp;
                }
            }
            else if (strcmp(curNode->firstChild->nodeName, "NOT") == 0)
            {
                if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "Exp") == 0)
                {
                    Operand newLabel1 = CreateOperand(OP_LABEL, -1, NULL);
                    Operand newLabel2 = CreateOperand(OP_LABEL, -1, NULL);
                    Operand newOp = CreateOperand(OP_TMPVAR, -1, NULL);
                    Operand newConstant = CreateOperand(OP_CONSTANT, 0, NULL);
                    struct InterCodes *newInterCode = CreateInterCode(IC_ASSIGN, newOp, newConstant, NULL, NULL);
                    InsertInterCode(newInterCode);
                    translate_Cond(curNode, newLabel1, newLabel2);
                    newInterCode = CreateInterCode(IC_LABEL, newLabel1, NULL, NULL, NULL);
                    InsertInterCode(newInterCode);
                    newConstant = CreateOperand(OP_CONSTANT, 1, NULL);
                    newInterCode = CreateInterCode(IC_ASSIGN, newOp, newConstant, NULL, NULL);
                    InsertInterCode(newInterCode);
                    newInterCode = CreateInterCode(IC_LABEL, newLabel2, NULL, NULL, NULL);
                    InsertInterCode(newInterCode);
                    return newOp;
                }
            }
            else if (strcmp(curNode->firstChild->nodeName, "ID") == 0)
            {
                SymbolNode symbolnode = SearchSymbol(curNode->firstChild->nodeValue.stringValue);
                if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "LP") == 0)
                {

                    if (curNode->firstChild->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nodeName, "Args") == 0)
                    {
                        if (strcmp(curNode->firstChild->nodeValue.stringValue, "write") == 0)
                        {
                            Operand newOp = translate_Exp(curNode->firstChild->nextSibling->nextSibling->firstChild);
                            struct InterCodes *newInterCode = CreateInterCode(IC_WRITE, newOp, NULL, NULL, NULL);
                            InsertInterCode(newInterCode);
                            newOp = CreateOperand(OP_CONSTANT, 0, NULL);
                            return newOp;
                        }
                        else
                        {
                            translate_Args(curNode->firstChild->nextSibling->nextSibling);
                            Operand newOp = CreateOperand(OP_TMPVAR, -1, NULL);
                            Operand funOp = CreateOperand(OP_FUNCTION, -1, curNode->firstChild->nodeValue.stringValue);
                            struct InterCodes *newInterCode = CreateInterCode(IC_CALL, newOp, funOp, NULL, NULL);
                            InsertInterCode(newInterCode);
                            return newOp;
                        }
                    }
                    else if (curNode->firstChild->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nodeName, "RP") == 0)
                    {
                        if (strcmp(curNode->firstChild->nodeValue.stringValue, "read") == 0)
                        {
                            Operand newOp = CreateOperand(OP_TMPVAR, -1, NULL);
                            struct InterCodes *newInterCode = CreateInterCode(IC_READ, newOp, NULL, NULL, NULL);
                            InsertInterCode(newInterCode);
                            return newOp;
                        }
                        else
                        {
                            Operand newOp = CreateOperand(OP_TMPVAR, -1, NULL);
                            Operand funOp = CreateOperand(OP_FUNCTION, -1, curNode->firstChild->nodeValue.stringValue);
                            struct InterCodes *newInterCode = CreateInterCode(IC_CALL, newOp, funOp, NULL, NULL);
                            InsertInterCode(newInterCode);
                            return newOp;
                        }
                    }
                }
                else
                {
                    Operand newOp = CreateOperand(OP_VARIABLE, -1, NULL);
                    var_count--;
                    newOp->u.var_no = symbolnode->var_no;
                    return newOp;
                }
            }
            else if (strcmp(curNode->firstChild->nodeName, "INT") == 0)
            {
                Operand newOp = CreateOperand(OP_CONSTANT, curNode->firstChild->nodeValue.intValue, NULL);
                return newOp;
            }
        }
    }
    return NULL;
}

// Args → Exp COMMA Args
// | Exp
void translate_Args(struct TreeNode *curNode)
{
    if (curNode != NULL && strcmp(curNode->nodeName, "Args") == 0)
    {
        if (curNode->firstChild != NULL && strcmp(curNode->firstChild->nodeName, "Exp") == 0)
        {
            Operand newOp = translate_Exp(curNode->firstChild);
            if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "COMMA") == 0)
            {
                if (curNode->firstChild->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nodeName, "Args") == 0)
                {
                    translate_Args(curNode->firstChild->nextSibling->nextSibling);
                }
            }

            struct InterCodes *newInterCode = CreateInterCode(IC_ARG, newOp, NULL, NULL, NULL);
            InsertInterCode(newInterCode);
        }
    }
}

// Exp → Exp ASSIGNOP Exp
// | Exp AND Exp
// | Exp OR Exp
// | Exp RELOP Exp
// | Exp PLUS Exp
// | Exp MINUS Exp
// | Exp STAR Exp
// | Exp DIV Exp
// | LP Exp RP
// | MINUS Exp
// | NOT Exp
// | ID LP Args RP
// | ID LP RP
// | Exp LB Exp RB
// | Exp DOT ID
// | ID
// | INT
// | FLOAT
void translate_Cond(struct TreeNode *curNode, Operand label_true, Operand label_false)
{
    if (curNode != NULL && strcmp(curNode->nodeName, "Exp") == 0)
    {
        if (curNode->firstChild != NULL)
        {
            if (strcmp(curNode->firstChild->nodeName, "Exp") == 0)
            {
                if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "AND") == 0)
                {
                    if (curNode->firstChild->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nodeName, "Exp") == 0)
                    {
                        Operand label = CreateOperand(OP_LABEL, -1, NULL);
                        translate_Cond(curNode->firstChild, label, label_false);
                        struct InterCodes *newInterCode = CreateInterCode(IC_LABEL, label, NULL, NULL, NULL);
                        InsertInterCode(newInterCode);
                        translate_Cond(curNode->firstChild->nextSibling->nextSibling, label_true, label_false);
                    }
                }
                else if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "OR") == 0)
                {
                    if (curNode->firstChild->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nodeName, "Exp") == 0)
                    {
                        Operand label = CreateOperand(OP_LABEL, -1, NULL);
                        translate_Cond(curNode->firstChild, label_true, label);
                        struct InterCodes *newInterCode = CreateInterCode(IC_LABEL, label, NULL, NULL, NULL);
                        InsertInterCode(newInterCode);
                        translate_Cond(curNode->firstChild->nextSibling->nextSibling, label_true, label_false);
                    }
                }
                else if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "RELOP") == 0)
                {
                    if (curNode->firstChild->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nodeName, "Exp") == 0)
                    {
                        Operand newOp1 = translate_Exp(curNode->firstChild);
                        Operand newOp2 = translate_Exp(curNode->firstChild->nextSibling->nextSibling);
                        struct InterCodes *newInterCode = CreateInterCode(IC_IF, newOp1, newOp2, label_true, curNode->firstChild->nextSibling->nodeValue.stringValue);
                        InsertInterCode(newInterCode);
                        newInterCode = CreateInterCode(IC_GOTO, label_false, NULL, NULL, NULL);
                        InsertInterCode(newInterCode);
                    }
                }
                else
                {
                    Operand newOp1 = translate_Exp(curNode);
                    Operand newOp2 = CreateOperand(OP_CONSTANT, 0, NULL);
                    struct InterCodes *newInterCode = CreateInterCode(IC_IF, newOp1, newOp2, label_true, "!=");
                    InsertInterCode(newInterCode);
                    newInterCode = CreateInterCode(IC_GOTO, label_false, NULL, NULL, NULL);
                    InsertInterCode(newInterCode);
                }
            }
            else if (strcmp(curNode->firstChild->nodeName, "NOT") == 0)
            {
                if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "Exp") == 0)
                {
                    translate_Cond(curNode->firstChild->nextSibling, label_false, label_true);
                }
            }
            else
            {
                Operand newOp1 = translate_Exp(curNode);
                Operand newOp2 = CreateOperand(OP_CONSTANT, 0, NULL);
                struct InterCodes *newInterCode = CreateInterCode(IC_IF, newOp1, newOp2, label_true, "!=");
                InsertInterCode(newInterCode);
                newInterCode = CreateInterCode(IC_GOTO, label_false, NULL, NULL, NULL);
                InsertInterCode(newInterCode);
            }
        }
    }
}