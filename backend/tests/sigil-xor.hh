#ifndef SIGIL_XOR_HH
#define SIGIL_XOR_HH

#include "sigil.hh"
#include <memory>

sigil::Queryable make_xor(sigil::Queryable a, sigil::Queryable b)
{
        auto torch_l = sigil::make_torch({a});
        auto torch_r = sigil::make_torch({b});
        auto nand_inputs = sigil::make_dust({torch_l, torch_r});

        auto torch_bottom_l = sigil::make_torch({a});
        auto torch_bottom_r = sigil::make_torch({b});

        auto center = sigil::make_torch(
                {torch_bottom_l, torch_bottom_r, nand_inputs});

        auto dust_l = sigil::make_dust({center, torch_bottom_l});
        auto dust_r = sigil::make_dust({center, torch_bottom_r});

        auto torch_final_l = sigil::make_torch({dust_l});
        auto torch_final_r = sigil::make_torch({dust_r});

        auto result = sigil::make_dust({torch_final_l, torch_final_r});

        return std::make_shared<sigil::QueryableFn>(
                [result](int timestamp) { return (*result)(timestamp); });
}

#endif // SIGIL_XOR_HH

