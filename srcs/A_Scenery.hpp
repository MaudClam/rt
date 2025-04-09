#ifndef ASCENERY_HPP
# define ASCENERY_HPP

# include "Ray.hpp"
# include "PhotonMap.hpp"


struct	Ray;
struct	Rays;
struct	HitRecord;
class	PhotonMap;
typedef	Rays			phRays_t;
typedef	std::mt19937	rand_gen_t;


class A_Scenery {
protected:
	int				_id;
	std::string		_name;
	std::string		_nick;
	bool			_isLight;
	Position		_pos;
	ARGBColor		_color;		// surface color
	Lighting 		_light;		// light for light source
public:
	CombineType		combineType;// combination type with the following primitive
	float			glossy;		// in range [0,1] matteness, (1,1000] glossy
	float			reflective;	// in range [0,1]
	float			refractive; // in range [0,1]
	float			diffusion;	// in range [0,1]
	float			matIOR;		// refractive in/out index relative to air
	float			matOIR;		// refractive out/in index relative to air
	A_Scenery(void);
	A_Scenery(std::string name, std::string nick, bool isLight = false);
	virtual ~A_Scenery(void);
	A_Scenery(const A_Scenery& other);
	
	inline int          get_id(void) const { return _id; }
	inline std::string  get_name(void) const { return std::string(_name); }
	inline std::string  get_nick(void) const { return std::string(_nick); }
	inline bool         get_isLight(void) const { return _isLight; }
	inline Position     get_pos(void) const { return Position(_pos); }
	inline Rgb          get_light(void) const { return _light.light; }
	inline ARGBColor    get_color(void) const { return ARGBColor(_color); }
	inline float        get_mattness(void) { return glossy > 0 && glossy <= 1 ? glossy : 0; }
	inline float        get_glossy(void) const {
		if (glossy > 1)
			return glossy;
		if (glossy <= MATTNESS_GLOSSY_LIMIT)
			return MATTNESS_GLOSSY_RATIO / glossy;
		return 0;
	}
	inline Probability  get_probability(void) const {
		return Probability(reflective, refractive, diffusion); }
	inline void         get_probability(Probability& p, float max = 1) const {
		p.raw[0] = reflective * (diffusion ? 1 : max);
		p.raw[1] = p.raw[0] + refractive * max;
		p.raw[2] = p.raw[1] + diffusion * max;
}
	inline void	set_id(int id) { _id = id; }
	inline void	set_name(const std::string& name) { _name = name; }
	inline void	set_nick(const std::string& nick) { _nick = nick; }
	inline void	set_isLight(bool isLight) { _isLight = isLight; }
	inline void	set_pos(const Position& pos) { _pos = pos; }
	inline void	set_color(const ARGBColor& color) { _color = color; }
	inline void set_material(std::istringstream& is) {
		is >> _color >> glossy >> reflective >> refractive >> matIOR;
		glossy = f2limits(glossy, 0, 1000);
		reflective = f2limits(reflective, 0., 1.);
		refractive = f2limits(refractive, 0., 1. - reflective);
		diffusion = f2limits(1. -  reflective - refractive, 0., 1.);
		matIOR = f2limits(matIOR, 0.1, 10.);
		matOIR = 1. / matIOR;
	}
	inline std::string output_material(void) const {
		std::ostringstream os;
		os << "   " << _color.rrggbb();
		os << " " << std::setw(4) << std::right << glossy;
		os << " " << std::setw(4) << std::right << reflective;
		os << " " << std::setw(4) << std::right << refractive;
		os << " " << std::setw(4) << std::right << matIOR;
		return os.str();
	}
	
	virtual A_Scenery* clone(void) const = 0;
	virtual int   get_iColor(const HitRecord& rec) const = 0;
	virtual void  lookat(const Position& eye, const LookatAux& aux, const Vec3f& pos, float roll) = 0;
	virtual void  roll(const Vec3f& pos, float shiftRoll) = 0;
	virtual bool  intersection(Ray& ray) = 0;
	virtual void  getNormal(Ray& ray) const = 0;
	virtual float lighting(Ray& ray) = 0;
	virtual bool  isGlowing(Ray& ray) const = 0;
	virtual void  photonEmissions(int num, phRays_t& rays) const = 0;
	virtual void  output(std::ostringstream& os) const = 0;
	friend std::ostream& operator<<(std::ostream& o, const A_Scenery& s);
};


struct Scenerys : public std::vector<A_Scenery*> {
	Scenerys(void) : std::vector<A_Scenery*>() {}
	~Scenerys(void) {}
	Scenerys& clear(int n) {
		Scenerys tmp;
		if (n) tmp.reserve(n);
		swap(tmp);
		return *this;
	}
};


#endif /* A_SCENERY_HPP */
