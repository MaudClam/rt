#ifndef PHOTONMAP_HPP
# define PHOTONMAP_HPP

# include <map>
# include <forward_list>
# include "Ray.hpp"
# include "A_Scenery.hpp"

struct	Ray;
struct	Rays;
class	A_Scenery;
struct	Scenerys;
struct	PhotonTrace;
typedef	Rays phRays_t;


typedef std::forward_list<PhotonTrace*> phTraces_t;

struct ClasterKey {
	MapType	type;
	int		x, y, z;
	ClasterKey(void);
	ClasterKey(MapType _type, int _x, int _y, int _z);
	ClasterKey(MapType type, const Vec3f point, float gridStep);
	ClasterKey(const PhotonTrace& trace, float gridStep);
	ClasterKey(const ClasterKey& other);
	~ClasterKey(void);
	ClasterKey& operator=(const ClasterKey& other);
	ClasterKey&	make(MapType _type, const Vec3f& point, float gridStep);
	ClasterKey&	make(const PhotonTrace& trace, float gridStep);
	friend bool operator==(const ClasterKey& left, const ClasterKey& right);
	friend bool operator!=(const ClasterKey& left, const ClasterKey& right);
	friend bool operator<(const ClasterKey& left, const ClasterKey& right);
	friend bool operator>(const ClasterKey& left, const ClasterKey& right);
	friend bool operator>=(const ClasterKey& left, const ClasterKey& right);
	friend bool operator<=(const ClasterKey& left, const ClasterKey& right);
	friend std::ostream& operator<<(std::ostream& o, const ClasterKey& key);
};


struct Claster {
	int			count;
	phTraces_t	traces;
	Claster(void);
	Claster(const Claster& other);
	~Claster(void);
	Claster& operator=(const Claster& other);
	inline void add_trace(PhotonTrace* trace) {
		traces.emplace_front(trace);
		count++;
	}
};


typedef std::map<ClasterKey,Claster>	clasters_t;


class PhotonMap : public clasters_t {
	int _sizeGlobal, _sizeCaustic, _sizeVolume, _looped;
public:
	int		totalPhotons;
	int		estimate;
	float	gridStep;
	Rgb		totalPow;
	MapType	type;
	PhotonMap(void);
	PhotonMap(const PhotonMap& other);
	~PhotonMap(void);
	PhotonMap& operator=(const PhotonMap& other);
private:
	int  get_size(MapType type) const;
	void swap_(PhotonMap& other);
	void clear_(void);
	void deleteTraces(void);
	void set_trace(PhotonTrace* trace);
	void photonPathsTracing_lll(Scenerys& scenerys, phRays_t& rays);
	bool tracePhotonPath(Scenerys& scenerys, Ray& ray, int r);
	inline void counter(MapType type) {
		switch (type) {
			case GLOBAL:	_sizeGlobal++; return;
			case CAUSTIC:	_sizeCaustic++; return;
			case VOLUME:	_sizeVolume++; return;
			default:		break;
		}
		_sizeGlobal = _sizeCaustic = _sizeVolume = 0;
	}
	inline void get_traces(const ClasterKey& key, phTraces_t& traces) const {
		auto search = find(key);
		if (search != end()) {
			traces.insert_after(traces.before_begin(), search->second.traces.begin(), search->second.traces.end());
		}
	}
public:
	inline float get_sqr(void) const { return gridStep * gridStep; }
	inline void get_traces(const Vec3f& point, phTraces_t& traces, MapType type) const {
		if (!traces.empty()) traces.clear();
		ClasterKey	key(type, point, gridStep);
		get_traces(key, traces);
	}
	inline void get_traces27(const Vec3f& point, phTraces_t& traces, MapType type) const {
		if (!traces.empty()) traces.clear();
		ClasterKey	key(type, point, gridStep);
		int x_begin = key.x - 1, x_end = key.x + 2;
		int y_begin = key.y - 1, y_end = key.y + 2;
		int z_begin = key.z - 1, z_end = key.z + 2;
		for (key.x = x_begin; key.x < x_end; key.x++)
			for (key.y = y_begin; key.y < y_end; key.y++)
				for (key.z = z_begin; key.z < z_end; key.z++) {
					get_traces(key, traces);
				}
	}
	void make(Scenerys& scenerys, Scenerys& lightsIdx);
	void lookat(const Position& eye, const LookatAux& aux, float roll);
	void outputPhotonMapParametrs(void);
};


#endif /* PHOTONMAP_HPP */
