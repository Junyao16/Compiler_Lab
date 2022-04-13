#include <stdio.h>
#include <stdlib.h>
#include "semanticAnalyzer.h"

void semanticAnalyzer() {
    InitSymbolTable();
    // Program → ExtDefList
    if(treeRoot != NULL && strcmp(treeRoot->nodeName, "Program") == 0){
        ExtDefList(treeRoot->firstChild);
    }
}

// ExtDefList → ExtDef ExtDefList
// | empty
void ExtDefList (struct TreeNode* curNode){
    if(curNode != NULL && strcmp(curNode->nodeName, "ExtDefList") == 0){
        if(curNode -> firstChild != NULL) {
            ExtDef(curNode -> firstChild);
            if(curNode -> firstChild -> nextSibling != NULL) {
                ExtDefList (curNode -> firstChild -> nextSibling);
            }
        }
        else{
            return;
        }
    }
}

// ExtDef → Specifier ExtDecList SEMI
// | Specifier SEMI
// | Specifier FunDec CompSt
void ExtDef (struct TreeNode* curNode){
    if(curNode != NULL && strcmp(curNode->nodeName, "ExtDef") == 0){
        if(curNode->firstChild != NULL){
            Type type = Specifier (curNode->firstChild);
            if(type == NULL) return;
            if(curNode->firstChild->nextSibling != NULL){
                if(strcmp(curNode->firstChild->nextSibling->nodeName, "ExtDecList") == 0){
                    ExtDecList(curNode->firstChild->nextSibling, type);
                }
                else if(strcmp(curNode->firstChild->nextSibling->nodeName, "SEMI") == 0){
                    
                }
                else if(strcmp(curNode->firstChild->nextSibling->nodeName, "FunDec") == 0){
                    FunDec(curNode->firstChild->nextSibling, type);
                    if(curNode->firstChild->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nodeName, "CompSt") == 0){
                        CompSt(curNode->firstChild->nextSibling->nextSibling, type);
                    }
                    //TODO:
                }
            }
        }
    }
}

// ExtDecList → VarDec
// | VarDec COMMA ExtDecList
void ExtDecList (struct TreeNode* curNode, Type type){
    if(curNode != NULL && strcmp(curNode->nodeName, "ExtDecList") == 0){
        if(curNode->firstChild != NULL){
            VarDec(curNode->firstChild, type, 0);
            if(curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "COMMA")){
                if(curNode->firstChild->nextSibling->nextSibling != NULL){
                    ExtDecList(curNode->firstChild->nextSibling->nextSibling, type);
                }
            }
        }
    }
}

// Specifier → TYPE
// | StructSpecifier
Type Specifier (struct TreeNode* curNode){
    if(curNode != NULL && strcmp(curNode->nodeName, "Specifier") == 0){
        if(curNode->firstChild != NULL){
            if(strcmp(curNode->firstChild->nodeName, "TYPE") == 0){
                Type type = (Type)malloc(sizeof(struct Type_));
                if(strcmp(curNode->firstChild->nodeValue.stringValue, "int") == 0){
                    type->kind = BASIC;
                    type->u.basic = INT_t;
                }
                else if(strcmp(curNode->firstChild->nodeValue.stringValue, "float") == 0){
                    type->kind = BASIC;
                    type->u.basic = FLOAT_t;
                }
                return type;
            }
            else if(strcmp(curNode->firstChild->nodeName, "StructSpecifier") == 0){
                Type type = StructSpecifier(curNode->firstChild);
                return type;
            }
        }
    }
    return NULL;
}

