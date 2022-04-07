#ifndef SYMBOL_TABLE
#define SYMBOL_TABLE

typedef struct Type_* Type;
typedef struct FieldList_* FieldList;
typedef struct Function_* Function;
typedef struct SymbolNode_* SymbolNode;

struct Type_ {
    enum { BASIC, ARRAY, STRUCTURE, FUNCTION } kind;
    union {
        // 基本类型
        int basic;
        // 数组类型信息包括元素类型与数组大小构成
        struct { Type elem; int size; } array;
        // 结构体类型信息是一个链表
        FieldList structure;
        // Function
        Function function;
    } u;
 };

struct FieldList_ {
    char name[32]; // 域的名字
    Type type; // 域的类型
    FieldList tail; // 下一个域
};

struct Function_ {
    Type returnType;
    int paraNum;
    FieldList paraList;
};

struct SymbolNode_ {
    Type type;
    char sName[32];
    SymbolNode nextSymbolNode;
};

struct HashNode {
    SymbolNode firstSymbolNode;
};

void InitSymbolTable ();

int InsertSymbol (Type symbolType, char* symbolName);

int DeleteSymbol (char* symbolName);

int SearchSymbol(char* symbolName);

unsigned int Hash (char* name);

#endif