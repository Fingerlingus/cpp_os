#pragma once

namespace nonstd {

template <typename T> typename std::remove_reference_t<T>&& move(T&& arg) {
	return static_cast<typename std::remove_reference_t<T>&&>(arg);
}

} // namespace nonstd