// StructSpecifier → STRUCT OptTag LC DefList RC
// | STRUCT Tag
Type StructSpecifier (struct TreeNode* curNode) {
    if(curNode != NULL && strcmp(curNode->nodeName, "StructSpecifier") == 0){
        if(curNode->firstChild != NULL){
            if(curNode->firstChild->nextSibling != NULL){
                if(strcmp(curNode->firstChild->nextSibling->nodeName, "OptTag") == 0){
                    Type type = (Type)malloc(sizeof(struct Type_));
                    strcpy(type->u.structure.structName, OptTag(curNode->firstChild->nextSibling));
                    if(type->u.structure.structName == "\0"){
                        type->kind = STRUCTURE;
                    }
                    else{
                        type->kind = STRUCT_NAME;
                    }
                    type->u.structure.fieldlist = NULL;
                    if(curNode->firstChild->nextSibling->nextSibling != NULL){
                        if(curNode->firstChild->nextSibling->nextSibling->nextSibling != NULL){
                            type->u.structure.fieldlist = DefList(curNode->firstChild->nextSibling->nextSibling->nextSibling, 1);
                            if(InsertFieldList(type->u.structure.fieldlist) == 0){
                                return NULL;
                            }                            
                        }
                    }
                    if(type->kind == STRUCT_NAME){
                        if(InsertSymbol (type, type->u.structure.structName) == -1){
                            ErrorOut(16, curNode->firstChild->nodeRow, type->u.structure.structName);
                        }
                    }
                    return type;
                }
                else if(strcmp(curNode->firstChild->nextSibling->nodeName, "Tag") == 0){
                    Type type = (Type)malloc(sizeof(struct Type_));
                    type->kind = STRUCTURE;
                    strcpy(type->u.structure.structName, Tag(curNode->firstChild->nextSibling));
                    if(SearchSymbol(type->u.structure.structName) == NULL){
                        ErrorOut(17, curNode->firstChild->nodeRow, type->u.structure.structName);
                        return NULL;
                    }
                    else{
                        type->u.structure = SearchSymbol(type->u.structure.structName)->type->u.structure;
                        //TODO: structure fieldlist copy
                        return type;
                    }
                }
            }
        }
    }
    return NULL;
}


// OptTag → ID
// | empty
char* OptTag (struct TreeNode* curNode) {
    if(curNode != NULL && strcmp(curNode->nodeName, "OptTag") == 0){
        if(curNode->firstChild != NULL){
            if(strcmp(curNode->firstChild->nodeName, "ID") == 0){
                return curNode->firstChild->nodeValue.stringValue;
            }
        }
        else {
            return "\0";
        }
    }
}

// Tag → ID
char* Tag (struct TreeNode* curNode){
    if(curNode != NULL && strcmp(curNode->nodeName, "Tag") == 0){
        if(curNode->firstChild != NULL){
            if(strcmp(curNode->firstChild->nodeName, "ID") == 0){
                return curNode->firstChild->nodeValue.stringValue;
            }
        }
    }
}

// VarDec → ID
// | VarDec LB INT RB
FieldList VarDec (struct TreeNode* curNode, Type type, int IsStruct){
    if(curNode != NULL && strcmp(curNode->nodeName, "VarDec") == 0){
        FieldList fieldlist = (FieldList)malloc(sizeof(struct FieldList_));
        if(curNode->firstChild != NULL){
            if(strcmp(curNode->firstChild->nodeName, "ID") == 0){
                fieldlist->type = type;
                strcpy(fieldlist->name, curNode->firstChild->nodeValue.stringValue);
                fieldlist->tail = NULL;
                fieldlist->Row = curNode->firstChild->nodeRow;
                if(IsStruct == 1){
                    return fieldlist;
                }
                if(SearchSymbol(curNode->firstChild->nodeValue.stringValue) == NULL){
                    InsertSymbol(type, curNode->firstChild->nodeValue.stringValue);
                    return fieldlist;
                }
                else {
                    ErrorOut(3, curNode->firstChild->nodeRow, curNode->firstChild->nodeValue.stringValue);
                    return NULL;
                }
            }
            else if (strcmp(curNode->firstChild->nodeName, "VarDec") == 0){
                Type tmpType = (Type)malloc(sizeof(struct Type_));
                tmpType->kind = ARRAY;
                tmpType->u.array.elem = type;
                
                if(curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "LB") == 0){
                    if(curNode->firstChild->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nodeName, "INT") == 0){
                        tmpType->u.array.size = curNode->firstChild->nextSibling->nextSibling->nodeValue.intValue;
                    }
                    else return NULL;
                }
                else{
                    return NULL;
                }
                return VarDec(curNode->firstChild, tmpType, IsStruct);
            }
        }
    }
    return NULL;
}

