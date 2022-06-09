#include <stdlib.h>
#include "objectCode.h"

struct Register myRegister[32];

struct StackNode *stackButtom;
struct StackNode *stackFP;
struct StackNode *stackSP;

void GenerateObjectCode(FILE *fout)
{
    InitRegister();
    InitCode(fout);
    InitStack();

    GenerateCodes(fout);
}

void InitCode(FILE *fout)
{
    fprintf(fout, ".data\n");
    fprintf(fout, "_prompt: .asciiz \"Enter an integer:\"\n");
    fprintf(fout, "_ret: .asciiz \"\\n\"\n");
    fprintf(fout, ".globl main\n\n");
    fprintf(fout, ".text\n");
    fprintf(fout, "read:\n");
    fprintf(fout, "li $v0, 4\n");
    fprintf(fout, "la $a0, _prompt\n");
    fprintf(fout, "syscall\n");
    fprintf(fout, "li $v0, 5\n");
    fprintf(fout, "syscall\n");
    fprintf(fout, "jr $ra\n\n");
    fprintf(fout, "write:\n");
    fprintf(fout, "li $v0, 1\n");
    fprintf(fout, "syscall\n");
    fprintf(fout, "li $v0, 4\n");
    fprintf(fout, "la $a0, _ret\n");
    fprintf(fout, "syscall\n");
    fprintf(fout, "move $v0, $0\n");
    fprintf(fout, "jr $ra\n\n");
}

void InitRegister()
{
    for (int i = 0; i < 32; i++)
    {
        myRegister[i].used = 0;
        myRegister[i].value = 0;
    }
    myRegister[0].regName = "$zero";
    myRegister[1].regName = "$at";
    myRegister[2].regName = "$v0";
    myRegister[3].regName = "$v1";

    myRegister[4].regName = "$a0";
    myRegister[5].regName = "$a1";
    myRegister[6].regName = "$a2";
    myRegister[7].regName = "$a3";

    myRegister[8].regName = "$t0";
    myRegister[9].regName = "$t1";
    myRegister[10].regName = "$t2";
    myRegister[11].regName = "$t3";

    myRegister[12].regName = "$t4";
    myRegister[13].regName = "$t5";
    myRegister[14].regName = "$t6";
    myRegister[15].regName = "$t7";

    myRegister[16].regName = "$s0";
    myRegister[17].regName = "$s1";
    myRegister[18].regName = "$s2";
    myRegister[19].regName = "$s3";

    myRegister[20].regName = "$s4";
    myRegister[21].regName = "$s5";
    myRegister[22].regName = "$s6";
    myRegister[23].regName = "$s7";

    myRegister[24].regName = "$t8";
    myRegister[25].regName = "$t9";

    myRegister[26].regName = "$k0";
    myRegister[27].regName = "$k1";

    myRegister[28].regName = "$gp";
    myRegister[29].regName = "$sp";

    myRegister[30].regName = "$fp";
    myRegister[31].regName = "$ra";
}

void InitStack()
{
    stackButtom = malloc(sizeof(struct StackNode));
    stackButtom->nextStackNode = NULL;
    stackButtom->isFP = 0;
    stackButtom->isRA = 0;
    stackButtom->kind = -1;
    stackButtom->offset = 0;
    stackButtom->u.no = -1;
    stackFP = stackButtom;
    stackSP = stackButtom;
    myRegister[30].value = 4;
    myRegister[29].value = 0;
}

int GetOpOffset(Operand op)
{
    struct StackNode *tmpStackNode = stackFP;
    while (tmpStackNode != NULL)
    {
        if (tmpStackNode->kind == op->kind)
        {
            if ((op->kind == OP_ADDRESS || op->kind == OP_VARIABLE) && tmpStackNode->u.no == op->u.var_no)
            {
                return tmpStackNode->offset;
            }
            if (op->kind == OP_TMPVAR && tmpStackNode->u.no == op->u.tmp_no)
            {
                return tmpStackNode->offset;
            }
        }
        tmpStackNode = tmpStackNode->nextStackNode;
    }
    return 1;
}

