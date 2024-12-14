#ifndef PHOTONTRACE_HPP
# define PHOTONTRACE_HPP

# include <forward_list>
# include "Power.hpp"

class A_Scenery;


struct PhotonTrace {
	MapType		type;
	Position	pos;
	Power		pow;
	int			sceneryId;
	PhotonTrace(void);
	PhotonTrace(MapType _type, const Vec3f& point, const Vec3f& dir, const Power& _pow, int _sceneryId);
	PhotonTrace(const PhotonTrace& other);
	~PhotonTrace(void);
	PhotonTrace& operator=(const PhotonTrace& other);
	inline PhotonTrace* clone(void) const { return new PhotonTrace(*this); }
};


struct Traces : public std::forward_list<PhotonTrace*> {
	Traces(void) : std::forward_list<PhotonTrace*>() {}
	~Traces(void){}
	Traces& clear_(void) {
		if (!empty())
			clear();
		return *this;
	}
};


#endif /* PHOTONTRACE_HPP */
