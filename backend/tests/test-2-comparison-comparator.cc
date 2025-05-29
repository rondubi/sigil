
#include "sigil.hh"

#include <iostream>

void test_comparator(
        const std::set<int> & l,
        const std::set<int> & r,
        const std::set<int> & b)
{
        auto c_ptr = sigil::make_compare_comparator(
                sigil::make_pulse_set(l),
                sigil::make_pulse_set(r),
                sigil::make_pulse_set(b));

        auto c = *c_ptr;

        std::set<int> combined_set = l;
        for (int i : r)
                combined_set.insert(i);

        for (int i : b)
                combined_set.insert(i);

        for (int i : combined_set)
                assert(c(i)
                       == b.contains(i)
                               * (l.contains(i) <= b.contains(i)
                                  && r.contains(i) <= b.contains(i)));
}

int main()
{
        test_comparator({0, 1, 2, 3}, {0, 2, 4, 6}, {0, 4});

        printf("Success on comparison comparator test!\n");

        return 0;
}
