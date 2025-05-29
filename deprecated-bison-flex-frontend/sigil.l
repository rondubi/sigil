%{
#include "sigil.tab.hh"
%}

%%

"module"                { return MODULE; }
"result"                { return RESULT; }
"dust"                  { return DUST; }
"torch"                 { return TORCH; }
"repeater"              { return REPEATER; }
"clock"                 { return CLOCK; }
"comparator"            { return COMPARATOR; }
"compare"               { return COMPARE; }
"subtract"              { return SUBTRACT; }

[0-9]+                  { yylval.ival = atoi(yytext); return INT; }
[a-zA-Z_][a-zA-Z_0-9]*  { yylval.sval = strdup(yytext); return IDENT; }

"->"                    { return ARROW; }
"="                     { return EQUAL; }
"."                     { return DOT; }
","                     { return COMMA; }
";"                     { return SEMICOLON; }
"("                     { return LPAREN; }
")"                     { return RPAREN; }
"{"                     { return LBRACE; }
"}"                     { return RBRACE; }
"<"                     { return LT; }
">"                     { return GT; }
"\$"                    { return DOLLAR; }

[ \t\r\n]+              { /* skip whitespace */ }

.                       { return yytext[0]; }

%%
