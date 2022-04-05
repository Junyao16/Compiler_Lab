%{
    #include <stdio.h>
    #include <string.h>
    #include "lex.yy.c"

    int haveError = 0;

    void yyerror(char* msg){
        printf("Error type B at Line %d: %s.\n", yylval.treeNode->nodeRow, msg);
        haveError = 1;
    }

    int yyparse(void);


    struct TreeNode* treeRoot;
    
%}


%locations

%union {
  struct TreeNode* treeNode;
}


%token <treeNode>SEMI
%token <treeNode>COMMA
%token <treeNode>ASSIGNOP
%token <treeNode>RELOP
%token <treeNode>PLUS
%token <treeNode>MINUS
%token <treeNode>STAR
%token <treeNode>DIV
%token <treeNode> AND
%token <treeNode>OR
%token <treeNode>DOT
%token <treeNode>NOT
%token <treeNode>TYPE
%token <treeNode>LP
%token <treeNode>RP
%token <treeNode>LB
%token <treeNode>RB
%token <treeNode>LC
%token <treeNode>RC
%token <treeNode>STRUCT
%token <treeNode>RETURN
%token <treeNode>IF
%token <treeNode>ELSE
%token <treeNode>WHILE

%token <treeNode>INT
%token <treeNode>FLOAT 
%token <treeNode>ID


/* 7.1.2 High-level Definitions */
%type <treeNode>Program
%type <treeNode>ExtDefList
%type <treeNode>ExtDef
%type <treeNode>ExtDecList

/* 7.1.3 Specifiers */
%type <treeNode>Specifier
%type <treeNode>StructSpecifier
%type <treeNode>OptTag
%type <treeNode>Tag

/* 7.1.4 Declarators */
%type <treeNode>VarDec
%type <treeNode>FunDec
%type <treeNode>VarList
%type <treeNode>ParamDec

/* 7.1.5 Statements */
%type <treeNode>CompSt
%type <treeNode>StmtList
%type <treeNode>Stmt

/* 7.1.6 Local Definitions */
%type <treeNode>DefList
%type <treeNode>Def
%type <treeNode>DecList
%type <treeNode>Dec

/* 7.1.7 Expressions */
%type <treeNode>Exp
%type <treeNode>Args


%start Program


%right ASSIGNOP
%left OR AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT UMINUS
%left LP RP LB RB DOT


%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE


%%


/* 7.1.2 High-level Definitions */
Program: ExtDefList{
    $$ = createTreeNode("Program", NULL, TYPE_NONTERMINAL, @$.first_line);
    addTreeNodeChild($$, $1);
    treeRoot = $$;
};

ExtDefList: ExtDef ExtDefList{
    $$ = createTreeNode("ExtDefList", NULL, TYPE_NONTERMINAL, @$.first_line);
    addTreeNodeChild($$, $1);
    addTreeNodeChild($$, $2);
}
| /* empty */ {
    $$ = NULL;
};

ExtDef: Specifier ExtDecList SEMI{
    $$ = createTreeNode("ExtDef", NULL, TYPE_NONTERMINAL, @$.first_line);
    addTreeNodeChild($$, $1);
    addTreeNodeChild($$, $2);
    addTreeNodeChild($$, $3);
}
| Specifier SEMI{
    $$ = createTreeNode("ExtDef", NULL, TYPE_NONTERMINAL, @$.first_line);
    addTreeNodeChild($$, $1);
    addTreeNodeChild($$, $2);
}
| Specifier FunDec CompSt{
    $$ = createTreeNode("ExtDef", NULL, TYPE_NONTERMINAL, @$.first_line);
    addTreeNodeChild($$, $1);
    addTreeNodeChild($$, $2);
    addTreeNodeChild($$, $3);
}
| error SEMI{

}
| Specifier error SEMI{

}
| error Specifier SEMI{

;}

ExtDecList: VarDec{
    $$ = createTreeNode("ExtDecList", NULL, TYPE_NONTERMINAL, @$.first_line);
    addTreeNodeChild($$, $1);
}
| VarDec COMMA ExtDecList{
    $$ = createTreeNode("ExtDecList", NULL, TYPE_NONTERMINAL, @$.first_line);
    addTreeNodeChild($$, $1);
    addTreeNodeChild($$, $2);
    addTreeNodeChild($$, $3);
};