// FunDec → ID LP VarList RP
// | ID LP RP
void FunDec (struct TreeNode* curNode, Type type){
    if(curNode != NULL && strcmp(curNode->nodeName, "FunDec") == 0){
        if(curNode->firstChild != NULL && strcmp(curNode->firstChild->nodeName, "ID") == 0){
            if(SearchSymbol(curNode->firstChild->nodeValue.stringValue) != 0){
                ErrorOut(4, curNode->firstChild->nodeRow, curNode->firstChild->nodeValue.stringValue);
                return;
            }
            Type funType = (Type)malloc(sizeof(struct Type_));
            funType->kind = FUNCTION;
            funType->u.function.returnType = type;
            if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "LP") == 0){
                if (curNode->firstChild->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nodeName, "VarList") == 0){
                    funType->u.function.paraList = VarList(curNode->firstChild->nextSibling->nextSibling);
                }
                else if (curNode->firstChild->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nodeName, "RP") == 0){
                    funType->u.function.paraList = NULL;
                }
            }
            InsertSymbol(funType, curNode->firstChild->nodeValue.stringValue);
        }
    }
}

// VarList → ParamDec COMMA VarList
// | ParamDec
FieldList VarList (struct TreeNode* curNode){
    if(curNode != NULL && strcmp(curNode->nodeName, "VarList") == 0){
        if(curNode->firstChild != NULL && strcmp(curNode->firstChild->nodeName, "ParamDec") == 0){
            FieldList fieldlist = ParamDec(curNode->firstChild);
            if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "COMMA") == 0){
                if (curNode->firstChild->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nodeName, "VarList") == 0){
                    if(fieldlist != NULL) fieldlist->tail = VarList(curNode->firstChild->nextSibling->nextSibling);
                    else fieldlist = VarList(curNode->firstChild->nextSibling->nextSibling);
                }
            }
        }
    }
    return NULL;
}

// ParamDec → Specifier VarDec

FieldList ParamDec (struct TreeNode* curNode){
    if(curNode != NULL && strcmp(curNode->nodeName, "ParamDec") == 0){
        if(curNode->firstChild != NULL && strcmp(curNode->firstChild->nodeName, "Specifier") == 0){
            Type type = Specifier(curNode->firstChild);
            if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "VarDec") == 0){
                FieldList fieldlist = VarDec(curNode->firstChild->nextSibling, type, 0);
                return fieldlist;
            }
        }
    }
    return NULL;
}


// CompSt → LC DefList StmtList RC
void CompSt (struct TreeNode* curNode, Type retType){
    if(curNode != NULL && strcmp(curNode->nodeName, "CompSt") == 0){
        if(curNode->firstChild != NULL && strcmp(curNode->firstChild->nodeName, "LC") == 0){
            if(curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "DefList") == 0){
                FieldList fieldlist = DefList(curNode->firstChild->nextSibling, 0);
                if (curNode->firstChild->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nodeName, "StmtList") == 0){
                    StmtList(curNode->firstChild->nextSibling->nextSibling, retType);
                }
            }
            if(curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "StmtList") == 0){
                StmtList(curNode->firstChild->nextSibling, retType);
            }
        }
    }
}

// StmtList → Stmt StmtList
// | empty
void StmtList (struct TreeNode* curNode, Type retType){
    if(curNode != NULL && strcmp(curNode->nodeName, "StmtList") == 0){
        if(curNode->firstChild != NULL && strcmp(curNode->firstChild->nodeName, "Stmt") == 0){
            Stmt(curNode->firstChild, retType);
            if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "StmtList") == 0){
                StmtList(curNode->firstChild->nextSibling, retType);
            }
        }
        else if (curNode->firstChild == NULL){
            // TODO:
        }
    }
}

// Stmt → Exp SEMI
// | CompSt
// | RETURN Exp SEMI
// | IF LP Exp RP Stmt
// | IF LP Exp RP Stmt ELSE Stmt
// | WHILE LP Exp RP Stmt

