#include "sigil.hh"
#include "sigil-and.hh"

#include <iostream>

// This is a good example for how modules are going to look

void test_and()
{
        auto and_gate_ptr = make_and(
                sigil::make_pulse_set({0, 1}), sigil::make_pulse_set({0, 2}));
        auto and_gate = *and_gate_ptr;

        assert(and_gate(0));
        assert(!and_gate(1));
        assert(!and_gate(2));
        assert(!and_gate(3));
}

int main()
{
        test_and();

        printf("Success on AND test!\n");

        return 0;
}