void PushOp(FILE *fout, Operand op, int size)
{
    //printf("%d, %d\n", op->kind, op->u.var_no);
    if (GetOpOffset(op) != 1)
    {
        return;
    }
    struct StackNode *newStackNode = malloc(sizeof(struct StackNode));
    newStackNode->kind = op->kind;
    if (op->kind == OP_ADDRESS || op->kind == OP_VARIABLE)
    {
        newStackNode->u.no = op->u.var_no;
    }
    if (op->kind == OP_TMPVAR)
    {
        newStackNode->u.no = op->u.tmp_no;
    }
    newStackNode->isFP = 0;
    newStackNode->isRA = 0;
    newStackNode->nextStackNode = NULL;
    myRegister[29].value -= size;
    newStackNode->offset = myRegister[29].value;
    fprintf(fout, "  addi $sp, $sp, -%d\n", size);
    stackSP->nextStackNode = newStackNode;
    stackSP = newStackNode;
}

void PopOps(FILE *fout)
{
    struct StackNode *tmpStackNode = stackFP->nextStackNode;
    stackFP->nextStackNode = NULL;
    stackSP = stackFP;
    while (tmpStackNode != NULL)
    {
        struct StackNode *tmp = tmpStackNode;
        tmpStackNode = tmpStackNode->nextStackNode;
        free(tmp);
        tmp = NULL;
    }
    myRegister[29].value = 0;
    myRegister[30].value = 4;
}

void GenerateCodes(FILE *fout)
{
    struct InterCodes *curIC = codeHead->next;
    int i = 0;
    while (curIC != codeHead)
    {
        TrasnlateSingleObjectCode(fout, curIC);
        curIC = curIC->next;
        //printf("%d\n", i++);
    }
}

