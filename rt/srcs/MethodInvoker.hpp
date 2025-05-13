#ifndef METHODINVOKER_HPP
#define METHODINVOKER_HPP

#include <map>
#include <functional>
#include <optional>
#include <string>
#include <utility>
#include <type_traits>

template <typename Func>
class MethodInvoker {
	std::map<std::string, Func> methodMap;

public:
	MethodInvoker() = default;

	MethodInvoker(std::initializer_list<std::pair<std::string, Func>> init) {
		for (const auto& [name, func] : init)
			methodMap.emplace(name, func);
	}

	[[nodiscard]]
	bool contains(const std::string& methodName) const {
		return methodMap.contains(methodName);
	}

	Func& operator[](const std::string& methodName) {
		return methodMap.at(methodName);
	}

	template <typename... Args>
	[[nodiscard]]
	auto invoke(const std::string& methodName, Args&&... args)
		-> std::optional<std::invoke_result_t<Func, Args...>> {
		auto it = methodMap.find(methodName);
		if (it == methodMap.end()) return std::nullopt;

		if constexpr (std::is_void_v<std::invoke_result_t<Func, Args...>>) {
			it->second(std::forward<Args>(args)...);
			return std::nullopt;
		} else {
			return it->second(std::forward<Args>(args)...);
		}
	}
};

#endif // METHODINVOKER_HPP
