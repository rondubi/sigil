#include <stdio.h>
#include "ast.hh"

extern "C" int yyparse(void);

extern SystemTask* root;

int main(void)
{
            int res = yyparse();
            printf("yyparse returned %d\n", res);
            return 0;
}

