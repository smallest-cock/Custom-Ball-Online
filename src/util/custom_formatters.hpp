#pragma once

#include "RLSDK/RLSDK/SDK_HEADERS/Core_structs.hpp"
template <>
struct std::formatter<FString> {
	constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }
	template <typename FormatContext>
	auto format(const FString &str, FormatContext &ctx) const {
		return std::format_to(ctx.out(), "{}", str.ToString());
	}
};

template <>
struct std::formatter<FName> {
	constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }
	template <typename FormatContext>
	auto format(const FName &str, FormatContext &ctx) const {
		return std::format_to(ctx.out(), "{}", str.ToString());
	}
};

namespace std {
	template <typename T>
	    requires std::is_base_of_v<UObject, std::remove_pointer_t<T>>
	struct formatter<T, char> {
		constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }

		template <typename FormatContext>
		auto format(T obj, FormatContext &ctx) const {
			using PtrType = std::conditional_t<std::is_pointer_v<T>, T, std::add_pointer_t<T>>;

			if constexpr (std::is_pointer_v<T>) {
				if (!obj) {
					const std::string_view null_str = "nullptr";
					return std::ranges::copy(null_str, ctx.out()).out;
				}
			}

			return format_to(ctx.out(), "0x{:X}", reinterpret_cast<uintptr_t>(static_cast<PtrType>(obj)));
		}
	};
}

template <>
struct std::formatter<FVector> {
	constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }
	template <typename FormatContext>
	auto format(const FVector &vec, FormatContext &ctx) const {
		return std::format_to(ctx.out(), "[X:{:.2f}-Y:{:.2f}-Z:{:.2f}]", vec.X, vec.Y, vec.Z);
	}
};

template <>
struct std::formatter<FLinearColor> {
	constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }
	template <typename FormatContext>
	auto format(const FLinearColor &col, FormatContext &ctx) const {
		return std::format_to(ctx.out(), "[R:{:.2f}-G:{:.2f}-B:{:.2f}-A:{:.2f}]", col.R, col.G, col.B, col.A);
	}
};

template <>
struct std::formatter<FColor> {
	constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }
	template <typename FormatContext>
	auto format(const FColor &col, FormatContext &ctx) const {
		return std::format_to(ctx.out(), "[R:{}-G:{}-B:{}]", col.R, col.G, col.B, col.A);
	}
};

template <>
struct std::formatter<FRotator> {
	constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }
	template <typename FormatContext>
	auto format(const FRotator &rot, FormatContext &ctx) const {
		return std::format_to(ctx.out(), "[Pitch:{}-Yaw:{}-Roll:{}]", rot.Pitch, rot.Yaw, rot.Roll);
	}
};
