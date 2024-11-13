#ifndef PHOTONTRACE_HPP
# define PHOTONTRACE_HPP

# include <forward_list>
# include "Power.hpp"
//# include "A_Scenery.hpp"

class A_Scenery;


struct PhotonTrace {
	MapType		type;
	Position	pos;
	Power		pow;
	A_Scenery*	scenery;
	PhotonTrace(void);
	PhotonTrace(MapType _type, const Vec3f& point, const Vec3f& dir, const Power& _pow, A_Scenery* _scenery);
	PhotonTrace(const PhotonTrace& other);
	~PhotonTrace(void);
	PhotonTrace& operator=(const PhotonTrace& other);
	inline PhotonTrace* clone(void) const { return new PhotonTrace(*this); }
};

typedef	std::forward_list<PhotonTrace*>	traces_t;


#endif /* PHOTONTRACE_HPP */
