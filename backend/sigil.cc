#include "sigil.hh"

#include <iostream>

namespace sigil
{

// TODO: is C++ short-circuiting a problem?

Queryable make_repeater(Queryable input, int delay)
{
        assert(1 <= delay && delay <= 4 && "Repeater delay must be in [1, 4]");

        return std::make_shared<QueryableFn>(
                [input, delay](int timestamp)
                {
                        if (timestamp < delay)
                                return false;
                        // NOTE: repeaters extend pulses
                        for (int i = 0; i < delay; ++i)
                                if ((*input)(timestamp - delay - i))
                                        return true;
                        return false;
                });
}

// Negation of a union means the output is on unless all inputs are off.
// Lazy solution: union with dust, negate in torch.
Queryable make_torch(std::initializer_list<Queryable> input)
{
        Queryable dust = make_dust(input);
        return std::make_shared<QueryableFn>([dust](int timestamp)
                                { return !(*dust)(timestamp); });
}

Queryable make_dust(std::initializer_list<Queryable> input)
{
        // TODO: make less stupid
        std::vector ivec = input;
        return std::make_shared<QueryableFn>(
                [input = ivec](int timestamp)
                {
                        for (Queryable q : input)
                                if ((*q)(timestamp))
                                        return true;
                        return false;
                });
}

Queryable
make_subtract_comparator(Queryable left, Queryable right, Queryable back)
{
        return std::make_shared<QueryableFn>(
                [left, right, back](int timestamp)
                {
                        return (*back)(timestamp)
                                && !((*left)(timestamp) || (*right)(timestamp));
                });
}

Queryable
make_compare_comparator(Queryable left, Queryable right, Queryable back)
{
        return std::make_shared<QueryableFn>(
                [left, right, back](int timestamp)
                {
                        // In binary mode, not super exciting
                        // Keep the capture in case I want to support analog
                        return (*back)(timestamp);
                });
}

Queryable make_clock(int period, int pulse_duration)
{
        assert(pulse_duration <= period
               && "Clock pulse cannot be longer than period");
        return std::make_shared<QueryableFn>(
                [period, pulse_duration](int timestamp)
                { return (timestamp % period) < pulse_duration; });
}

Queryable make_pulse_set(const std::set<int> & pulse_times)
{
        return std::make_shared<QueryableFn>([pulse_times](int timestamp)
                                { return pulse_times.contains(timestamp); });
}

}; // namespace sigil