/* 7.1.3 Specifiers */
Specifier: TYPE{
    $$ = createTreeNode("Specifier", NULL, TYPE_NONTERMINAL, @$.first_line);
    addTreeNodeChild($$, $1);
}
| StructSpecifier{
    $$ = createTreeNode("Specifier", NULL, TYPE_NONTERMINAL, @$.first_line);
    addTreeNodeChild($$, $1);
};

StructSpecifier: STRUCT OptTag LC DefList RC{
    $$ = createTreeNode("StructSpecifier", NULL, TYPE_NONTERMINAL, @$.first_line);
    addTreeNodeChild($$, $1);
    addTreeNodeChild($$, $2);
    addTreeNodeChild($$, $3);
    addTreeNodeChild($$, $4);
    addTreeNodeChild($$, $5);
}
| STRUCT Tag{
    $$ = createTreeNode("StructSpecifier", NULL, TYPE_NONTERMINAL, @$.first_line);
    addTreeNodeChild($$, $1);
    addTreeNodeChild($$, $2);
}
| STRUCT OptTag LC error RC{

};

OptTag: ID{
    $$ = createTreeNode("OptTag", NULL, TYPE_NONTERMINAL, @$.first_line);
    addTreeNodeChild($$, $1);
}
| /* empty */{
    $$ = NULL;
};

Tag: ID{
    $$ = createTreeNode("Tag", NULL, TYPE_NONTERMINAL, @$.first_line);
    addTreeNodeChild($$, $1);
};


/* 7.1.4 Declarators */
VarDec: ID{
    $$ = createTreeNode("VarDec", NULL, TYPE_NONTERMINAL, @$.first_line);
    addTreeNodeChild($$, $1);
}
| VarDec LB INT RB{
    $$ = createTreeNode("VarDec", NULL, TYPE_NONTERMINAL, @$.first_line);
    addTreeNodeChild($$, $1);
    addTreeNodeChild($$, $2);
    addTreeNodeChild($$, $3);
    addTreeNodeChild($$, $4);
}
| VarDec LB error RB{

};

FunDec: ID LP VarList RP{
    $$ = createTreeNode("FunDec", NULL, TYPE_NONTERMINAL, @$.first_line);
    addTreeNodeChild($$, $1);
    addTreeNodeChild($$, $2);
    addTreeNodeChild($$, $3);
    addTreeNodeChild($$, $4);
}
| ID LP RP{
    $$ = createTreeNode("FunDec", NULL, TYPE_NONTERMINAL, @$.first_line);
    addTreeNodeChild($$, $1);
    addTreeNodeChild($$, $2);
    addTreeNodeChild($$, $3);
}
| ID LP error RP{

}
| ID error RP{

}
| error LP VarList RP{

};

VarList: ParamDec COMMA VarList{
    $$ = createTreeNode("VarList", NULL, TYPE_NONTERMINAL, @$.first_line);
    addTreeNodeChild($$, $1);
    addTreeNodeChild($$, $2);
    addTreeNodeChild($$, $3);
}
| ParamDec{
    $$ = createTreeNode("VarList", NULL, TYPE_NONTERMINAL, @$.first_line);
    addTreeNodeChild($$, $1);
};

ParamDec: Specifier VarDec{
    $$ = createTreeNode("ParamDec", NULL, TYPE_NONTERMINAL, @$.first_line);
    addTreeNodeChild($$, $1);
    addTreeNodeChild($$, $2);
};


/* 7.1.5 Statements */
CompSt: LC DefList StmtList RC{
    $$ = createTreeNode("CompSt", NULL, TYPE_NONTERMINAL, @$.first_line);
    addTreeNodeChild($$, $1);
    addTreeNodeChild($$, $2);
    addTreeNodeChild($$, $3);
    addTreeNodeChild($$, $4);
}
| LC error RC{

};

StmtList: Stmt StmtList{
    $$ = createTreeNode("StmtList", NULL, TYPE_NONTERMINAL, @$.first_line);
    addTreeNodeChild($$, $1);
    addTreeNodeChild($$, $2);
}
| /* empty */ {
    $$ = NULL;
};

