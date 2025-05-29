#ifndef SIGIL_AND_HH
#define SIGIL_AND_HH

#include "sigil.hh"

#include <memory>

sigil::Queryable make_and(sigil::Queryable a, sigil::Queryable b)
{
        // Assumes that copying is cheap. Otherwise this is an ownership
        // clusterfuck
        auto torch_l = sigil::make_torch({a});
        auto torch_r = sigil::make_torch({b});

        auto torch_c = sigil::make_torch({torch_l, torch_r});

        return std::make_shared<sigil::QueryableFn>(
                [torch_c](int timestamp) { return (*torch_c)(timestamp); });
}

#endif // SIGIL_AND_HH

