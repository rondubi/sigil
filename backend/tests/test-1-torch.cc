#include "sigil.hh"

#include <cassert>

int main()
{
        auto t_ptr = sigil::make_torch({sigil::make_pulse_set({1})});
        auto t = *t_ptr;

        assert(t(0));
        assert(!t(1));
        assert(t(2));

        printf("Success on torch test!\n");

        return 0;
}
