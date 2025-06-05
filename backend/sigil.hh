#ifndef SIGIL_HH
#define SIGIL_HH

#include <cassert>
#include <functional>
#include <initializer_list>
#include <memory>
#include <set>

namespace sigil
{

using QueryableFn = std::function<bool(int)>;
using Queryable = std::shared_ptr<QueryableFn>;

Queryable make_repeater(Queryable input, int delay);

template <int d>
Queryable make_repeater(Queryable input)
{
        return make_repeater(input, d);
}

Queryable make_torch(std::initializer_list<Queryable> input);

Queryable make_dust(std::initializer_list<Queryable> input);

Queryable
make_subtract_comparator(Queryable left, Queryable right, Queryable back);

Queryable
make_compare_comparator(Queryable left, Queryable right, Queryable back);

Queryable make_clock(int period, int pulse_duration);

template <int period, int pulse_duration>
Queryable make_clock()
{
        return make_clock(period, pulse_duration);
}

// Must be ordered
Queryable make_pulse_set(const std::set<int> & pulse_times);

}; // namespace sigil
 
#endif // SIGIL_HH

