%{
#include <cstdio>
#include <cstdlib>
#include <vector>
#include "ast.hh"

void yyerror(const char *s);
int yylex(void);

// typedef struct {
//     int ival;
//     char *sval;
// } YYSTYPE;
// 
// #define YYSTYPE YYSTYPE

Prog * root = nullptr;

%}

%union {
    int ival;
    char* sval;

    struct Expr * expr;
    struct Expr * expr_list;
    struct Prog * prog;
    struct Statement * statement;
}

%type <expr> val_expr
%type <expr_list> fn_arg_like
%type <prog> statement_list;
%type <prog> prog;
%type <statement> statement;
%type <statement> module_decl;
%type <statement> system_task;

%token <sval> IDENT
%token <ival> INT
%token MODULE RESULT DUST TORCH REPEATER CLOCK COMPARATOR COMPARE SUBTRACT
%token ARROW EQUAL DOT COMMA SEMICOLON LPAREN RPAREN LBRACE RBRACE LT GT DOLLAR

%%

prog:
    statement_list YYEOF
    {
        root = $1;
    }
;

statement_list:
    /* empty */
    {
        $$ = new Prog();
    }
    | statement_list statement
    {
        $$ = $1;
        $$->stmts.push_back(*$2);
    }
;

statement:
    module_decl
    {
        $$ = $1;
    }
    | system_task
    {
        $$ = $1;
    }
;

module_decl:
    MODULE IDENT LPAREN ident_list RPAREN ARROW results_list module_body
    {
        $$ = new ModuleDecl();
    }
;

ident_list:
    IDENT
    | ident_list COMMA IDENT
;

results_list:
    RESULT
    | ident_list
;

module_body:
    LBRACE module_body_stmts RBRACE
;

module_body_stmts:
    /* empty */
    | module_body_stmts module_body_stmt SEMICOLON
;

module_body_stmt:
    component_decl
    | component_assignment
    | module_instantiation
;

component_decl:
    type IDENT
;

component_assignment:
    IDENT EQUAL val_expr
;

val_expr:
    IDENT
    {
        $$ = new IdentExpr($1);
    }
    | INT
    {
        $$ = new IntExpr($1);
    }
    | IDENT DOT IDENT
    {
        $$ = new MemberAccessExpr(std::make_unique<IdentExpr>($1), $3);
    }
;

module_instantiation:
    IDENT IDENT LPAREN fn_arg_like RPAREN
;

fn_arg_like:
    val_expr
    {
        $$ = new std::vector<Expr*>();
        $$->push_back($1);
    }
    | fn_arg_like COMMA val_expr
    {
        $1->push_back($3);
        $$ = $1;
    }
;

system_task:
    DOLLAR IDENT LPAREN fn_arg_like RPAREN SEMICOLON
    {
        $$ = new SystemTask{ $2, std::move(*$4) };
        delete $4;
    }
;

type:
    DUST
    | TORCH
    | REPEATER LT INT GT
    | CLOCK LT INT COMMA INT GT
    | COMPARATOR LT COMPARE GT
    | COMPARATOR LT SUBTRACT GT
;

%%

void yyerror(const char *s)
{
    fprintf(stderr, "Error: %s\n", s);
}
