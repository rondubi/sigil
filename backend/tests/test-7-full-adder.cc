#include "sigil.hh"
#include "sigil-add.hh"

#include <iostream>


void test_addition()
{
        for (int l = 0; l < 2; ++l)
        {
                for (int r = 0; r < 2; ++r)
                {
                        for (int c = 0; c < 2; ++c)
                        {
                                auto full_ptr = make_full_adder(
                                        std::make_shared<sigil::QueryableFn>(
                                                [l](int _) { return l % 2; }),
                                        std::make_shared<sigil::QueryableFn>(
                                                [r](int _) { return r % 2; }),
                                        std::make_shared<sigil::QueryableFn>(
                                                [c](int _) { return c % 2; }));
                                auto full = *full_ptr;

                                printf("Testing %d + %d + %d\n", l, r, c);

                                full_adder_res_t f = full(0);
                                const int result = f.bit + f.carry * 2;
                                printf("\tGot %d\n", result);
                                assert(result == l + r + c);
                        }
                }
        }
}

int main()
{
        test_addition();

        printf("Success with full-adder addition!\n");
        return 0;
}
