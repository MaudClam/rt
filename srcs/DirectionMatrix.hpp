//
//  DirectionMatrix.hpp
//  rt
//
//  Created by uru on 18/10/2024.
//

#ifndef DIRECTIONMATRIX_HPP
# define DIRECTIONMATRIX_HPP

# include <set>
#include <random>
# include "geometry.hpp"
# include "Header.h"


struct	Dir;
struct	DirectionMatrix;
typedef DirectionMatrix		DirMatrix;
typedef std::set<Dir>		dirSet_t;
typedef std::vector<Ray>	photonRays_t;


struct Dir {
	int	phi;
	int	theta;
	Vec3f	v;
	Dir(void);
	Dir(int _phi, int _theta, int maxPhi, int maxTheta);
	Dir(const Vec3f& _v, int maxPhi, int maxTheta);
	Dir(const Dir& other);
	~Dir(void);
	Dir& operator=(const Dir& other);
	void get_degree(float& _phi, float& _theta) const ;
	void get_degree(float& _phi, float& _theta, int maxPhi, int maxTheta) const;
	std::string roundedOutputDegree(int factor = 2) const;
	std::string roundedOutputDegree(int maxPhi, int maxTheta, int factor = 2) const;
	friend std::ostream& operator<<(std::ostream& o, const Dir& d);
	friend bool operator==(const Dir& left, const Dir& right);
	friend bool operator!=(const Dir& left, const Dir& right);
	friend bool operator<(const Dir& left, const Dir& right);
	friend bool operator>(const Dir& left, const Dir& right);
	friend bool operator>=(const Dir& left, const Dir& right);
	friend bool operator<=(const Dir& left, const Dir& right);
};


struct DirectionMatrix : public dirSet_t {
	int maxPhi, maxTheta;
	DirectionMatrix(void);
	DirectionMatrix(float maxPhi, float maxTheta);
	~DirectionMatrix(void);
private:
	void make(void);
	inline	void makeOneRay(photonRays_t& s, const Position& pos, const Dir& key) const {
		if ((key.theta == 0 && key.phi != 0) || (key.theta == maxTheta && key.phi != 0))
			return;
		auto it = find(key);
		if (it != end()) {
			s.emplace_back();
			s.back().dir = it->v;
			s.back().pov = pos.p;
		}
	}
public:
	void	randomSample(photonRays_t& s, const Position& pos, int n) const;
	void	randomSampleHemisphere(photonRays_t& s, const Position& pos, int n) const;
	void	randomSampleHemisphereCosineDistribution(photonRays_t& s, const Position& pos, int n) const;
	friend std::ostream& operator<<(std::ostream& o, const DirectionMatrix& dM);
};

#endif /* DIRECTIONMATRIX_HPP */
