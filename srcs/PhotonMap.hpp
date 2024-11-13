#ifndef PHOTONMAP_HPP
# define PHOTONMAP_HPP

# include <map>
# include <forward_list>
# include <random>
# include "PhotonTrace.hpp"
# include "A_Scenery.hpp"
# include "Ray.hpp"

struct	Ray;
class	A_Scenery;
typedef	std::vector<Ray>						photonRays_t;
typedef	std::vector<A_Scenery*>					a_scenerys_t;
typedef	std::mt19937							rand_gen_t;
typedef	std::uniform_real_distribution<float>	rand_distr_t;
typedef std::forward_list<PhotonTrace*>			traces_t;

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
};


struct Claster {
	int			count;
	traces_t	traces;
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
	rand_gen_t&	_gen;
	int			_sizeGlobal, _sizeCaustic, _sizeVolume, _totalPhotons;
	float		_gridStep;
	Power		_totalPow;
public:
	PhotonMap(rand_gen_t& gen);
	PhotonMap(const PhotonMap& other);
	~PhotonMap(void);
	PhotonMap& operator=(const PhotonMap& other);
private:
	void swap_(PhotonMap& other);
	void deleteTraces(void);
	void set_totalPow(a_scenerys_t& lightsIdx);
	inline void counter(MapType type) {
		switch (type) {
			case GLOBAL:	_sizeGlobal++; return;
			case CAUSTIC:	_sizeCaustic++; return;
			case VOLUME:	_sizeVolume++; return;
			default:		break;
		}
		_sizeGlobal = _sizeCaustic = _sizeVolume = 0;
	}
	void photonRayTracing_lll(a_scenerys_t& scenerys, photonRays_t& rays);
	void tracePhotonRay(rand_distr_t& distr, a_scenerys_t& scenerys, Ray& ray);

public:
	int  get_size(MapType type) const;
	void lookat(const Position& eye, const LookatAux& aux, float roll);
	void randomSampleHemisphere(int n, const Position& pos, const Power& pow, photonRays_t& rays, bool is_cosineDistr) const;
	void make(a_scenerys_t& scenerys, a_scenerys_t& lightsIdx);
	inline void set_trace(PhotonTrace* trace) {
		auto it_bool = try_emplace(ClasterKey().make(*trace, _gridStep), Claster());
		it_bool.first->second.add_trace(trace);
		counter(trace->type);
	}
	inline void set_newTrace(const Vec3f& point, const Vec3f& dir, const Power& pow, MapType type, A_Scenery* scenery) {
		PhotonTrace* trace = new PhotonTrace(type, point, dir, pow, scenery);
		set_trace(trace);
	}
	inline void get_traces(const ClasterKey& key, traces_t& traces) const {
		auto claster = find(key);
		if (claster != end()) {
			traces.insert_after(traces.before_begin(), claster->second.traces.begin(), claster->second.traces.end());
		}
	}
	inline void get_traces(const Vec3f& point, traces_t& traces, MapType type) const {
		traces.clear();
		ClasterKey	key(type, point, _gridStep);
		get_traces(key, traces);
	}
	inline void get_traces27(const Vec3f& point, traces_t& traces, MapType type) const {
		traces.clear();
		ClasterKey	key(type, point, _gridStep);
		int x_begin = key.x - 1, x_end = key.x + 2;
		int y_begin = key.y - 1, y_end = key.y + 2;
		int z_begin = key.z - 1, z_end = key.z + 2;
		for (key.x = x_begin; key.x < x_end; key.x++)
			for (key.y = y_begin; key.y < y_end; key.y++)
				for (key.z = z_begin; key.z < z_end; key.z++)
					get_traces(key, traces);
	}
	inline void getHemisphereRandomPhiTheta(const Vec3f& normal, float& phi, float& theta, bool is_cosineDistr) const {
		rand_distr_t distr(-1.0, 1.0);
		float phiNorm = 0., thetaNorm = 0.;
		normal.cartesian2sphericalDirection(phiNorm, thetaNorm);
		bool condition = true;
		while (condition) {
			phi = distr(_gen);
			theta = distr(_gen);
			if (is_cosineDistr) {
				phi = cosineDistr(phi);
				theta = cosineDistr(theta);
			}
			phi = loop(phiNorm + phi * M_PI_2, M_2PI);
			theta = reverse(thetaNorm + theta * M_PI_4, (float)M_PI);
			condition =	(almostEqual(theta, 0.) && !almostEqual(phi, 0.)) ||
						(almostEqual(theta, M_PI) && !almostEqual(phi, 0.));
		}
	}
	inline void getHemisphereRandomVec3f(const Vec3f& normal, Vec3f& dir, bool is_cosineDistr) const {
		float phi = 0., theta = 0.;
		getHemisphereRandomPhiTheta(normal, phi, theta, is_cosineDistr);
		dir.sphericalDirection2cartesian(phi, theta);
	}
};

#endif /* PHOTONMAP_HPP */
