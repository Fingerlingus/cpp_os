#pragma once

enum class align_val_t : std::size_t { };
struct nothrow_t { explicit nothrow_t() = default; };
extern const nothrow_t nothrow;
