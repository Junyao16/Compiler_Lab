#ifndef SYMBOL_TABLE
#define SYMBOL_TABLE

typedef struct Type_ *Type;
typedef struct FieldList_ *FieldList;
typedef struct SymbolNode_ *SymbolNode;

struct Type_
{
    enum
    {
        BASIC,
        ARRAY,
        STRUCTURE,
        FUNCTION,
        STRUCT_NAME
    } kind;
    union {
        // 基本类型
        enum
        {
            INT_t,
            FLOAT_t
        } basic;
        // 数组类型信息包括元素类型与数组大小构成
        struct
        {
            Type elem;
            int size;
        } array;
        // 结构体类型信息是一个链表
        struct
        {
            FieldList fieldlist;
            char structName[32];
        } structure;
        // Function
        struct
        {
            Type returnType;
            FieldList paraList;
        } function;
    } u;
};

struct FieldList_
{
    char name[32];  // 域的名字
    Type type;      // 域的类型
    FieldList tail; // 下一个域
    int Row;
};

struct SymbolNode_
{
    Type type;
    char sName[32];

    int var_no;
    int offset;
    int isAddrParam;

    SymbolNode nextSymbolNode;
};

struct HashNode
{
    SymbolNode firstSymbolNode;
};

void InitSymbolTable();

int InsertSymbol(Type symbolType, char *symbolName);

int DeleteSymbol(char *symbolName);

SymbolNode SearchSymbol(char *symbolName);

unsigned int Hash(char *name);

void CreateReadFunction();
void CreateWriteFunction();

int GetSize(Type type);

#endif