#include "sigil.hh"
#include "sigil-add.hh"

#include <iostream>

struct four_bit_adder_res_t
{
        bool o0;
        bool o1;
        bool o2;
        bool o3;
        bool c;
};

using four_bit_adder = std::function<four_bit_adder_res_t(int)>;

std::shared_ptr<four_bit_adder> make_four_bit_adder(
        sigil::Queryable l0,
        sigil::Queryable l1,
        sigil::Queryable l2,
        sigil::Queryable l3,
        sigil::Queryable r0,
        sigil::Queryable r1,
        sigil::Queryable r2,
        sigil::Queryable r3)
{
        auto b0 = make_half_adder(l0, r0);
        auto b1 = make_full_adder(
                l1,
                r1,
                std::make_shared<sigil::QueryableFn>(
                        [b0](int timestamp)
                        { return (*b0)(timestamp).carry; }));
        auto b2 = make_full_adder(
                l2,
                r2,
                std::make_shared<sigil::QueryableFn>(
                        [b1](int timestamp)
                        { return (*b1)(timestamp).carry; }));
        auto b3 = make_full_adder(
                l3,
                r3,
                std::make_shared<sigil::QueryableFn>(
                        [b2](int timestamp)
                        { return (*b2)(timestamp).carry; }));

        return std::make_shared<four_bit_adder>(
                [b0, b1, b2, b3](int timestamp) -> four_bit_adder_res_t
                {
                        return {
                                .o0 = (*b0)(timestamp).bit,
                                .o1 = (*b1)(timestamp).bit,
                                .o2 = (*b2)(timestamp).bit,
                                .o3 = (*b3)(timestamp).bit,
                                .c = (*b3)(timestamp).carry,
                        };
                });
}

void test_addition()
{
        std::set<int> l0, l1, l2, l3, r0, r1, r2, r3;

        // Each pair is at a different timestamp
        // where timestamp = r * 16 + l;
        for (int l = 0; l <= 0xf; ++l)
        {
                for (int r = 0; r <= 0xf; ++r)
                {
                        const int timestamp = r * 0x10 + l;

                        if ((l >> 0) % 2 == 0)
                                l0.insert(timestamp);
                        if ((l >> 1) % 2 == 0)
                                l1.insert(timestamp);
                        if ((l >> 2) % 2 == 0)
                                l2.insert(timestamp);
                        if ((l >> 3) % 2 == 0)
                                l3.insert(timestamp);

                        if ((r >> 0) % 2 == 0)
                                r0.insert(timestamp);
                        if ((r >> 1) % 2 == 0)
                                r1.insert(timestamp);
                        if ((r >> 2) % 2 == 0)
                                r2.insert(timestamp);
                        if ((r >> 3) % 2 == 0)
                                r3.insert(timestamp);
                }
        }

        printf("Made sets\n");

        auto ad4b_ptr = make_four_bit_adder(
                sigil::make_pulse_set(l0),
                sigil::make_pulse_set(l1),
                sigil::make_pulse_set(l2),
                sigil::make_pulse_set(l3),
                sigil::make_pulse_set(r0),
                sigil::make_pulse_set(r1),
                sigil::make_pulse_set(r2),
                sigil::make_pulse_set(r3));
        auto ad4b = *ad4b_ptr;
        printf("Made adder\n");

        for (int l = 0; l <= 0xf; ++l)
        {
                for (int r = 0; r <= 0xf; ++r)
                {
                        printf("Testing %d + %d\n", l, r);

                        const int timestamp = r * 0x10 + l;

                        four_bit_adder_res_t f = ad4b(timestamp);
                        const int result = f.o0 + (f.o1 << 1) + (f.o2 << 2)
                                + (f.o3 << 3) + (f.c << 4);
                        printf("\tGot %d\n", result);
                        assert(result == 30 - (l + r));
                }
        }
}

int main()
{
        test_addition();

        printf("Success with 4-bit addition!\n");
        return 0;
}