void Stmt (struct TreeNode* curNode, Type retType){
    if(curNode != NULL && strcmp(curNode->nodeName, "Stmt") == 0){
        if(curNode->firstChild != NULL){
            if(strcmp(curNode->firstChild->nodeName, "Exp") == 0){
                Exp(curNode->firstChild);
            }
            else if(strcmp(curNode->firstChild->nodeName, "CompSt") == 0){
                CompSt(curNode->firstChild, retType);                
            }
            else if(strcmp(curNode->firstChild->nodeName, "RETURN") == 0){
                if(curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "Exp") == 0){
                    if(CompareType(Exp(curNode->firstChild->nextSibling), retType) != 1){
                        ErrorOut(8, curNode->firstChild->nextSibling->nodeRow, NULL);
                    }
                }
            }
            else if(strcmp(curNode->firstChild->nodeName, "IF") == 0){
                if(curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "LP") == 0){
                    if(curNode->firstChild->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nodeName, "Exp") == 0){
                        Type type = Exp(curNode->firstChild->nextSibling->nextSibling);
                        if(curNode->firstChild->nextSibling->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nextSibling->nodeName, "RP") == 0){
                            if(curNode->firstChild->nextSibling->nextSibling->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nextSibling->nextSibling->nodeName, "Stmt") == 0){
                                Stmt(curNode->firstChild->nextSibling->nextSibling->nextSibling->nextSibling, retType);
                                if(curNode->firstChild->nextSibling->nextSibling->nextSibling->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nextSibling->nextSibling->nextSibling->nodeName, "ELSE") == 0){
                                    if(curNode->firstChild->nextSibling->nextSibling->nextSibling->nextSibling->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nextSibling->nextSibling->nextSibling->nextSibling->nodeName, "Stmt") == 0){
                                        Stmt(curNode->firstChild->nextSibling->nextSibling->nextSibling->nextSibling->nextSibling->nextSibling, retType);
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else if(strcmp(curNode->firstChild->nodeName, "WHILE") == 0){
                if(curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "LP") == 0){
                    if(curNode->firstChild->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nodeName, "Exp") == 0){
                        Type type = Exp(curNode->firstChild->nextSibling->nextSibling);
                        if(curNode->firstChild->nextSibling->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nextSibling->nodeName, "RP") == 0){
                            if(curNode->firstChild->nextSibling->nextSibling->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nextSibling->nextSibling->nodeName, "Stmt") == 0){
                                Stmt(curNode->firstChild->nextSibling->nextSibling->nextSibling->nextSibling, retType);
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
FieldList DefList (struct TreeNode* curNode, int IsStruct){
    if(curNode != NULL && strcmp(curNode->nodeName, "DefList") == 0){
        if(curNode->firstChild != NULL && strcmp(curNode->firstChild->nodeName, "Def") == 0){
            FieldList fieldlist = Def(curNode->firstChild, IsStruct);
            if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "DefList") == 0){
                if(fieldlist != NULL) fieldlist->tail = DefList(curNode->firstChild->nextSibling, IsStruct);
                else fieldlist = DefList(curNode->firstChild->nextSibling, IsStruct);
            }
            return fieldlist;
        }
        else if (curNode->firstChild == NULL){
            return NULL;
        }
    }
    return NULL;
}

// Def → Specifier DecList SEMI
FieldList Def (struct TreeNode* curNode, int IsStruct){
    if(curNode != NULL && strcmp(curNode->nodeName, "Def") == 0){
        FieldList fieldlist = (FieldList)malloc(sizeof(struct FieldList_));
        if(curNode->firstChild != NULL && strcmp(curNode->firstChild->nodeName, "Specifier") == 0){
            Type type = Specifier(curNode->firstChild);
            if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "DecList") == 0){
                fieldlist = DecList(curNode->firstChild->nextSibling, type, IsStruct);
            }
        }
        return fieldlist;
    }
    return NULL;
}

// DecList → Dec
// | Dec COMMA DecList
FieldList DecList (struct TreeNode* curNode, Type type, int IsStruct){
    if(curNode != NULL && strcmp(curNode->nodeName, "DecList") == 0){
        FieldList fieldlist = (FieldList)malloc(sizeof(struct FieldList_));
        if(curNode->firstChild != NULL && strcmp(curNode->firstChild->nodeName, "Dec") == 0){
            fieldlist = Dec(curNode->firstChild, type, IsStruct);
            if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "COMMA") == 0){
                if (curNode->firstChild->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nodeName, "DecList") == 0){
                    if(fieldlist != NULL) fieldlist->tail = DecList(curNode->firstChild->nextSibling->nextSibling, type, IsStruct);
                    else fieldlist = DecList(curNode->firstChild->nextSibling->nextSibling, type, IsStruct);
                }
            }
        }
        return fieldlist;
    }
    return NULL;
}

// Dec → VarDec
// | VarDec ASSIGNOP Exp

