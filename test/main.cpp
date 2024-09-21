//
//  main.cpp
//  test
//
//  Created by uru on 19/09/2024.
//

#include <iostream>
#include <vector>

typedef std::vector<float>	floatSet_t;

enum Controls {
	NEXT,
	PREVIOUS,
	CHANGE_SOFT_SHADOW_LENGTH,
	CHANGE_SOFT_SHADOW_SOFTNESS,
};

float getValue(const floatSet_t& set, float val, int key) {
	size_t i = 0, size = set.size();
	if (size > 0) {
		while ( i < size && set[i] != val)
			i++;
		if (i != size) {
			switch (key) {
				case NEXT:		i++; break;
				case PREVIOUS:	i--; break;
			}
			if (i >= 0 && i < size)
				val = set[i];
		} else {
			val = set[0];
		}
	}
	return val;
}

float getValue(float val, int option, int key) {
	static floatSet_t softShadowLengths = {
		0.0, 1.1, 1.2, 1.4, 1.6, 1.8, 2.0, 2.2, 2.4, 2.6
	};
	static floatSet_t softShadowSoftnesses = {
		0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0
	};
	if (option == CHANGE_SOFT_SHADOW_LENGTH) {
		return getValue(softShadowLengths, val, key);
	}else if (option == CHANGE_SOFT_SHADOW_SOFTNESS) {
		return getValue(softShadowSoftnesses, val, key);
	}
	return val;
}

int main(void) {
	float v = 2.1;
	v = getValue(v, CHANGE_SOFT_SHADOW_LENGTH, PREVIOUS);
	std::cout << v << "\n";
	v = getValue(v, CHANGE_SOFT_SHADOW_LENGTH, PREVIOUS);
	std::cout << v << "\n";
	v = getValue(v, CHANGE_SOFT_SHADOW_LENGTH, PREVIOUS);
	std::cout << v << "\n";
	v = getValue(v, CHANGE_SOFT_SHADOW_LENGTH, PREVIOUS);
	std::cout << v << "\n";
	v = getValue(v, CHANGE_SOFT_SHADOW_LENGTH, PREVIOUS);
	std::cout << v << "\n";
	v = getValue(v, CHANGE_SOFT_SHADOW_LENGTH, NEXT);
	std::cout << v << "\n";
	v = getValue(v, CHANGE_SOFT_SHADOW_LENGTH, NEXT);
	std::cout << v << "\n";
	v = getValue(v, CHANGE_SOFT_SHADOW_LENGTH, NEXT);
	std::cout << v << "\n";
	v = getValue(v, CHANGE_SOFT_SHADOW_LENGTH, NEXT);
	std::cout << v << "\n";
	v = getValue(v, CHANGE_SOFT_SHADOW_LENGTH, NEXT);
	std::cout << v << "\n";
	v = getValue(v, CHANGE_SOFT_SHADOW_LENGTH, NEXT);
	std::cout << v << "\n";
	v = getValue(v, CHANGE_SOFT_SHADOW_LENGTH, NEXT);
	std::cout << v << "\n";
	v = getValue(v, CHANGE_SOFT_SHADOW_LENGTH, NEXT);
	std::cout << v << "\n";
	v = getValue(v, CHANGE_SOFT_SHADOW_LENGTH, NEXT);
	std::cout << v << "\n";
	v = getValue(v, CHANGE_SOFT_SHADOW_LENGTH, NEXT);
	std::cout << v << "\n";
	v = getValue(v, CHANGE_SOFT_SHADOW_LENGTH, PREVIOUS);
	std::cout << v << "\n";
	return 0;
}
