%{
    #include "syntax.tab.h"

    int yycolumn = 1;
    #define YY_USER_ACTION \
        yylloc.first_line = yylloc.last_line = yylineno; \
        yylloc.first_column = yycolumn; \
        yylloc.last_column = yycolumn + yyleng - 1; \
        yycolumn += yyleng;

    
    
    
    enum TYPE_NODE{
        TYPE_INT = 0,
        TYPE_FLOAT = 1,
        TYPE_OTHER = 2,
        TYPE_NONTERMINAL = 3
    };

    /* syntax tree node */
    struct TreeNode{
        struct TreeNode* firstChild;
        struct TreeNode* nextSibling;
        char nodeName[32];
        union{
            int intValue;
            float floatValue;
            char stringValue[32];
        }nodeValue;
        enum TYPE_NODE nodeType;
        int nodeRow;

    };

    struct TreeNode* createTreeNode(char* name, char* value, enum TYPE_NODE type, int row){
        struct TreeNode* newNode = (struct TreeNode*)malloc(sizeof(struct TreeNode));
        newNode->firstChild = NULL;
        newNode->nextSibling = NULL;
        newNode->nodeType = type;
        strcpy(newNode->nodeName, name);
        if(value == NULL){
            newNode->nodeValue.stringValue[0] = '\0';
        }
        else{
            switch (type){
                case TYPE_INT: {
                    newNode->nodeValue.intValue = atoi(value);
                    break;
                }
                case TYPE_FLOAT: {
                    newNode->nodeValue.floatValue = atof(value);
                    break;
                }
                case TYPE_OTHER: {
                    strcpy(newNode->nodeValue.stringValue, value);
                    break;
                }
                case TYPE_NONTERMINAL: {
                    strcpy(newNode->nodeValue.stringValue, value);
                    break;
                }
            }            
        }
        newNode->nodeRow = row;
        return newNode;
    }




    void addTreeNodeChild(struct TreeNode*parent, struct TreeNode*child){
        if(parent == NULL || child == NULL){
            return;
        }
        else{
            if(parent->firstChild == NULL){
                parent->firstChild = child;
            }
            else{
                struct TreeNode* tmp = parent->firstChild;
                while(tmp->nextSibling != NULL){
                    tmp = tmp->nextSibling;
                }
                tmp->nextSibling = child;
            }
        }
    }



    void printTree(struct TreeNode* node, int nodeDepth){
        if(node == NULL){
            return;
        }
        for(int i = 0; i < nodeDepth; i++){
            printf("  ");
        }
        //printf("%d\n", node->nodeType);
        switch(node->nodeType){
            case TYPE_INT: {
                printf("%s: %d\n", node->nodeName, node->nodeValue.intValue);
                break;
            }
            case TYPE_FLOAT: {
                printf("%s: %f\n", node->nodeName, node->nodeValue.floatValue);
                break;
            }
            case TYPE_OTHER: {
                if(node->nodeValue.stringValue[0] == '\0'){
                    printf("%s\n", node->nodeName);
                }
                else{
                    printf("%s: %s\n", node->nodeName, node->nodeValue.stringValue);
                }
                break;
            }
            case TYPE_NONTERMINAL: {
                printf("%s (%d)\n", node->nodeName, node->nodeRow);
                break;
            }
        }
        if(node -> firstChild != NULL){
            printTree(node->firstChild, nodeDepth+1);
        }
        if(node -> nextSibling != NULL){
            printTree(node->nextSibling, nodeDepth);
        }
    }

%}


%option yylineno

SEMI ";"
COMMA ","
ASSIGNOP "="
RELOP >|<|>=|<=|==|!=
PLUS "+"
MINUS "-"
STAR "*"
DIV "/"
AND "&&"
OR "||"
DOT "."
NOT "!"
TYPE int|float
LP "("
RP ")"
LB "["
RB "]"
LC "{"
RC "}"
STRUCT struct
RETURN return
IF if
ELSE else
WHILE while

WS ("\n"|"\t"|"\r"|" ")+

digit [0-9]
letter [a-zA-Z]

digits {digit}+
letters {letter}+

INT 0|([1-9]{digit}*)
/* A sequence of digits without spaces */

FLOAT ({digits}\.{digits})|((\.{digits}|{digits}\.|{digits}\.{digits})([eE][+-]?{digits}))
/* A real number consisting of digits and one decimal point. The deci-mal point must be surrounded by at least one digit */
/* float in exponential form */

ID ({letter}|_)({letter}|{digit}|_)*
/* A character string consisting of 52 upper- or lower-case alphabetic, 10 numeric and one underscore characters. Besides, an identifier must not start with a digit */

%%