FieldList Dec (struct TreeNode* curNode, Type type, int IsStruct){
    if(curNode != NULL && strcmp(curNode->nodeName, "Dec") == 0){
        FieldList fieldlist = (FieldList)malloc(sizeof(struct FieldList_));
        if(curNode->firstChild != NULL && strcmp(curNode->firstChild->nodeName, "VarDec") == 0){
            fieldlist = VarDec(curNode->firstChild, type, IsStruct);
            if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "ASSIGNOP") == 0){
                if(IsStruct == 1){
                    ErrorOut(15, curNode->firstChild->nextSibling->nodeRow, curNode->firstChild->nextSibling->nodeValue.stringValue);
                }
                if (curNode->firstChild->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nodeName, "Exp") == 0){
                    if(CompareType(fieldlist->type, Exp(curNode->firstChild->nextSibling->nextSibling)) == 0){
                        ErrorOut(5, curNode->firstChild->nodeRow, NULL);
                    }
                }
            }
        }
        return fieldlist;
    }
    return NULL;
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
Type Exp (struct TreeNode* curNode){
    if(curNode != NULL && strcmp(curNode->nodeName, "Exp") == 0){
        if(curNode->firstChild != NULL){
            if(strcmp(curNode->firstChild->nodeName, "Exp") == 0){
                Type type1 = Exp(curNode->firstChild);
                if(type1 == NULL){
                    return NULL;
                }
                if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "ASSIGNOP") == 0){
                    if (curNode->firstChild->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nodeName, "Exp") == 0){
                        Type type2 = Exp(curNode->firstChild->nextSibling->nextSibling);
                        if(type2 == NULL){
                            return NULL;
                        }
                        if(CompareType(type1, type2) == 0){
                            ErrorOut(5, curNode->firstChild->nextSibling->nodeRow, NULL);
                            return NULL;
                        }
                        else if(CompareType(type1, type2) == -1){
                            return NULL;
                        }
                        if(strcmp(curNode->firstChild->firstChild->nodeName, "ID") == 0 && curNode->firstChild->firstChild->nextSibling == NULL){

                        }
                        else if(strcmp(curNode->firstChild->firstChild->nodeName, "Exp") == 0 && curNode->firstChild->firstChild->nextSibling != NULL){
                            if(strcmp(curNode->firstChild->firstChild->nextSibling->nodeName, "LB") == 0 || strcmp(curNode->firstChild->firstChild->nextSibling->nodeName, "DOT") == 0){

                            }
                            else{
                                ErrorOut(6, curNode->firstChild->nextSibling->nodeRow, NULL);
                                return NULL;
                            }
                        }
                        else{
                            ErrorOut(6, curNode->firstChild->nextSibling->nodeRow, NULL);
                            return NULL;
                        }
                        return type1;
                    }
                }
                else if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "AND") == 0){
                    if (curNode->firstChild->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nodeName, "Exp") == 0){
                        Type type2 = Exp(curNode->firstChild->nextSibling->nextSibling);
                        if(type2 == NULL){
                            return NULL;
                        }
                        if(type1->kind == FUNCTION) type1 = type1->u.function.returnType;
                        if(type2->kind == FUNCTION) type2 = type2->u.function.returnType;
                        if(type1->kind != BASIC || type2->kind != BASIC || type1->u.basic != INT_t || type2->u.basic != INT_t){
                            ErrorOut(7, curNode->firstChild->nextSibling->nodeRow, NULL);
                            return NULL;
                        }
                        return type1;
                    }
                }
                else if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "OR") == 0){
                    if (curNode->firstChild->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nodeName, "Exp") == 0){
                        Type type2 = Exp(curNode->firstChild->nextSibling->nextSibling);
                        if(type2 == NULL){
                            return NULL;
                        }
                        if(type1->kind == FUNCTION) type1 = type1->u.function.returnType;
                        if(type2->kind == FUNCTION) type2 = type2->u.function.returnType;
                        if(type1->kind != BASIC || type2->kind != BASIC || type1->u.basic != INT_t || type2->u.basic != INT_t){
                            ErrorOut(7, curNode->firstChild->nextSibling->nodeRow, NULL);
                            return NULL;
                        }
                        return type1;
                    }
                }
                else if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "RELOP") == 0){
                    if (curNode->firstChild->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nodeName, "Exp") == 0){
                        Type type2 = Exp(curNode->firstChild->nextSibling->nextSibling);
                        if(type2 == NULL){
                            return NULL;
                        }
                        if(type1->kind == FUNCTION) type1 = type1->u.function.returnType;
                        if(type2->kind == FUNCTION) type2 = type2->u.function.returnType;
                        if(type1->kind != BASIC || type2->kind != BASIC || type1->u.basic != type2->u.basic){
                            ErrorOut(7, curNode->firstChild->nextSibling->nodeRow, NULL);
                            return NULL;
                        }
                        Type type = (Type)malloc(sizeof(struct Type_));
                        type->kind = BASIC;
                        type->u.basic = INT_t;
                        return type;
                    }
                }
                else if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "PLUS") == 0){
                    if (curNode->firstChild->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nodeName, "Exp") == 0){
                        Type type2 = Exp(curNode->firstChild->nextSibling->nextSibling);
                        if(type2 == NULL){
                            return NULL;
                        }
                        if(type1->kind == FUNCTION) type1 = type1->u.function.returnType;
                        if(type2->kind == FUNCTION) type2 = type2->u.function.returnType;
                        if(type1->kind != BASIC || type2->kind != BASIC || type1->u.basic != type2->u.basic){
                            ErrorOut(7, curNode->firstChild->nextSibling->nodeRow, NULL);
                            return NULL;
                        }
                        return type1;
                    }
                }
                else if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "MINUS") == 0){
                    if (curNode->firstChild->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nodeName, "Exp") == 0){
                        Type type2 = Exp(curNode->firstChild->nextSibling->nextSibling);
                        if(type2 == NULL){
                            return NULL;
                        }
                        if(type1->kind == FUNCTION) type1 = type1->u.function.returnType;
                        if(type2->kind == FUNCTION) type2 = type2->u.function.returnType;
                        if(type1->kind != BASIC || type2->kind != BASIC || type1->u.basic != type2->u.basic){
                            ErrorOut(7, curNode->firstChild->nextSibling->nodeRow, NULL);
                            return NULL;
                        }
                        return type1;
                    }
                }
                else if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "STAR") == 0){
                    if (curNode->firstChild->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nodeName, "Exp") == 0){
                        Type type2 = Exp(curNode->firstChild->nextSibling->nextSibling);
                        if(type2 == NULL){
                            return NULL;
                        }
                        if(type1->kind == FUNCTION) type1 = type1->u.function.returnType;
                        if(type2->kind == FUNCTION) type2 = type2->u.function.returnType;
                        if(type1->kind != BASIC || type2->kind != BASIC || type1->u.basic != type2->u.basic){
                            ErrorOut(7, curNode->firstChild->nextSibling->nodeRow, NULL);
                            return NULL;
                        }
                        return type1;
                    }
                }
                else if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "DIV") == 0){
                    if (curNode->firstChild->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nodeName, "Exp") == 0){
                        Type type2 = Exp(curNode->firstChild->nextSibling->nextSibling);
                        if(type2 == NULL){
                            return NULL;
                        }
                        if(type1->kind == FUNCTION) type1 = type1->u.function.returnType;
                        if(type2->kind == FUNCTION) type2 = type2->u.function.returnType;
                        if(type1->kind != BASIC || type2->kind != BASIC || type1->u.basic != type2->u.basic){
                            ErrorOut(7, curNode->firstChild->nextSibling->nodeRow, NULL);
                            return NULL;
                        }
                        return type1;
                    }
                }
                else if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "LB") == 0){
                    if (curNode->firstChild->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nodeName, "Exp") == 0){
                        Type type2 = Exp(curNode->firstChild->nextSibling->nextSibling);
                        if(type2 == NULL){
                            return NULL;
                        }
                        if(type1->kind == FUNCTION) type1 = type1->u.function.returnType;
                        if(type2->kind == FUNCTION) type2 = type2->u.function.returnType;
                        if(type1->kind != ARRAY){
                            ErrorOut(10, curNode->firstChild->nextSibling->nodeRow, curNode->firstChild->firstChild->nodeValue.stringValue);
                            return NULL;
                        }
                        if(type2->kind != BASIC || type2->u.basic != INT_t){
                            ErrorOut(12, curNode->firstChild->nextSibling->nextSibling->nodeRow, NULL);
                            return NULL;
                        }
                        return type1->u.array.elem;
                    }
                }
                else if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "DOT") == 0){
                    if (curNode->firstChild->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nodeName, "ID") == 0){
                        if(type1->kind == FUNCTION) type1 = type1->u.function.returnType;
                        if(type1->kind != STRUCTURE){
                            ErrorOut(13, curNode->firstChild->nextSibling->nodeRow, NULL);
                        }
                        else{
                            FieldList fieldlist = SearchFieldList(type1->u.structure.fieldlist, curNode->firstChild->nextSibling->nextSibling->nodeValue.stringValue);
                            if(fieldlist == NULL){
                                ErrorOut(14, curNode->firstChild->nextSibling->nodeRow, curNode->firstChild->nextSibling->nextSibling->nodeValue.stringValue);
                            }
                            else return fieldlist->type;
                        }
                        return NULL;
                    }
                }
            }
            else if(strcmp(curNode->firstChild->nodeName, "LP") == 0){
                if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "Exp") == 0){
                    return Exp(curNode->firstChild->nextSibling);
                }
            }
            else if(strcmp(curNode->firstChild->nodeName, "MINUS") == 0){
                if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "Exp") == 0){
                    Type type = Exp(curNode->firstChild->nextSibling);
                    if(type == NULL){
                        return NULL;
                    }
                    if(type->kind == FUNCTION) type = type->u.function.returnType;
                    if(type->kind != BASIC){
                        ErrorOut(7, curNode->firstChild->nodeRow, NULL);
                        return NULL;
                    }
                    return type;
                }
            }
            else if(strcmp(curNode->firstChild->nodeName, "NOT") == 0){
                if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "Exp") == 0){
                    Type type = Exp(curNode->firstChild->nextSibling);
                    if(type == NULL){
                        return NULL;
                    }
                    if(type->kind == FUNCTION) type = type->u.function.returnType;
                    if(type->kind != BASIC || type->u.basic != INT_t){
                        ErrorOut(7, curNode->firstChild->nodeRow, NULL);
                        return NULL;
                    }
                    return type;
                }
            }
            else if(strcmp(curNode->firstChild->nodeName, "ID") == 0){
                SymbolNode symbolnode = SearchSymbol(curNode->firstChild->nodeValue.stringValue);
                if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "LP") == 0){
                    if(symbolnode == NULL){
                        ErrorOut(2, curNode->firstChild->nodeRow, curNode->firstChild->nodeValue.stringValue);
                        return NULL;
                    }
                    else if(symbolnode->type->kind != FUNCTION){
                        ErrorOut(11, curNode->firstChild->nodeRow, curNode->firstChild->nodeValue.stringValue);
                        return NULL;
                    }
                    if (curNode->firstChild->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nodeName, "Args") == 0){
                        if(Args(curNode->firstChild->nextSibling->nextSibling, symbolnode->type->u.function.paraList) == 0){
                            ErrorOut(9, curNode->firstChild->nodeRow, curNode->firstChild->nodeValue.stringValue);
                            return NULL;
                        }
                    }
                    else if (curNode->firstChild->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nodeName, "RP") == 0){
                        if(symbolnode->type->u.function.paraList != NULL){
                            ErrorOut(9, curNode->firstChild->nodeRow, curNode->firstChild->nodeValue.stringValue);
                            return NULL;
                        }
                    }
                    return symbolnode->type;
                }
                else{
                    if(symbolnode == NULL){
                        ErrorOut(1, curNode->firstChild->nodeRow, curNode->firstChild->nodeValue.stringValue);
                        return NULL;
                    }
                    return symbolnode->type;
                }
            }
            else if(strcmp(curNode->firstChild->nodeName, "INT") == 0){
                Type type = (Type)malloc(sizeof(struct Type_));
                type->kind = BASIC;
                type->u.basic = INT_t;
                return type;
            }
            else if(strcmp(curNode->firstChild->nodeName, "FLOAT") == 0){
                Type type = (Type)malloc(sizeof(struct Type_));
                type->kind = BASIC;
                type->u.basic = FLOAT_t;
                return type;
            }
        }

    }
}


