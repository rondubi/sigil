#ifndef SIGIL_ADD_HH
#define SIGIL_ADD_HH

#include "sigil.hh"
#include "sigil-xor.hh"
#include "sigil-and.hh"

#include <functional>
#include <memory>

// This is a good model of how generated code for multiple-returns will look
struct half_adder_res_t
{
        bool bit;
        bool carry;
};

using half_adder = std::function<half_adder_res_t(int)>;

std::shared_ptr<half_adder>
make_half_adder(sigil::Queryable a, sigil::Queryable b)
{
        auto xor_gate = make_xor(a, b);
        auto and_gate = make_and(a, b);

        return std::make_shared<half_adder>(
                [xor_gate, and_gate](int timestamp) -> half_adder_res_t
                {
                        return {
                                .bit = (*xor_gate)(timestamp),
                                .carry = (*and_gate)(timestamp),
                        };
                });
}

struct full_adder_res_t
{
        bool bit;
        bool carry;
};

using full_adder = std::function<full_adder_res_t(int)>;

/*
 * NOTE: the following is wrong!
 *
full_adder
make_full_adder(sigil::Queryable l, sigil::Queryable r, sigil::Queryable carry)
{
        auto h1 = make_half_adder(l, r);
        auto h2 = make_half_adder(
                [h1](int timestamp) { return h1(timestamp).bit; }, carry);

        // We could just return h2(timestamp) but that's not how generated code
        // will look
        return [h2](int timestamp) -> full_adder_res_t
        {
                return {
                        .bit = h2(timestamp).bit,
                        .carry = h2(timestamp).carry,
                };
        };
}
*/

std::shared_ptr<full_adder>
make_full_adder(sigil::Queryable l, sigil::Queryable r, sigil::Queryable carry)
{
        auto h1 = make_half_adder(l, r);
        auto h2 = make_half_adder(
                std::make_shared<sigil::QueryableFn>(
                        [h1](int timestamp) { return (*h1)(timestamp).bit; }),
                carry);
        auto x = make_xor(
                std::make_shared<sigil::QueryableFn>(
                        [h1](int timestamp) { return (*h1)(timestamp).carry; }),
                std::make_shared<sigil::QueryableFn>(
                        [h2](int timestamp)
                        { return (*h2)(timestamp).carry; }));

        return std::make_shared<full_adder>(
                [h2, x](int timestamp) -> full_adder_res_t
                {
                        return {
                                .bit = (*h2)(timestamp).bit,
                                .carry = (*x)(timestamp),
                        };
                });
}

// We have half adder, we have full adder, where's Blackadder?

#endif // SIGIL_ADD_HH

