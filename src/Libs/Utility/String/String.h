#pragma once

#include "../Exception.h"

#include <array>
#include <magic_enum.hpp>
#include <sstream>
#include <string>
#include <string_view>

namespace tactics {

char tolower(char c);
char toupper(char c);

template<typename T> class Str {
public:
	static std::string to(T value) {
		return std::to_string(value);
	}

	static T from(const std::string& value) {
		T result;
		std::istringstream ss(value);
		if (!(ss >> result)) {
			throw TACTICS_EXCEPTION("Cannot convert string to type T");
		}
		return result;
	}
};

template<typename T> std::string toString(const T& value) {
	return Str<T>::to(value);
}

template<typename T> T fromString(const std::string& value) {
	return Str<T>::from(value);
}

template<typename T> std::array<T, 4> parseStringToVectorValues(const std::string& str) {
	std::array<T, 4> result = {};
	std::stringstream ss(str);
	std::string temp;
	size_t index = 0;

	while (std::getline(ss, temp, ',') && index < result.size()) {
		result[index++] = fromString<T>(temp);
	}

	return result;
}

} // namespace tactics

#define STRING_SERIALIZATION(TYPE)                 \
	namespace tactics {                            \
	template<> class Str<TYPE> {                   \
	public:                                        \
		static std::string to(const TYPE& value);  \
		static TYPE from(std::string_view string); \
	};                                             \
	}                                              \
	FORMAT_STR(TYPE);

#define STRING_ENUM_SERIALIZATION(TYPE)                                                        \
	namespace tactics {                                                                        \
	template<> class Str<TYPE> {                                                               \
	public:                                                                                    \
		static std::string to(const TYPE& value) {                                             \
			return std::string(magic_enum::enum_name(value));                                  \
		}                                                                                      \
		static TYPE from(std::string_view string) {                                            \
			if (auto value = magic_enum::enum_cast<TYPE>(string); value.has_value()) {         \
				return value.value();                                                          \
			} else {                                                                           \
				throw TACTICS_EXCEPTION("Cannot convert string {} to type {}", string, #TYPE); \
			}                                                                                  \
		}                                                                                      \
	};                                                                                         \
	}                                                                                          \
	FORMAT_STR(TYPE)

#define FORMAT_STR(TYPE)                                                                        \
	template<> struct fmt::formatter<TYPE> {                                                    \
	public:                                                                                     \
		constexpr auto parse(format_parse_context& ctx) {                                       \
			return ctx.begin();                                                                 \
		}                                                                                       \
                                                                                                \
		template<typename Context> constexpr auto format(TYPE const& obj, Context& ctx) const { \
			return fmt::format_to(ctx.out(), "{}", tactics::Str<TYPE>::to(obj));                \
		}                                                                                       \
	};