// Args → Exp COMMA Args
// | Exp
int Args (struct TreeNode* curNode, FieldList fieldlist){
    if(curNode != NULL && strcmp(curNode->nodeName, "Args") == 0){
        if(curNode->firstChild != NULL && strcmp(curNode->firstChild->nodeName, "Exp") == 0){
            if(fieldlist == NULL){
                return 0;
            }
            if(fieldlist != NULL && CompareType(Exp(curNode->firstChild), fieldlist->type) != 1) {
                return 0;
            }
            if (curNode->firstChild->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nodeName, "COMMA") == 0){
                if (curNode->firstChild->nextSibling->nextSibling != NULL && strcmp(curNode->firstChild->nextSibling->nextSibling->nodeName, "Args") == 0){
                    return Args(curNode->firstChild->nextSibling->nextSibling, fieldlist->tail);
                }
            }
            else{
                if(fieldlist->tail == NULL) return 1;
            }
        }
    }
    return 0;
}

void ErrorOut(int errorType, int errorRow, char* msg){
    printf("Error type %d at Line %d: ", errorType, errorRow);
	switch (errorType){
		case 1:
			printf("Undefined variable \"%s\".\n", msg);
			break;
		case 2:
			printf("Undefined function \"%s\".\n", msg);
			break;
		case 3:
			printf("Redefined variable \"%s\".\n", msg);
			break;
		case 4:
			printf("Redefined function \"%s\".\n", msg);
			break;
		case 5:
			printf("Type mismatched for assignment.\n");
			break;
		case 6:
			printf("The left-hand side of an assignment must be a variable.\n");
			break;
		case 7:
			printf("Type mismatched for operands.\n");
			break;
		case 8:
			printf("Type mismatched for return.\n");
			break;
		case 9:
			printf("Function \"%s\" is not applicable for arguments.\n", msg);
			break;
		case 10:
			printf("\"%s\" is not an array.\n", msg);
			break;
		case 11:
			printf("\"%s\" is not a function.\n", msg);
			break;
		case 12:
			printf("Not an integer.\n");
			break;
		case 13:
			printf("Illegal use of \".\".\n");
			break;
		case 14:
			printf("Non-existent field \"%s\".\n", msg);
			break;
		case 15:
			printf("Redefined field \"%s\".\n", msg);
			break;
		case 16:
			printf("Duplicated name \"%s\".\n", msg);
			break;
		case 17:
			printf("Undefined structure \"%s\".\n", msg);
			break;
	}
}

