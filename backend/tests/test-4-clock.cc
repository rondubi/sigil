#include "sigil.hh"

#include <iostream>

void test_clock()
{
        auto c_ptr = sigil::make_clock(3, 1);
        auto c = *c_ptr;

        assert(c(0));
        assert(!c(1));
        assert(!c(2));
        assert(c(3));

        printf("Success in clock test!\n");
}

int main()
{
        test_clock();

        return 0;
}
