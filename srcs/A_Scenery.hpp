#ifndef ASCENERY_HPP
# define ASCENERY_HPP

# include "Ray.hpp"
# include "PhotonMap.hpp"


struct	Ray;
struct	Rays;
struct	HitRecord;
class	PhotonMap;
typedef	Rays			photonRays_t;
typedef	std::mt19937	rand_gen_t;


class A_Scenery {
protected:
	int				_id;
	std::string		_name;
	std::string		_nick;
	bool			_isLight;
	Position		_pos;
	ARGBColor		_color;		// surface color
public:
	CombineType		combineType;// combination type with the following primitive
	Lighting 		light;		// light for light source
	int				specular;	// in range [-1,1000]
	float			reflective;	// in range [0,1]
	float			refractive; // in range [0,1]
	float			diffusion;	// in range [0,1]
	float			matIOR;		// refractive in/out index relative to air
	float			matOIR;		// refractive out/in index relative to air
	A_Scenery(void);
	virtual ~A_Scenery(void);
	A_Scenery(const A_Scenery& other);
	
	inline int			get_id(void) const { return _id; }
	inline std::string	get_name(void) const { return std::string(_name); }
	inline std::string	get_nick(void) const { return std::string(_nick); }
	inline bool			get_isLight(void) const { return _isLight; }
	inline Position		get_pos(void) const { return Position(_pos); }
	ARGBColor	get_color(void) const;
	inline void	set_id(int id) { _id = id; }
	inline void	set_name(const std::string& name) { _name = name; }
	inline void	set_nick(const std::string& nick) { _nick = nick; }
	inline void	set_isLight(bool isLight) { _isLight = isLight; }
	inline void	set_pos(const Position& pos) { _pos = pos; }
	inline void	set_color(const ARGBColor& color) { _color = color; }
	
	virtual int get_iColor(const HitRecord& rec) const = 0;
	virtual A_Scenery* clone(void) const = 0;
	virtual void lookat(const Position& eye, const LookatAux& aux, const Vec3f& pos, float roll) = 0;
	virtual void roll(const Vec3f& pos, float shiftRoll) = 0;
	virtual bool intersection(Ray& ray) const = 0;
	virtual void getNormal(Ray& ray) const = 0;
	virtual float lighting(Ray& ray) const = 0;
	virtual void photonEmissions(int num, const PhotonMap& phMap, photonRays_t& rays) const = 0;
	virtual void output(std::ostringstream& os) const = 0;
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
