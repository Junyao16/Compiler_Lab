#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbolTable.h"

#define TABLE_LEN 0x3fff

struct HashNode symbolTable[TABLE_LEN];

void InitSymbolTable()
{
    for (int i = 0; i < TABLE_LEN; i++)
    {
        symbolTable[i].firstSymbolNode = NULL;
    }
    CreateReadFunction();
    CreateWriteFunction();
}

int InsertSymbol(Type symbolType, char *symbolName)
{
    unsigned int hashValue = Hash(symbolName);
    //printf("Insert %s\n", symbolName);

    if (SearchSymbol(symbolName) != NULL)
    {
        return -1;
    }

    SymbolNode newNode = (SymbolNode)malloc(sizeof(struct SymbolNode_));
    if (newNode == NULL)
    {
        return 0;
    }
    newNode->type = symbolType;
    strcpy(newNode->sName, symbolName);
    newNode->nextSymbolNode = symbolTable[hashValue].firstSymbolNode;
    newNode->var_no = -1;
    symbolTable[hashValue].firstSymbolNode = newNode;
    return 1;
}

int DeleteSymbol(char *symbolName)
{
    unsigned int hashValue = Hash(symbolName);

    SymbolNode tmp = symbolTable[hashValue].firstSymbolNode;
    if (tmp == NULL)
    {
        return 0;
    }
    if (strcmp(tmp->sName, symbolName) == 0)
    {
        symbolTable[hashValue].firstSymbolNode = symbolTable[hashValue].firstSymbolNode->nextSymbolNode;
        return 1;
    }
    while (tmp->nextSymbolNode != NULL)
    {
        if (strcmp(tmp->nextSymbolNode->sName, symbolName) == 0)
        {
            tmp->nextSymbolNode = tmp->nextSymbolNode->nextSymbolNode;
            return 1;
        }
        tmp = tmp->nextSymbolNode;
    }
    return 0;
}

SymbolNode SearchSymbol(char *symbolName)
{
    unsigned int hashValue = Hash(symbolName);

    SymbolNode tmp = symbolTable[hashValue].firstSymbolNode;
    if (tmp == NULL)
    {
        return NULL;
    }
    if (strcmp(tmp->sName, symbolName) == 0)
    {
        return tmp;
    }
    while (tmp->nextSymbolNode != NULL)
    {
        if (strcmp(tmp->nextSymbolNode->sName, symbolName) == 0)
        {
            return tmp->nextSymbolNode;
        }
        tmp = tmp->nextSymbolNode;
    }
    return NULL;
}

unsigned int Hash(char *name)
{
    unsigned int val = 0, i;
    for (; *name; ++name)
    {
        val = (val << 2) + *name;
        if (i = val & ~TABLE_LEN)
            val = (val ^ (i >> 12)) & TABLE_LEN;
    }
    return val;
}

void CreateReadFunction()
{
    Type retType = (Type)malloc(sizeof(struct Type_));
    retType->kind = BASIC;
    retType->u.basic = INT_t;
    Type type = (Type)malloc(sizeof(struct Type_));
    type->kind = FUNCTION;
    type->u.function.paraList = NULL;
    type->u.function.returnType = retType;
    InsertSymbol(type, "read");
}

void CreateWriteFunction()
{

    Type retType = (Type)malloc(sizeof(struct Type_));
    retType->kind = BASIC;
    retType->u.basic = INT_t;
    FieldList fieldlist = (FieldList)malloc(sizeof(struct FieldList_));
    strcpy(fieldlist->name, "writeValue");
    fieldlist->type = retType;
    fieldlist->tail = NULL;
    fieldlist->Row = -1;
    Type type = (Type)malloc(sizeof(struct Type_));
    type->kind = FUNCTION;
    type->u.function.paraList = fieldlist;
    type->u.function.returnType = retType;
    InsertSymbol(type, "write");
}