void TrasnlateSingleObjectCode(FILE *fout, struct InterCodes *curIC)
{
    switch (curIC->code.kind)
    {
    case IC_LABEL:
    {
        fprintf(fout, "label%d", curIC->code.u.single.op->u.label_no);
        fprintf(fout, " :\n");
        break;
    }
    case IC_FUNCTION:
    {
        PopOps(fout);
        fprintf(fout, "%s", curIC->code.u.single.op->u.funName);
        fprintf(fout, " :\n");
        if (strcmp(curIC->code.u.single.op->u.funName, "main") == 0)
        {
            fprintf(fout, "  addi $fp, $sp, 4\n");
        }
        else
        {
            struct InterCodes *tmpIC = curIC->next;
            int paramOffset = 0;
            while (tmpIC != NULL)
            {
                if (tmpIC->code.kind != IC_PARAM)
                {
                    break;
                }
                tmpIC = tmpIC->next;
                paramOffset += 4;
            }

            tmpIC = tmpIC->prev;
            while (tmpIC != NULL)
            {
                if (tmpIC->code.kind != IC_PARAM)
                {
                    break;
                }
                struct StackNode *newStackNode = malloc(sizeof(struct StackNode));
                newStackNode->kind = tmpIC->code.u.single.op->kind;
                if (newStackNode->kind == OP_ADDRESS || newStackNode->kind == OP_VARIABLE)
                {
                    newStackNode->u.no = tmpIC->code.u.single.op->u.var_no;
                }
                if (newStackNode->kind == OP_TMPVAR)
                {
                    newStackNode->u.no = tmpIC->code.u.single.op->u.tmp_no;
                }
                newStackNode->isFP = 0;
                newStackNode->isRA = 0;
                newStackNode->nextStackNode = NULL;
                myRegister[29].value -= 4;
                newStackNode->offset = myRegister[29].value;
                stackSP->nextStackNode = newStackNode;
                stackSP = newStackNode;
                tmpIC = tmpIC->prev;
            }
            fprintf(fout, "  addi $fp, $sp, %d\n", paramOffset + 8 + 4);

            myRegister[29].value -= 8;
            struct StackNode *newStackNode = malloc(sizeof(struct StackNode));
            newStackNode->isFP = 1;
            newStackNode->isRA = 0;
            newStackNode->nextStackNode = NULL;
            newStackNode->offset = myRegister[29].value + 4;
            stackSP->nextStackNode = newStackNode;
            stackSP = newStackNode;

            newStackNode = malloc(sizeof(struct StackNode));
            newStackNode->isRA = 1;
            newStackNode->isFP = 0;
            newStackNode->nextStackNode = NULL;
            newStackNode->offset = myRegister[29].value + 0;
            stackSP->nextStackNode = newStackNode;
            stackSP = newStackNode;
        }
        break;
    }
    case IC_ASSIGN:
    {
        if (curIC->code.u.assign.right->kind == OP_CONSTANT)
        {
            Load(fout, curIC->code.u.assign.right, 8);
            Store(fout, curIC->code.u.assign.left, 8);
        }
        else
        {
            Load(fout, curIC->code.u.assign.right, 8);
            fprintf(fout, "  move %s, %s\n", myRegister[9].regName, myRegister[8].regName);
            Store(fout, curIC->code.u.assign.left, 9);
        }

        break;
    }
    case IC_ADD:
    {

        if (curIC->code.u.binop.op2->kind == OP_CONSTANT)
        {
            Load(fout, curIC->code.u.binop.op1, 8);
            fprintf(fout, "  addi %s, %s, %d\n", myRegister[9].regName, myRegister[8].regName, curIC->code.u.binop.op2->u.value);
            Store(fout, curIC->code.u.binop.result, 9);
        }
        else
        {
            Load(fout, curIC->code.u.binop.op1, 8);
            Load(fout, curIC->code.u.binop.op2, 9);
            fprintf(fout, "  add %s, %s, %s\n", myRegister[10].regName, myRegister[8].regName, myRegister[9].regName);
            Store(fout, curIC->code.u.binop.result, 10);
        }
        break;
    }
    case IC_SUB:
    {
        if (curIC->code.u.binop.op2->kind == OP_CONSTANT)
        {
            Load(fout, curIC->code.u.binop.op1, 8);
            fprintf(fout, "  addi %s, %s, %d\n", myRegister[9].regName, myRegister[8].regName, -curIC->code.u.binop.op2->u.value);
            Store(fout, curIC->code.u.binop.result, 9);
        }
        else
        {
            Load(fout, curIC->code.u.binop.op1, 8);
            Load(fout, curIC->code.u.binop.op2, 9);
            fprintf(fout, "  sub %s, %s, %s\n", myRegister[10].regName, myRegister[8].regName, myRegister[9].regName);
            Store(fout, curIC->code.u.binop.result, 10);
        }
        break;
    }
    case IC_MUL:
    {
        Load(fout, curIC->code.u.binop.op1, 8);
        Load(fout, curIC->code.u.binop.op2, 9);
        fprintf(fout, "  mul %s, %s, %s\n", myRegister[10].regName, myRegister[8].regName, myRegister[9].regName);
        Store(fout, curIC->code.u.binop.result, 10);
        break;
    }
    case IC_DIV:
    {
        Load(fout, curIC->code.u.binop.op1, 8);
        Load(fout, curIC->code.u.binop.op2, 9);
        fprintf(fout, "  div %s, %s\n", myRegister[8].regName, myRegister[9].regName);
        fprintf(fout, "  mflo %s", myRegister[10].regName);
        Store(fout, curIC->code.u.binop.result, 10);
        break;
    }
    case IC_GET_CONTENT:
    {
        Load(fout, curIC->code.u.assign.right, 8);
        fprintf(fout, "  lw %s, 0(%s)\n", myRegister[9].regName, myRegister[8].regName);
        Store(fout, curIC->code.u.assign.left, 9);
        break;
    }
    case IC_ASG_CONTENT:
    {
        Load(fout, curIC->code.u.assign.right, 8);
        Load(fout, curIC->code.u.assign.left, 9);
        fprintf(fout, "  sw %s, 0(%s)\n", myRegister[8].regName, myRegister[9].regName);
        Store(fout, curIC->code.u.assign.left, 9);
        break;
    }
    case IC_GOTO:
    {
        fprintf(fout, "  j ");
        fprintf(fout, "label%d", curIC->code.u.single.op->u.label_no);
        fprintf(fout, "\n");
        break;
    }
    case IC_IF:
    {
        Load(fout, curIC->code.u.ifStmt.op1, 8);
        Load(fout, curIC->code.u.ifStmt.op2, 9);
        fprintf(fout, "  ");
        if (strcmp(curIC->code.u.ifStmt.relop, "==") == 0)
        {
            fprintf(fout, "beq");
        }
        if (strcmp(curIC->code.u.ifStmt.relop, "!=") == 0)
        {
            fprintf(fout, "bne");
        }
        if (strcmp(curIC->code.u.ifStmt.relop, ">") == 0)
        {
            fprintf(fout, "bgt");
        }
        if (strcmp(curIC->code.u.ifStmt.relop, "<") == 0)
        {
            fprintf(fout, "blt");
        }
        if (strcmp(curIC->code.u.ifStmt.relop, ">=") == 0)
        {
            fprintf(fout, "bge");
        }
        if (strcmp(curIC->code.u.ifStmt.relop, "<=") == 0)
        {
            fprintf(fout, "ble");
        }
        fprintf(fout, " %s, %s, label%d\n", myRegister[8].regName, myRegister[9].regName, curIC->code.u.ifStmt.op3->u.label_no);
        break;
    }
    case IC_RETURN:
    {
        Load(fout, curIC->code.u.single.op, 8);
        fprintf(fout, "  move $v0, %s\n", myRegister[8].regName);

        fprintf(fout, "  jr $ra\n");
        break;
    }
    case IC_DEC:
    {
        PushOp(fout, curIC->code.u.assign.left, curIC->code.u.assign.right->u.value);
        break;
    }
    case IC_ARG:
    {
        if (curIC->prev->code.kind == IC_ARG)
        {
            break;
        }
        struct InterCodes *tmpIC = curIC;
        int argOffset = myRegister[29].value;
        while (tmpIC != NULL)
        {
            if (tmpIC->code.kind != IC_ARG)
            {
                break;
            }

            fprintf(fout, "  addi $sp, $sp, %d\n", -4);
            argOffset -= 4;

            switch (tmpIC->code.u.single.op->kind)
            {
            case OP_VARIABLE:
            {
                int opOffset = GetOpOffset(tmpIC->code.u.single.op) - myRegister[30].value;
                fprintf(fout, "  lw %s, %d(%s)\n", myRegister[8].regName, opOffset, myRegister[30].regName);
                break;
            }
            case OP_CONSTANT:
            {
                fprintf(fout, "  li %s, %d\n", myRegister[8].regName, tmpIC->code.u.single.op->u.value);
                break;
            }
            case OP_ADDRESS:
            {
                int opOffset = GetOpOffset(tmpIC->code.u.single.op) - myRegister[30].value;
                fprintf(fout, "  la %s, %d(%s)\n", myRegister[8].regName, opOffset, myRegister[30].regName);
                break;
            }
            case OP_TMPVAR:
            {
                int opOffset = GetOpOffset(tmpIC->code.u.single.op) - myRegister[30].value;
                fprintf(fout, "  lw %s, %d(%s)\n", myRegister[8].regName, opOffset, myRegister[30].regName);
                break;
            }
            default:
            {
                break;
            }
            }
            fprintf(fout, "  sw %s, 0($sp)\n", myRegister[8].regName);

            tmpIC = tmpIC->next;
        }

        argOffset = argOffset - myRegister[29].value;

        fprintf(fout, "  addi $sp, $sp, %d\n", -8);
        fprintf(fout, "  sw $fp, 4($sp)\n");
        fprintf(fout, "  sw $ra, 0($sp)\n");
        fprintf(fout, "  jal %s\n", tmpIC->code.u.assign.right->u.funName);
        fprintf(fout, "  move %s, $v0\n", myRegister[8].regName);

        fprintf(fout, "  addi $sp, $fp, %d\n", argOffset - 8 - 4);
        fprintf(fout, "  lw $fp, 4($sp)\n");
        fprintf(fout, "  lw $ra, 0($sp)\n");

        fprintf(fout, "  addi $sp, $sp, %d\n", 8);

        fprintf(fout, "  addi $sp, $sp, %d\n", -argOffset);

        Store(fout, tmpIC->code.u.assign.left, 8);

        break;
    }
    case IC_CALL:
    {
        break;
    }
    case IC_PARAM:
    {
        break;
    }
    case IC_READ:
    {
        fprintf(fout, "  addi $sp, $sp, %d\n", -4);
        fprintf(fout, "  sw $ra, 0($sp)\n");
        fprintf(fout, "  jal read\n");
        fprintf(fout, "  lw $ra, 0($sp)\n");
        fprintf(fout, "  addi $sp, $sp, %d\n", 4);
        fprintf(fout, "  move %s, $v0\n", myRegister[8].regName);
        Store(fout, curIC->code.u.single.op, 8);
        break;
    }
    case IC_WRITE:
    {

        Load(fout, curIC->code.u.single.op, 4);
        fprintf(fout, "  addi $sp, $sp, %d\n", -4);
        fprintf(fout, "  sw $ra, 0($sp)\n");
        fprintf(fout, "  jal write\n");
        fprintf(fout, "  lw $ra, 0($sp)\n");
        fprintf(fout, "  addi $sp, $sp, %d\n", 4);
        break;
    }
    }
}