Stmt: Exp SEMI{
    $$ = createTreeNode("Stmt", NULL, TYPE_NONTERMINAL, @$.first_line);
    addTreeNodeChild($$, $1);
    addTreeNodeChild($$, $2);
}
| CompSt{
    $$ = createTreeNode("Stmt", NULL, TYPE_NONTERMINAL, @$.first_line);
    addTreeNodeChild($$, $1);
}
| RETURN Exp SEMI{
    $$ = createTreeNode("Stmt", NULL, TYPE_NONTERMINAL, @$.first_line);
    addTreeNodeChild($$, $1);
    addTreeNodeChild($$, $2);
    addTreeNodeChild($$, $3);
}
| IF LP Exp RP Stmt %prec LOWER_THAN_ELSE{
    $$ = createTreeNode("Stmt", NULL, TYPE_NONTERMINAL, @$.first_line);
    addTreeNodeChild($$, $1);
    addTreeNodeChild($$, $2);
    addTreeNodeChild($$, $3);
    addTreeNodeChild($$, $4);
    addTreeNodeChild($$, $5);
}
| IF LP Exp RP Stmt ELSE Stmt{
    $$ = createTreeNode("Stmt", NULL, TYPE_NONTERMINAL, @$.first_line);
    addTreeNodeChild($$, $1);
    addTreeNodeChild($$, $2);
    addTreeNodeChild($$, $3);
    addTreeNodeChild($$, $4);
    addTreeNodeChild($$, $5);
    addTreeNodeChild($$, $6);
    addTreeNodeChild($$, $7);
}
| WHILE LP Exp RP Stmt{
    $$ = createTreeNode("Stmt", NULL, TYPE_NONTERMINAL, @$.first_line);
    addTreeNodeChild($$, $1);
    addTreeNodeChild($$, $2);
    addTreeNodeChild($$, $3);
    addTreeNodeChild($$, $4);
    addTreeNodeChild($$, $5);
}
| error SEMI{

}
| Exp error SEMI{

}
| RETURN Exp error{

}
| RETURN error SEMI{

}


/* 7.1.6 Local Definitions */
DefList: Def DefList{
    $$ = createTreeNode("DefList", NULL, TYPE_NONTERMINAL, @$.first_line);
    addTreeNodeChild($$, $1);
    addTreeNodeChild($$, $2);
}
| /* empty */ {
    $$ = NULL;
};

Def: Specifier DecList SEMI{
    $$ = createTreeNode("Def", NULL, TYPE_NONTERMINAL, @$.first_line);
    addTreeNodeChild($$, $1);
    addTreeNodeChild($$, $2);
    addTreeNodeChild($$, $3);
}
| Specifier error SEMI{

}
| Specifier DecList error{
    
}
| error SEMI{

};

DecList: Dec{
    $$ = createTreeNode("DecList", NULL, TYPE_NONTERMINAL, @$.first_line);
    addTreeNodeChild($$, $1);
}
| Dec COMMA DecList{
    $$ = createTreeNode("DecList", NULL, TYPE_NONTERMINAL, @$.first_line);
    addTreeNodeChild($$, $1);
    addTreeNodeChild($$, $2);
    addTreeNodeChild($$, $3);
};

Dec: VarDec{
    $$ = createTreeNode("Dec", NULL, TYPE_NONTERMINAL, @$.first_line);
    addTreeNodeChild($$, $1);
}
| VarDec ASSIGNOP Exp{
    $$ = createTreeNode("Dec", NULL, TYPE_NONTERMINAL, @$.first_line);
    addTreeNodeChild($$, $1);
    addTreeNodeChild($$, $2);
    addTreeNodeChild($$, $3);
};


