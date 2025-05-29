#include "sigil.hh"
#include "sigil-xor.hh"

#include <iostream>

// This is a good example for how modules are going to look

void test_xor()
{
        auto xor_gate_ptr = make_xor(
                sigil::make_pulse_set({0, 1}), sigil::make_pulse_set({0, 2}));
        auto xor_gate = *xor_gate_ptr;

        assert(!xor_gate(0));
        assert(xor_gate(1));
        assert(xor_gate(2));
        assert(!xor_gate(3));
}

int main()
{
        test_xor();

        printf("Success on XOR test!\n");

        return 0;
}
