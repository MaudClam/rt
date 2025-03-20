#ifndef RGB_HPP
# define RGB_HPP

# include <iostream>


const float _1_255(1. / 255);


// class Rgb
class Rgb {
protected:
	union {
		struct { float b, g, r; };
		float raw[3];
	};
	inline int _setСhar(unsigned char c, int i) const {
		return int(c) << (8 * i);
	}
	inline unsigned char _getСhar(int val, int i) const {
		return 0xFF & (val >> (8 * i));
	}
public:
	Rgb(void);
	Rgb(const Rgb& other);
	Rgb(int rgba);
	Rgb(float _r, float _g, float _b);
	~Rgb(void);
	virtual Rgb& operator=(const Rgb& other);
	virtual Rgb& operator=(int rgba);
	virtual Rgb& operator+=(const Rgb& other);
	virtual Rgb& operator+=(int rgba);
	virtual inline Rgb   operator*(float f) const { return Rgb(r * f, g * f, b * f); }
	virtual inline void  reset() { raw[0] = raw[1] = raw[2] = 0; }
	virtual inline float get_band(int i) const { return raw[i]; }
	Rgb& operator*=(const Rgb& other);
	Rgb& operator*=(int rgba);
	Rgb& operator*=(float f);
	Rgb& attenuate(int attenuation, float intensity);
	inline float get_unitBand(int i) const {
		return get_band(i) > 1 ? 1 : (get_band(i) < 0 ? 0 : get_band(i));
	}
	inline float get_maxBand(void) const {
		return std::max(std::max(get_band(0), get_band(1)), get_band(2)); }
	inline int   get_rgba(float gamma = 1) const {
		int rgba = 0;
		for (int i = 0; i < 3; i++) {
			rgba += _setСhar(255 * (gamma == 1 ? get_unitBand(i) : std::pow(get_unitBand(i), float(1. / gamma))), i);
		}
		return rgba;
	}
	inline bool  isNull(void) const { return raw[0] || raw[1] || raw[2];}
};


// class MeanRgb
class MeanRgb : public Rgb {
	int	_n;
public:
	MeanRgb(void);
	MeanRgb(const Rgb& other);
	MeanRgb(float _r, float _g, float _b);
	~MeanRgb(void);
	MeanRgb& operator=(const MeanRgb& other);
	MeanRgb& operator=(int rgba);
	MeanRgb& operator+=(const Rgb& rgb);
	MeanRgb& operator+=(int rgba);
	inline Rgb   operator*(float f) const { return Rgb(r * f / _n, g * f / _n, b * f / _n); }
	inline void  reset() { _n = 0; raw[0] = raw[1] = raw[2] = 0; }
	inline float get_band(int i) const { return raw[i] / _n; }
};


#endif