void Load(FILE *fout, Operand op, int regNo)
{
    if (GetOpOffset(op) == 1 && op->kind != OP_CONSTANT)
    {
        PushOp(fout, op, 4);
    }
    switch (op->kind)
    {
    case OP_VARIABLE:
    {
        int opOffset = GetOpOffset(op) - myRegister[30].value;
        fprintf(fout, "  lw %s, %d(%s)\n", myRegister[regNo].regName, opOffset, myRegister[30].regName);
        break;
    }
    case OP_CONSTANT:
    {
        fprintf(fout, "  li %s, %d\n", myRegister[regNo].regName, op->u.value);
        break;
    }
    case OP_ADDRESS:
    {
        int opOffset = GetOpOffset(op) - myRegister[30].value;
        fprintf(fout, "  la %s, %d(%s)\n", myRegister[regNo].regName, opOffset, myRegister[30].regName);
        break;
    }
    case OP_TMPVAR:
    {
        int opOffset = GetOpOffset(op) - myRegister[30].value;
        fprintf(fout, "  lw %s, %d(%s)\n", myRegister[regNo].regName, opOffset, myRegister[30].regName);
        break;
    }
    default:
    {
        break;
    }
    }
}

void Store(FILE *fout, Operand op, int regNo)
{
    if (GetOpOffset(op) == 1)
    {
        PushOp(fout, op, 4);
    }
    switch (op->kind)
    {
    case OP_VARIABLE:
    {
        int opOffset = GetOpOffset(op) - myRegister[30].value;
        fprintf(fout, "  sw %s, %d(%s)\n", myRegister[regNo].regName, opOffset, myRegister[30].regName);
        break;
    }
    case OP_TMPVAR:
    {
        int opOffset = GetOpOffset(op) - myRegister[30].value;
        fprintf(fout, "  sw %s, %d(%s)\n", myRegister[regNo].regName, opOffset, myRegister[30].regName);
        break;
    }
    default:
    {
        break;
    }
    }
}