//TODO:
int CompareType(Type type1, Type type2){
    if(type1 == NULL || type2 ==NULL) return -1;
    if(type1->kind != type2->kind){
        if(type2->kind == FUNCTION){
            if(CompareType(type1, type2->u.function.returnType) == 1) return 1;
        }
        return 0;
    } 
    if(type1->kind == BASIC){
        if(type1->u.basic == type2->u.basic) return 1;
    }
    else if(type1->kind == ARRAY){
        if(type1->u.array.size == type2->u.array.size && CompareType(type1->u.array.elem, type2->u.array.elem) == 1) return 1;
    }
    else if(type1->kind == STRUCTURE){
        if(CompareFieldList(type1->u.structure.fieldlist, type2->u.structure.fieldlist) == 1) return 1;
    }
    else if(type1->kind == FUNCTION){
        if(CompareType(type1->u.function.returnType, type2->u.function.returnType) == 1) return 1;
    }
    return 0;
}

int CompareFieldList(FieldList fieldlist1, FieldList fieldlist2){
    if(fieldlist1 == NULL || fieldlist2 == NULL) return 0;
    FieldList tmp1 = fieldlist1;
    FieldList tmp2 = fieldlist2;
    while(tmp1 != NULL && tmp2 != NULL){
        if(CompareType(tmp1->type, tmp2->type) != 1) return 0;
        tmp1 = tmp1->tail;
        tmp2 = tmp2->tail;
    }
    if(tmp1 != NULL || tmp2 != NULL) return 0;
    return 1;
}

int InsertFieldList(FieldList fieldlist){
    int error = 0;
    FieldList tmp = fieldlist -> tail;
    FieldList tmp1 = fieldlist;
    while(tmp != NULL){
        tmp1 = fieldlist;
        while(tmp1 != tmp){
            if(strcmp(tmp->name, tmp1->name) == 0){
                ErrorOut(15, tmp->Row, tmp->name);
                error = 1;
                break;
            }
            tmp1 = tmp1->tail;
        }
        if(tmp->type->kind == STRUCTURE){
            InsertFieldList(tmp->type->u.structure.fieldlist);
        }
        else{
            if(SearchSymbol(tmp->name) == 0){
                InsertSymbol(tmp->type, tmp->name);
            }
            else{
                error = 1;
            }
        }
        tmp = tmp -> tail;
    }
    if(error == 1) return 0;
    return 1;
}

FieldList SearchFieldList(FieldList fieldlist, char* name){
    while(fieldlist != NULL){
        if(strcmp(fieldlist->name, name) == 0){
            return fieldlist;
        }
        fieldlist = fieldlist->tail;
    }
    return NULL;
}