{SEMI} {
    //printf("SEMI at line %d\n", yylineno);
    yylval.treeNode = createTreeNode("SEMI", NULL, TYPE_OTHER, yylineno);
    return SEMI;
}
{COMMA} {
    //printf("COMMA at line %d\n", yylineno);
    yylval.treeNode = createTreeNode("COMMA", NULL, TYPE_OTHER, yylineno);
    return COMMA;
}
{ASSIGNOP} {
    //printf("ASSIGNOP at line %d\n", yylineno);
    yylval.treeNode = createTreeNode("ASSIGNOP", NULL, TYPE_OTHER, yylineno);
    return ASSIGNOP;
}
{RELOP} {
    //printf("RELOP at line %d\n", yylineno);
    yylval.treeNode = createTreeNode("RELOP", yytext, TYPE_OTHER, yylineno);
    return RELOP;
}
{PLUS} {
    //printf("PLUS at line %d\n", yylineno);
    yylval.treeNode = createTreeNode("PLUS", NULL, TYPE_OTHER, yylineno);
    return PLUS;
}
{MINUS} {
    //printf("MINUS at line %d\n", yylineno);
    yylval.treeNode = createTreeNode("MINUS", NULL, TYPE_OTHER, yylineno);
    return MINUS;
}
{STAR} {
    //printf("STAR at line %d\n", yylineno);
    yylval.treeNode = createTreeNode("STAR", NULL, TYPE_OTHER, yylineno);
    return STAR;
}
{DIV} {
    //printf("DIV at line %d\n", yylineno);
    yylval.treeNode = createTreeNode("DIV", NULL, TYPE_OTHER, yylineno);
    return DIV;
}
{AND} {
    //printf("AND at line %d\n", yylineno);
    yylval.treeNode = createTreeNode("AND", NULL, TYPE_OTHER, yylineno);
    return AND;
}
{OR} {
    //printf("OR at line %d\n", yylineno);
    yylval.treeNode = createTreeNode("OR", NULL, TYPE_OTHER, yylineno);
    return OR;
}
{DOT} {
    //printf("DOT at line %d\n", yylineno);
    yylval.treeNode = createTreeNode("DOT", NULL, TYPE_OTHER, yylineno);
    return DOT;
}
{NOT} {
    //printf("NOT at line %d\n", yylineno);
    yylval.treeNode = createTreeNode("NOT", NULL, TYPE_OTHER, yylineno);
    return NOT;
}
{TYPE} {
    //printf("TYPE: %s at line %d\n", yytext, yylineno);
    yylval.treeNode = createTreeNode("TYPE", yytext, TYPE_OTHER, yylineno);
    return TYPE;
}
{LP} {
    //printf("LP at line %d\n", yylineno);
    yylval.treeNode = createTreeNode("LP", NULL, TYPE_OTHER, yylineno);
    return LP;
}
{RP} {
    //printf("RP at line %d\n", yylineno);
    yylval.treeNode = createTreeNode("RP", NULL, TYPE_OTHER, yylineno);
    return RP;
}
{LB} {
    //printf("LB at line %d\n", yylineno);
    yylval.treeNode = createTreeNode("LB", NULL, TYPE_OTHER, yylineno);
    return LB;
}
{RB} {
    //printf("RB at line %d\n", yylineno);
    yylval.treeNode = createTreeNode("RB", NULL, TYPE_OTHER, yylineno);
    return RB;
}
{LC} {
    //printf("LC at line %d\n", yylineno);
    yylval.treeNode = createTreeNode("LC", NULL, TYPE_OTHER, yylineno);
    return LC;
}
{RC} {
    //printf("RC at line %d\n", yylineno);
    yylval.treeNode = createTreeNode("RC", NULL, TYPE_OTHER, yylineno);
    return RC;
}
{STRUCT} {
    //printf("STRUCT at line %d\n", yylineno);
    yylval.treeNode = createTreeNode("STRUCT", NULL, TYPE_OTHER, yylineno);
    return STRUCT;
}
{RETURN} {
    //printf("RETURN at line %d\n", yylineno);
    yylval.treeNode = createTreeNode("RETURN", NULL, TYPE_OTHER, yylineno);
    return RETURN;
}
{IF} {
    //printf("IF at line %d\n", yylineno);
    yylval.treeNode = createTreeNode("IF", NULL, TYPE_OTHER, yylineno);
    return IF;
}
{ELSE} {
    //printf("ELSE at line %d\n", yylineno);
    yylval.treeNode = createTreeNode("ELSE", NULL, TYPE_OTHER, yylineno);
    return ELSE;
}
{WHILE} {
    //printf("WHILE at line %d\n", yylineno);
    yylval.treeNode = createTreeNode("WHILE", NULL, TYPE_OTHER, yylineno);
    return WHILE;
}

{INT} {
    //printf("INT: %d at line %d\n", atoi(yytext), yylineno);
    yylval.treeNode = createTreeNode("INT", yytext, TYPE_INT, yylineno);
    return INT;
}

{FLOAT} {
    //printf("FLOAT: %f at line %d\n", atof(yytext), yylineno);
    yylval.treeNode = createTreeNode("FLOAT", yytext, TYPE_FLOAT, yylineno);
    return FLOAT;
}

{ID} {
    //printf("ID: %s at line %d\n", yytext, yylineno);
    yylval.treeNode = createTreeNode("ID", yytext, TYPE_OTHER, yylineno);
    return ID;
}


"\n" { yycolumn = 1; }


{WS} {
    /* DO NOTHING */
}

. {
    printf("Error type A at line %d: Mysterious characters \'%s\'\n", yylineno, yytext);
}

%%