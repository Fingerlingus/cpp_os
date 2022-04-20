#pragma once

#include "cuchar.hpp"

namespace nonstd {

using streamoff = long long;
using streampos = mbstate_t;
using wstreampos = mbstate_t;
using u8streampos = mbstate_t;
using u16streampos = mbstate_t;
using u32streampos = mbstate_t;

}
