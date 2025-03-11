#ifndef AVERAGE_HPP
# define AVERAGE_HPP

# include "Ray.hpp"


template <class t> class	Average3;
typedef Average3<int>		Average3i;
typedef Average3<float>		Average3f;

template <class t> class Average3 : private Vec3<t> {
	int	_n;
public:
	Average3(void) : Vec3<t>(), _n(0) {}
	Average3(const Average3<t>& other) : Vec3<t>(other.Vec3<t>), _n(other._n) {}
	~Average3(void) {}
	Average3<t>& operator=(const Average3<t>& other) {
		if (this != & other) {
			_n = other._n;
			for (int i = 0; i < 3; i++)
				this->raw[i] = other.raw[i];
		}
		return *this;
	}
	Average3<t>& add(const ARGBColor& c, float weight = 1) {
		if (weight > 0) {
			for (int i = 0; i < 3; i++)
				this->raw[i] += c.raw[i] * weight;
			_n++;
		}
		return *this;
	}
	Average3<t>& getARGBColor(ARGBColor& c) {
		for (int i = 0; i < 3; i++)
			c.raw[i] = (unsigned char)i2limits( std::round( (float)this->raw[i] / (float)_n ), 0, 255 );
		return *this; }
	inline ARGBColor getARGBColor(void) {
		ARGBColor c;
		getARGBColor(c);
		return c;
	}
};

#endif /* AVERAGE_HPP */
