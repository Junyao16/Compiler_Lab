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
            fprintf(fout, " ");
            fprintf(fout, "%d", tmp->code.u.assign.right->u.value);
            fprintf(fout, "\n");
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

Operand CreateOperand(int kind, int val, char *funName, char *varName)
{
    Operand newOperand = (Operand)malloc(sizeof(struct Operand_));
    newOperand->kind = kind;
    switch (kind)
    {
    case OP_VARIABLE:
    {
        newOperand->u.var_no = var_count;
        var_count++;
        strcpy(newOperand->varName, varName);
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
        strcpy(newOperand->varName, varName);
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
                Operand newOp;
                SymbolNode symbol = SearchSymbol(curNode->firstChild->nodeValue.stringValue);
                if (symbol->type->kind != BASIC)
                {
                    newOp = CreateOperand(OP_VARIABLE, -1, NULL, symbol->sName);
                    Operand newConstant = CreateOperand(OP_CONSTANT, GetSize(symbol->type), NULL, NULL);
                    struct InterCodes *newInterCode = CreateInterCode(IC_DEC, newOp, newConstant, NULL, NULL);
                    InsertInterCode(newInterCode);
                }
                else
                {
                    newOp = CreateOperand(OP_VARIABLE, -1, NULL, symbol->sName);
                }
                symbol->var_no = newOp->u.var_no;
                return newOp;
            }
            else if (strcmp(curNode->firstChild->nodeName, "VarDec") == 0)
            {
                if (strcmp(curNode->firstChild->firstChild->nodeName, "ID") == 0)
                {
                    SymbolNode symbol = SearchSymbol(curNode->firstChild->firstChild->nodeValue.stringValue);
                    Operand newOp = CreateOperand(OP_VARIABLE, -1, NULL, symbol->sName);
                    symbol->var_no = newOp->u.var_no;
                    Operand newConstant = CreateOperand(OP_CONSTANT, GetSize(symbol->type), NULL, NULL);
                    struct InterCodes *newInterCode = CreateInterCode(IC_DEC, newOp, newConstant, NULL, NULL);
                    InsertInterCode(newInterCode);
                    return newOp;
                }
                else
                {
                    ErrorOutIC();
                    return NULL;
                }
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
            Operand newOp = CreateOperand(OP_FUNCTION, -1, curNode->firstChild->nodeValue.stringValue, NULL);
            struct InterCodes *newInterCode = CreateInterCode(IC_FUNCTION, newOp, NULL, NULL, NULL);
            InsertInterCode(newInterCode);
            SymbolNode tmpSymbol = SearchSymbol(newOp->u.funName);
            FieldList paralist = tmpSymbol->type->u.function.paraList;
            while (paralist != NULL)
            {
                SymbolNode param = SearchSymbol(paralist->name);
                Operand tmpOp = CreateOperand(OP_VARIABLE, -1, NULL, param->sName);
                struct InterCodes *tmpInterCode = CreateInterCode(IC_PARAM, tmpOp, NULL, NULL, NULL);
                InsertInterCode(tmpInterCode);
                if (param->type->kind == ARRAY)
                {
                    ErrorOutIC();
                    return;
                }
                param->var_no = tmpOp->u.var_no;
                param->isAddrParam = 1;
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
void translate_Stmt(struct TreeNode *curNode)
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
                    Operand label1 = CreateOperand(OP_LABEL, -1, NULL, NULL);
                    Operand label2 = CreateOperand(OP_LABEL, -1, NULL, NULL);
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
                                    Operand label3 = CreateOperand(OP_LABEL, -1, NULL, NULL);
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
                    Operand label1 = CreateOperand(OP_LABEL, -1, NULL, NULL);
                    Operand label2 = CreateOperand(OP_LABEL, -1, NULL, NULL);
                    Operand label3 = CreateOperand(OP_LABEL, -1, NULL, NULL);
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
void translate_Def(struct TreeNode *curNode)
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
void translate_DecList(struct TreeNode *curNode)
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
void translate_Dec(struct TreeNode *curNode)
{
    if (curNode != NULL && strcmp(curNode->nodeName, "Dec") == 0)
    {
        if (curNode->firstChild != NULL && strcmp(curNode->firstChild->nodeName, "VarDec") == 0)
        {
            Operand newOp1 = translate_VarDec(curNode->firstChild);
            if (newOp1 == NULL)
            {
                return;
            }
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
                if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "ASSIGNOP") == 0)
                {
                    if (curNode->firstChild->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nodeName, "Exp") == 0)
                    {
                        Operand newOp2 = translate_Exp(curNode->firstChild->nextSibling->nextSibling);
                        if (strcmp(curNode->firstChild->firstChild->nodeName, "ID") == 0 && curNode->firstChild->firstChild->nextSibling == NULL)
                        {
                            Operand newOp1 = translate_Exp(curNode->firstChild);
                            SymbolNode symbolnode1 = SearchSymbol(newOp1->varName);
                            if (symbolnode1->type->kind == BASIC)
                            {
                                struct InterCodes *newInterCode = CreateInterCode(IC_ASSIGN, newOp1, newOp2, NULL, NULL);
                                InsertInterCode(newInterCode);
                                Operand newOp = newOp1;
                                return newOp;
                            }
                            else
                            {
                                SymbolNode symbolnode2 = SearchSymbol(newOp2->varName);
                                int size1 = GetSize(symbolnode1->type);
                                int size2 = GetSize(symbolnode2->type);
                                Operand newAddr1 = CreateOperand(OP_TMPVAR, -1, NULL, NULL);
                                Operand newAddr2 = CreateOperand(OP_TMPVAR, -1, NULL, NULL);
                                struct InterCodes *newInterCode;
                                newInterCode = CreateInterCode(IC_ASSIGN, newAddr1, newOp1, NULL, NULL);
                                InsertInterCode(newInterCode);
                                newInterCode = CreateInterCode(IC_ASSIGN, newAddr2, newOp2, NULL, NULL);
                                InsertInterCode(newInterCode);
                                Operand newTmp = CreateOperand(OP_TMPVAR, -1, NULL, NULL);
                                newInterCode = CreateInterCode(IC_GET_CONTENT, newTmp, newAddr2, NULL, NULL);
                                InsertInterCode(newInterCode);
                                newInterCode = CreateInterCode(IC_ASG_CONTENT, newAddr1, newTmp, NULL, NULL);
                                InsertInterCode(newInterCode);
                                int offset = 4;
                                while (offset < size1 && offset < size2)
                                {
                                    Operand newOff = CreateOperand(OP_CONSTANT, 4, NULL, NULL);
                                    newInterCode = CreateInterCode(IC_ADD, newAddr1, newOff, newAddr1, NULL);
                                    InsertInterCode(newInterCode);
                                    newInterCode = CreateInterCode(IC_ADD, newAddr2, newOff, newAddr2, NULL);
                                    InsertInterCode(newInterCode);
                                    newTmp = CreateOperand(OP_TMPVAR, -1, NULL, NULL);
                                    newInterCode = CreateInterCode(IC_GET_CONTENT, newTmp, newAddr2, NULL, NULL);
                                    InsertInterCode(newInterCode);
                                    newInterCode = CreateInterCode(IC_ASG_CONTENT, newAddr1, newTmp, NULL, NULL);
                                    InsertInterCode(newInterCode);
                                    offset += 4;
                                }
                                return NULL;
                            }
                        }
                        else if (strcmp(curNode->firstChild->firstChild->nodeName, "Exp") == 0 && curNode->firstChild->firstChild->nextSibling != NULL)
                        {
                            Operand newOp1 = translate_Exp(curNode->firstChild->firstChild);
                            if (strcmp(curNode->firstChild->firstChild->nextSibling->nodeName, "LB") == 0)
                            {
                                //TODO:
                                Operand newOp3 = translate_Exp(curNode->firstChild->firstChild->nextSibling->nextSibling);
                                Operand newConstant = CreateOperand(OP_CONSTANT, GetSize(SearchSymbol(newOp1->varName)->type->u.array.elem), NULL, NULL);
                                Operand newAddr = CreateOperand(OP_TMPVAR, -1, NULL, NULL);
                                Operand newOff = CreateOperand(OP_TMPVAR, -1, NULL, NULL);
                                struct InterCodes *newInterCode = CreateInterCode(IC_MUL, newConstant, newOp3, newOff, NULL);
                                InsertInterCode(newInterCode);
                                newInterCode = CreateInterCode(IC_ADD, newOp1, newOff, newAddr, NULL);
                                InsertInterCode(newInterCode);
                                newInterCode = CreateInterCode(IC_ASG_CONTENT, newAddr, newOp2, NULL, NULL);
                                InsertInterCode(newInterCode);

                                SymbolNode symbolnode = SearchSymbol(newOp1->varName);
                                if (symbolnode->type->u.array.elem->kind == BASIC)
                                {
                                    return newOp2;
                                    // Operand newOp = CreateOperand(OP_TMPVAR, -1, NULL, NULL);
                                    // newInterCode = CreateInterCode(IC_GET_CONTENT, newOp, newAddr, NULL, NULL);
                                    // InsertInterCode(newInterCode);
                                    // strcpy(newOp->varName, symbolnode->sName);
                                    // return newOp;
                                }
                                else
                                {
                                    strcpy(newAddr->varName, symbolnode->type->u.array.elem->u.structure.structName);
                                    return newAddr;
                                }
                            }
                            if (strcmp(curNode->firstChild->firstChild->nextSibling->nodeName, "DOT") == 0)
                            {
                                SymbolNode symbolnode = SearchSymbol(curNode->firstChild->firstChild->nextSibling->nextSibling->nodeValue.stringValue);

                                Operand newOp3;
                                if (symbolnode->type->kind != BASIC)
                                {
                                    newOp3 = CreateOperand(OP_ADDRESS, -1, NULL, symbolnode->sName);
                                }
                                else
                                {
                                    newOp3 = CreateOperand(OP_VARIABLE, -1, NULL, symbolnode->sName);
                                }
                                var_count--;
                                newOp3->u.var_no = symbolnode->var_no;

                                int offset = 0;
                                FieldList fieldlist = SearchSymbol(newOp1->varName)->type->u.structure.fieldlist;
                                while (fieldlist != NULL && strcmp(fieldlist->name, newOp3->varName) != 0)
                                {
                                    offset += GetSize(fieldlist->type);
                                    fieldlist = fieldlist->tail;
                                }
                                Operand newOff = CreateOperand(OP_CONSTANT, offset, NULL, NULL);
                                Operand newAddr = CreateOperand(OP_TMPVAR, -1, NULL, NULL);
                                struct InterCodes *newInterCode = CreateInterCode(IC_ADD, newOp1, newOff, newAddr, NULL);
                                InsertInterCode(newInterCode);
                                newInterCode = CreateInterCode(IC_ASG_CONTENT, newAddr, newOp2, NULL, NULL);
                                InsertInterCode(newInterCode);

                                if (symbolnode->type->kind == BASIC)
                                {
                                    return newOp2;
                                    // Operand newOp = CreateOperand(OP_TMPVAR, -1, NULL, NULL);
                                    // newInterCode = CreateInterCode(IC_GET_CONTENT, newOp, newAddr, NULL, NULL);
                                    // InsertInterCode(newInterCode);
                                    // strcpy(newOp->varName, symbolnode->sName);
                                    // return newOp;
                                }
                                else
                                {
                                    strcpy(newAddr->varName, symbolnode->sName);
                                    return newAddr;
                                }
                            }
                        }
                    }
                }
                else if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "AND") == 0)
                {
                    if (curNode->firstChild->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nodeName, "Exp") == 0)
                    {
                        Operand newLabel1 = CreateOperand(OP_LABEL, -1, NULL, NULL);
                        Operand newLabel2 = CreateOperand(OP_LABEL, -1, NULL, NULL);
                        Operand newOp = CreateOperand(OP_TMPVAR, -1, NULL, NULL);
                        Operand newConstant = CreateOperand(OP_CONSTANT, 0, NULL, NULL);
                        struct InterCodes *newInterCode = CreateInterCode(IC_ASSIGN, newOp, newConstant, NULL, NULL);
                        InsertInterCode(newInterCode);
                        translate_Cond(curNode, newLabel1, newLabel2);
                        newInterCode = CreateInterCode(IC_LABEL, newLabel1, NULL, NULL, NULL);
                        InsertInterCode(newInterCode);
                        newConstant = CreateOperand(OP_CONSTANT, 1, NULL, NULL);
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
                        Operand newLabel1 = CreateOperand(OP_LABEL, -1, NULL, NULL);
                        Operand newLabel2 = CreateOperand(OP_LABEL, -1, NULL, NULL);
                        Operand newOp = CreateOperand(OP_TMPVAR, -1, NULL, NULL);
                        Operand newConstant = CreateOperand(OP_CONSTANT, 0, NULL, NULL);
                        struct InterCodes *newInterCode = CreateInterCode(IC_ASSIGN, newOp, newConstant, NULL, NULL);
                        InsertInterCode(newInterCode);
                        translate_Cond(curNode, newLabel1, newLabel2);
                        newInterCode = CreateInterCode(IC_LABEL, newLabel1, NULL, NULL, NULL);
                        InsertInterCode(newInterCode);
                        newConstant = CreateOperand(OP_CONSTANT, 1, NULL, NULL);
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
                        Operand newLabel1 = CreateOperand(OP_LABEL, -1, NULL, NULL);
                        Operand newLabel2 = CreateOperand(OP_LABEL, -1, NULL, NULL);
                        Operand newOp = CreateOperand(OP_TMPVAR, -1, NULL, NULL);
                        Operand newConstant = CreateOperand(OP_CONSTANT, 0, NULL, NULL);
                        struct InterCodes *newInterCode = CreateInterCode(IC_ASSIGN, newOp, newConstant, NULL, NULL);
                        InsertInterCode(newInterCode);
                        translate_Cond(curNode, newLabel1, newLabel2);
                        newInterCode = CreateInterCode(IC_LABEL, newLabel1, NULL, NULL, NULL);
                        InsertInterCode(newInterCode);
                        newConstant = CreateOperand(OP_CONSTANT, 1, NULL, NULL);
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
                        Operand newOp1 = translate_Exp(curNode->firstChild);
                        Operand newOp2 = translate_Exp(curNode->firstChild->nextSibling->nextSibling);
                        Operand newOp = CreateOperand(OP_TMPVAR, -1, NULL, NULL);
                        struct InterCodes *newInterCode = CreateInterCode(IC_ADD, newOp1, newOp2, newOp, NULL);
                        InsertInterCode(newInterCode);
                        return newOp;
                    }
                }
                else if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "MINUS") == 0)
                {
                    if (curNode->firstChild->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nodeName, "Exp") == 0)
                    {
                        Operand newOp1 = translate_Exp(curNode->firstChild);
                        Operand newOp2 = translate_Exp(curNode->firstChild->nextSibling->nextSibling);
                        Operand newOp = CreateOperand(OP_TMPVAR, -1, NULL, NULL);
                        struct InterCodes *newInterCode = CreateInterCode(IC_SUB, newOp1, newOp2, newOp, NULL);
                        InsertInterCode(newInterCode);
                        return newOp;
                    }
                }
                else if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "STAR") == 0)
                {
                    if (curNode->firstChild->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nodeName, "Exp") == 0)
                    {
                        Operand newOp1 = translate_Exp(curNode->firstChild);
                        Operand newOp2 = translate_Exp(curNode->firstChild->nextSibling->nextSibling);
                        Operand newOp = CreateOperand(OP_TMPVAR, -1, NULL, NULL);
                        struct InterCodes *newInterCode = CreateInterCode(IC_MUL, newOp1, newOp2, newOp, NULL);
                        InsertInterCode(newInterCode);
                        return newOp;
                    }
                }
                else if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "DIV") == 0)
                {
                    if (curNode->firstChild->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nodeName, "Exp") == 0)
                    {
                        Operand newOp1 = translate_Exp(curNode->firstChild);
                        Operand newOp2 = translate_Exp(curNode->firstChild->nextSibling->nextSibling);
                        Operand newOp = CreateOperand(OP_TMPVAR, -1, NULL, NULL);
                        struct InterCodes *newInterCode = CreateInterCode(IC_DIV, newOp1, newOp2, newOp, NULL);
                        InsertInterCode(newInterCode);
                        return newOp;
                    }
                }
                else if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "LB") == 0)
                {
                    if (curNode->firstChild->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nodeName, "Exp") == 0)
                    {
                        Operand newOp1 = translate_Exp(curNode->firstChild);
                        Operand newOp2 = translate_Exp(curNode->firstChild->nextSibling->nextSibling);
                        Operand newConstant = CreateOperand(OP_CONSTANT, GetSize(SearchSymbol(newOp1->varName)->type->u.array.elem), NULL, NULL);
                        Operand newAddr = CreateOperand(OP_TMPVAR, -1, NULL, NULL);
                        Operand newOff = CreateOperand(OP_TMPVAR, -1, NULL, NULL);
                        struct InterCodes *newInterCode = CreateInterCode(IC_MUL, newConstant, newOp2, newOff, NULL);
                        InsertInterCode(newInterCode);
                        newInterCode = CreateInterCode(IC_ADD, newOp1, newOff, newAddr, NULL);
                        InsertInterCode(newInterCode);

                        SymbolNode symbolnode = SearchSymbol(newOp1->varName);
                        if (symbolnode->type->u.array.elem->kind == BASIC)
                        {
                            Operand newOp = CreateOperand(OP_TMPVAR, -1, NULL, NULL);
                            newInterCode = CreateInterCode(IC_GET_CONTENT, newOp, newAddr, NULL, NULL);
                            InsertInterCode(newInterCode);
                            strcpy(newOp->varName, symbolnode->sName);
                            return newOp;
                        }
                        else
                        {
                            strcpy(newAddr->varName, symbolnode->type->u.array.elem->u.structure.structName);
                            return newAddr;
                        }
                    }
                }
                else if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "DOT") == 0)
                {
                    if (curNode->firstChild->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nodeName, "ID") == 0)
                    {
                        Operand newOp1 = translate_Exp(curNode->firstChild);
                        SymbolNode symbolnode = SearchSymbol(curNode->firstChild->nextSibling->nextSibling->nodeValue.stringValue);

                        Operand newOp2;
                        if (symbolnode->type->kind != BASIC)
                        {
                            newOp2 = CreateOperand(OP_ADDRESS, -1, NULL, symbolnode->sName);
                        }
                        else
                        {
                            newOp2 = CreateOperand(OP_VARIABLE, -1, NULL, symbolnode->sName);
                        }
                        var_count--;
                        newOp2->u.var_no = symbolnode->var_no;

                        int offset = 0;
                        FieldList fieldlist = SearchSymbol(newOp1->varName)->type->u.structure.fieldlist;
                        while (fieldlist != NULL && strcmp(fieldlist->name, symbolnode->sName) != 0)
                        {
                            offset += GetSize(fieldlist->type);
                            fieldlist = fieldlist->tail;
                        }
                        Operand newOff = CreateOperand(OP_CONSTANT, offset, NULL, NULL);
                        Operand newAddr = CreateOperand(OP_TMPVAR, -1, NULL, NULL);
                        struct InterCodes *newInterCode = CreateInterCode(IC_ADD, newOp1, newOff, newAddr, NULL);
                        InsertInterCode(newInterCode);

                        if (symbolnode->type->kind == BASIC)
                        {
                            Operand newOp = CreateOperand(OP_TMPVAR, -1, NULL, NULL);
                            newInterCode = CreateInterCode(IC_GET_CONTENT, newOp, newAddr, NULL, NULL);
                            InsertInterCode(newInterCode);
                            strcpy(newOp->varName, symbolnode->sName);
                            return newOp;
                        }
                        else
                        {
                            strcpy(newAddr->varName, symbolnode->sName);
                            return newAddr;
                        }
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
                    Operand newOp = CreateOperand(OP_TMPVAR, -1, NULL, NULL);
                    Operand newConstant = CreateOperand(OP_CONSTANT, 0, NULL, NULL);
                    struct InterCodes *newInterCode = CreateInterCode(IC_SUB, newConstant, newTmp, newOp, NULL);
                    InsertInterCode(newInterCode);
                    return newOp;
                }
            }
            else if (strcmp(curNode->firstChild->nodeName, "NOT") == 0)
            {
                if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "Exp") == 0)
                {
                    Operand newLabel1 = CreateOperand(OP_LABEL, -1, NULL, NULL);
                    Operand newLabel2 = CreateOperand(OP_LABEL, -1, NULL, NULL);
                    Operand newOp = CreateOperand(OP_TMPVAR, -1, NULL, NULL);
                    Operand newConstant = CreateOperand(OP_CONSTANT, 0, NULL, NULL);
                    struct InterCodes *newInterCode = CreateInterCode(IC_ASSIGN, newOp, newConstant, NULL, NULL);
                    InsertInterCode(newInterCode);
                    translate_Cond(curNode, newLabel1, newLabel2);
                    newInterCode = CreateInterCode(IC_LABEL, newLabel1, NULL, NULL, NULL);
                    InsertInterCode(newInterCode);
                    newConstant = CreateOperand(OP_CONSTANT, 1, NULL, NULL);
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
                // if (symbolnode->type->kind != BASIC)
                // {
                //     printf("other:%s\n", symbolnode->sName);
                // }
                // else
                // {
                //     printf("basic:%s\n", symbolnode->sName);
                // }
                if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "LP") == 0)
                {

                    if (curNode->firstChild->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nodeName, "Args") == 0)
                    {
                        if (strcmp(curNode->firstChild->nodeValue.stringValue, "write") == 0)
                        {
                            Operand newOp = translate_Exp(curNode->firstChild->nextSibling->nextSibling->firstChild);
                            struct InterCodes *newInterCode = CreateInterCode(IC_WRITE, newOp, NULL, NULL, NULL);
                            InsertInterCode(newInterCode);
                            newOp = CreateOperand(OP_CONSTANT, 0, NULL, NULL);
                            return newOp;
                        }
                        else
                        {
                            translate_Args(curNode->firstChild->nextSibling->nextSibling);
                            Operand newOp = CreateOperand(OP_TMPVAR, -1, NULL, NULL);
                            Operand funOp = CreateOperand(OP_FUNCTION, -1, curNode->firstChild->nodeValue.stringValue, NULL);
                            struct InterCodes *newInterCode = CreateInterCode(IC_CALL, newOp, funOp, NULL, NULL);
                            InsertInterCode(newInterCode);
                            return newOp;
                        }
                    }
                    else if (curNode->firstChild->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nodeName, "RP") == 0)
                    {
                        if (strcmp(curNode->firstChild->nodeValue.stringValue, "read") == 0)
                        {
                            Operand newOp = CreateOperand(OP_TMPVAR, -1, NULL, NULL);
                            struct InterCodes *newInterCode = CreateInterCode(IC_READ, newOp, NULL, NULL, NULL);
                            InsertInterCode(newInterCode);
                            return newOp;
                        }
                        else
                        {
                            Operand newOp = CreateOperand(OP_TMPVAR, -1, NULL, NULL);
                            Operand funOp = CreateOperand(OP_FUNCTION, -1, curNode->firstChild->nodeValue.stringValue, NULL);
                            struct InterCodes *newInterCode = CreateInterCode(IC_CALL, newOp, funOp, NULL, NULL);
                            InsertInterCode(newInterCode);
                            return newOp;
                        }
                    }
                }
                else
                {
                    Operand newOp;
                    if (symbolnode->type->kind != BASIC)
                    {
                        if (symbolnode->isAddrParam == 1)
                        {
                            newOp = CreateOperand(OP_VARIABLE, -1, NULL, symbolnode->sName);
                        }
                        else
                        {
                            newOp = CreateOperand(OP_ADDRESS, -1, NULL, symbolnode->sName);
                            //printf("other:%s\n", symbolnode->sName);
                        }
                    }
                    else
                    {
                        newOp = CreateOperand(OP_VARIABLE, -1, NULL, symbolnode->sName);
                        //printf("basic:%s\n", symbolnode->sName);
                    }
                    var_count--;
                    newOp->u.var_no = symbolnode->var_no;
                    return newOp;
                }
            }
            else if (strcmp(curNode->firstChild->nodeName, "INT") == 0)
            {
                Operand newOp = CreateOperand(OP_CONSTANT, curNode->firstChild->nodeValue.intValue, NULL, NULL);
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
                        Operand label = CreateOperand(OP_LABEL, -1, NULL, NULL);
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
                        Operand label = CreateOperand(OP_LABEL, -1, NULL, NULL);
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
                    Operand newOp2 = CreateOperand(OP_CONSTANT, 0, NULL, NULL);
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
                Operand newOp2 = CreateOperand(OP_CONSTANT, 0, NULL, NULL);
                struct InterCodes *newInterCode = CreateInterCode(IC_IF, newOp1, newOp2, label_true, "!=");
                InsertInterCode(newInterCode);
                newInterCode = CreateInterCode(IC_GOTO, label_false, NULL, NULL, NULL);
                InsertInterCode(newInterCode);
            }
        }
    }
}

void ErrorOutIC()
{
    if (!haveError)
    {
        printf("Cannot translate: Code contains variables of multi-dimensional array type or parameters of array type.\n");
        haveError = 1;
    }
}