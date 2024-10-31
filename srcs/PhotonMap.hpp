//
//  PhotonMap.hpp
//  rt
//
//  Created by uru on 12/10/2024.
//

#ifndef PHOTONMAP_HPP
# define PHOTONMAP_HPP

# include <map>
# include <forward_list>
# include "Power.hpp"


enum MapType {ALL, GLOBAL, CAUSTIC, VOLUME, RESET};


struct PhotonTrace {
	MapType		type;
	Position	pos;
	Power		pow;
	PhotonTrace(void);
	PhotonTrace(MapType _type, const Vec3f& point, const Vec3f& dir, const Power& _pow);
	PhotonTrace(const PhotonTrace& other);
	~PhotonTrace(void);
	PhotonTrace& operator=(const PhotonTrace& other);
	inline PhotonTrace* clone(void) const { return new PhotonTrace(*this); }
};


struct ClasterKey {
	MapType			type;
	int				x, y, z;
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


typedef std::forward_list<PhotonTrace*>	traces_t;
typedef std::map<ClasterKey,traces_t>	clasters_t;


class PhotonMap : public clasters_t {
	int		_sizeGlobal, _sizeCaustic, _sizeVolume;
	float	_gridStep;
public:
	PhotonMap(void);
	PhotonMap(const PhotonMap& other);
	~PhotonMap(void);
	PhotonMap& operator=(const PhotonMap& other);
private:
	void swap_(PhotonMap& other);
	inline void counter(MapType type) {
		switch (type) {
			case GLOBAL:	_sizeGlobal++; return;
			case CAUSTIC:	_sizeCaustic++; return;
			case VOLUME:	_sizeVolume++; return;
			default:		break;
		}
		_sizeGlobal = _sizeCaustic = _sizeVolume = 0;
	}
	inline void get_traces(const ClasterKey& key, traces_t& traces) const {
		auto claster = find(key);
		if (claster != end()) {
			traces.insert_after(traces.before_begin(), claster->second.begin(), claster->second.end());
		}
	}
	void deleteTraces(void);
	inline void set_trace(PhotonTrace* trace) {
		auto it_bool = try_emplace(ClasterKey().make(*trace, _gridStep), traces_t());
		it_bool.first->second.emplace_front(trace);
		counter(trace->type);
	}
public:
	inline void set_newTrace(const Vec3f& point, const Vec3f& dir, const Power& pow, MapType type) {
		PhotonTrace* trace = new PhotonTrace(type, point, dir, pow);
		set_trace(trace);
	}
	inline void get_traces(const Vec3f& point, traces_t& traces, MapType type) const {
		ClasterKey	key(type, point, _gridStep);
		get_traces(key, traces);
	}
	inline void get_traces27(const Vec3f& point, traces_t& traces, MapType type) const {
		ClasterKey	key(type, point, _gridStep);
		int x_begin = key.x - 1, x_end = key.x + 2;
		int y_begin = key.y - 1, y_end = key.y + 2;
		int z_begin = key.z - 1, z_end = key.z + 2;
		for (key.x = x_begin; key.x < x_end; key.x++)
			for (key.y = y_begin; key.y < y_end; key.y++)
				for (key.z = z_begin; key.z < z_end; key.z++)
					get_traces(key, traces);
	}
	int  get_size(MapType type) const;
	void lookat(const Position& eye, const LookatAux& aux, float roll);
};

#endif /* PHOTONMAP_HPP */
