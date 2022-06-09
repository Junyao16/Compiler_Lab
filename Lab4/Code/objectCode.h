#include "interCode.h"

extern struct InterCodes *codeHead;
extern struct InterCodes *codeTail;

struct Register
{
    char *regName;
    int used;
    int value;
};

struct StackNode
{
    int offset;
    int kind;
    int isFP;
    int isRA;
    union {
        int no;
        int value;
    } u;
    struct StackNode *nextStackNode;
};

void GenerateObjectCode(FILE *fout);

void InitCode(FILE *fout);

void InitRegister();

void InitStack();

int GetOpOffset(Operand op);

void PushOp(FILE *fout, Operand op, int size);

void PopOps(FILE *fout);

void GenerateCodes(FILE *fout);

void TrasnlateSingleObjectCode(FILE *fout, struct InterCodes *curIC);

void Load(FILE *fout, Operand op, int regNo);

void Store(FILE *fout, Operand op, int regNo);