/* 7.1.7 Expressions */
Exp: Exp ASSIGNOP Exp{
    $$ = createTreeNode("Exp", NULL, TYPE_NONTERMINAL, @$.first_line);
    addTreeNodeChild($$, $1);
    addTreeNodeChild($$, $2);
    addTreeNodeChild($$, $3);
}
| Exp AND Exp{
    $$ = createTreeNode("Exp", NULL, TYPE_NONTERMINAL, @$.first_line);
    addTreeNodeChild($$, $1);
    addTreeNodeChild($$, $2);
    addTreeNodeChild($$, $3);
}
| Exp OR Exp{
    $$ = createTreeNode("Exp", NULL, TYPE_NONTERMINAL, @$.first_line);
    addTreeNodeChild($$, $1);
    addTreeNodeChild($$, $2);
    addTreeNodeChild($$, $3);
}
| Exp RELOP Exp{
    $$ = createTreeNode("Exp", NULL, TYPE_NONTERMINAL, @$.first_line);
    addTreeNodeChild($$, $1);
    addTreeNodeChild($$, $2);
    addTreeNodeChild($$, $3);
}
| Exp PLUS Exp{
    $$ = createTreeNode("Exp", NULL, TYPE_NONTERMINAL, @$.first_line);
    addTreeNodeChild($$, $1);
    addTreeNodeChild($$, $2);
    addTreeNodeChild($$, $3);
}
| Exp MINUS Exp{
    $$ = createTreeNode("Exp", NULL, TYPE_NONTERMINAL, @$.first_line);
    addTreeNodeChild($$, $1);
    addTreeNodeChild($$, $2);
    addTreeNodeChild($$, $3);
}
| Exp STAR Exp{
    $$ = createTreeNode("Exp", NULL, TYPE_NONTERMINAL, @$.first_line);
    addTreeNodeChild($$, $1);
    addTreeNodeChild($$, $2);
    addTreeNodeChild($$, $3);
}
| Exp DIV Exp{
    $$ = createTreeNode("Exp", NULL, TYPE_NONTERMINAL, @$.first_line);
    addTreeNodeChild($$, $1);
    addTreeNodeChild($$, $2);
    addTreeNodeChild($$, $3);
}
| LP Exp RP{
    $$ = createTreeNode("Exp", NULL, TYPE_NONTERMINAL, @$.first_line);
    addTreeNodeChild($$, $1);
    addTreeNodeChild($$, $2);
    addTreeNodeChild($$, $3);
}
| MINUS Exp{
    $$ = createTreeNode("Exp", NULL, TYPE_NONTERMINAL, @$.first_line);
    addTreeNodeChild($$, $1);
    addTreeNodeChild($$, $2);
}
| MINUS Exp %prec UMINUS{
	$$ = createTreeNode("Exp", NULL, TYPE_NONTERMINAL, @$.first_line);
	addTreeNodeChild($$, $1);
    addTreeNodeChild($$, $2);
}
| NOT Exp{
    $$ = createTreeNode("Exp", NULL, TYPE_NONTERMINAL, @$.first_line);
    addTreeNodeChild($$, $1);
    addTreeNodeChild($$, $2);
}
| ID LP Args RP{
    $$ = createTreeNode("Exp", NULL, TYPE_NONTERMINAL, @$.first_line);
    addTreeNodeChild($$, $1);
    addTreeNodeChild($$, $2);
    addTreeNodeChild($$, $3);
    addTreeNodeChild($$, $4);
}
| ID LP RP{
    $$ = createTreeNode("Exp", NULL, TYPE_NONTERMINAL, @$.first_line);
    addTreeNodeChild($$, $1);
    addTreeNodeChild($$, $2);
    addTreeNodeChild($$, $3);
}
| Exp LB Exp RB{
    $$ = createTreeNode("Exp", NULL, TYPE_NONTERMINAL, @$.first_line);
    addTreeNodeChild($$, $1);
    addTreeNodeChild($$, $2);
    addTreeNodeChild($$, $3);
    addTreeNodeChild($$, $4);
}
| Exp DOT ID{
    $$ = createTreeNode("Exp", NULL, TYPE_NONTERMINAL, @$.first_line);
    addTreeNodeChild($$, $1);
    addTreeNodeChild($$, $2);
    addTreeNodeChild($$, $3);
}
| ID{
    $$ = createTreeNode("Exp", NULL, TYPE_NONTERMINAL, @$.first_line);
    addTreeNodeChild($$, $1);
}
| INT{
    $$ = createTreeNode("Exp", NULL, TYPE_NONTERMINAL, @$.first_line);
    addTreeNodeChild($$, $1);
}
| FLOAT{
    $$ = createTreeNode("Exp", NULL, TYPE_NONTERMINAL, @$.first_line);
    addTreeNodeChild($$, $1);
}
|Exp ASSIGNOP error{
    
};
|Exp AND error{
    
};
|Exp OR error{
    
}
|Exp RELOP error{
    
}
|Exp PLUS error{
    
}
|Exp MINUS error{
    
}
|Exp STAR error{
    
}
|Exp DIV error{
    
}
|LP error RP{
    
}
|MINUS error{
    
}
|NOT error{
    
}
|ID LP error RP{
    
}
|Exp LB error RB{
    
};

Args: Exp COMMA Args{
    $$ = createTreeNode("Args", NULL, TYPE_NONTERMINAL, @$.first_line);
    addTreeNodeChild($$, $1);
    addTreeNodeChild($$, $2);
    addTreeNodeChild($$, $3);
}
| Exp{
    $$ = createTreeNode("Args", NULL, TYPE_NONTERMINAL, @$.first_line);
    addTreeNodeChild($$, $1);
};



%%