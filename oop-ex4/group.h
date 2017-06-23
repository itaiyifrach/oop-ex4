#pragma once
#include <iostream>
#include <algorithm>
#include <list>
#include <map>
#include <functional>
#include <string>
#include <cstring>

//using namespace std;

template <typename T>
using deref_iter_t = std::remove_reference_t<decltype(*std::declval<T>())>;

template<class Iterator, class GroupingFunc>
auto groupValues(Iterator begin, Iterator end, GroupingFunc groupingFunc) {
	using T = deref_iter_t<Iterator>;
	using GroupingType = std::result_of_t<GroupingFunc(T&)>;
	std::map<GroupingType, std::list<T>> groups;
	std::for_each(begin, end, [&groups, groupingFunc](const auto& val) {
		groups[groupingFunc(val)].push_back(val);
	});
	return groups;
}

template<class MapOfLists>
void print(const MapOfLists& m) {
	for (const auto& p : m) {
		std::cout << p.first << std::endl << "-----" << std::endl;
		for (const auto& val : p.second) {
			std::cout << val << std::endl;
		}
		std::cout << "-----" << std::endl;
	}
}

int main() {
	std::list<std::string> strs = { "hello", "world", "Hello", "World" };
	auto groupOfStrings = groupValues(strs.begin(), strs.end(), [](auto& val) {
		return (char)std::toupper(val.at(0));
	});
	print(groupOfStrings);

	std::list<int> numbers = { 1, 5, 10, 24, 13 };
	auto groupOfNumbers = groupValues(numbers.begin(), numbers.end(), [](int val) {
		int decile = int(val / 10) * 10;
		return std::to_string(decile) + '-' + std::to_string(decile + 9);
	});
	print(groupOfNumbers);
}
