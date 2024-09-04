//
//  main.cpp
//  combine_test
//
//  Created by uru on 03/09/2024.
//

# include <iostream>
# include <forward_list>
# include <vector>

typedef std::forward_list<int>	list_t;
typedef list_t::iterator		list_it;
typedef std::vector<list_it>	delete_t;

std::ostream& operator<<(std::ostream& o, list_t& list) {
	int i = 0;
	for (auto l = list.begin(); l != list.end(); ++l, i++) {
		if (l != list.begin()) {
			o << ",";
		}
		o << *l;
	}
	o << " size=" << i;
	return o;
}

int main(void) {
	list_t		list = {0,1,2,3,4,5,6,7,8,9};
	delete_t	del;

	std::cout << list << std::endl;
	for (auto it = list.before_begin(), end = list.end(); it != end; ++it) {
		auto l = it;
		if (++l != end) {
			if ( *l % 3 == 0 ) {
				del.push_back(it);
			}
			if ( *l % 2 == 0 ) {
				list.emplace_front( *l * 10);
			}
		}
	}
	for (auto d = del.begin(), end = del.begin(); d != end; ++d) {
		list.erase_after(*d);
	}
	std::cout << list << std::endl;
	std::cout << "Hello, World!\n";
	return 0;
}
