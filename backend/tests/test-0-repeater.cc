#include "sigil.hh"

#include <iostream>

void test_delay(int delay)
{
        auto r_ptr = sigil::make_repeater(sigil::make_pulse_set({0}), delay);
        auto r = *r_ptr;
        // Pulse delay
        for (int i = 0; i < delay; ++i)
                assert(!r(i));
        // Pulse extension
        for (int i = 0; i < delay; ++i)
                assert(r(delay) + i);
        assert(!r(delay * 2));

        printf("Success with delay = %d!\n", delay);
}

int main()
{
        test_delay(1);
        test_delay(2);
        test_delay(3);
        test_delay(4);
        return 0;
}
