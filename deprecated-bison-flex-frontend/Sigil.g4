grammar Sigil;

prog: statement* EOF;

statement
    : module_decl
    | system_task
    ;

module_decl
    : 'module' identifier '(' ident_list? ')' '->' results_list module_body
    ;

ident_list
    : identifier (',' identifier)*
    ;

results_list
    : 'result'
    | ident_list
    ;

module_body
    : '{' (module_body_stmt ';')* '}'
    ;

module_body_stmt
    : component_decl
    | component_assignment
    | module_instantiation
    ;

component_decl
    : type identifier
    ;

component_assignment
    : identifier '=' val_expr
    ;

val_expr
    : identifier
    | identifier '.' identifier // instantiated_module_result_access
    ;

module_instantiation
    : identifier identifier '(' fn_arg_like? ')'
    ;

fn_arg_like
    : val_expr (',' val_expr)*
    ;

system_task
    : '$' identifier '(' fn_arg_like? ')' ';'
    ;

type
    : 'dust'
    | 'torch'
    | 'repeater' '<' repeater_delay '>'
    | 'clock' '<' INT ',' INT '>'
    | 'comparator' '<' ('compare' | 'subtract') '>'
    ;

repeater_delay
    : '1' | '2' | '3' | '4'
    ;

// Lexical rules

identifier
    : CHAR (CHAR | '_')*
    ;

CHAR
    : 'a'..'z' | 'A'..'Z'
    ;

INT
    : [0-9]+
    ;

WS
    : [ \t\r\n]+ -> skip
    ;

