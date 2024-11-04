//
//  DirMatrix.hpp
//  rt
//
//  Created by uru on 18/10/2024.
//

#ifndef DIRMATRIX_HPP
# define DIRMATRIX_HPP

# include <set>
# include <random>
# include "Ray.hpp"

struct	Ray;
typedef	std::vector<Ray>						photonRays_t;
typedef	std::mt19937							rand_gen_t;
typedef	std::uniform_real_distribution<float>	rand_distr_t;

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


typedef std::set<Dir> dirSet_t;

struct DirMatrix : public dirSet_t {
	int maxPhi, maxTheta;
	rand_gen_t& rand_gen;
	DirMatrix(float maxPhi, float maxTheta, rand_gen_t& gen);
	~DirMatrix(void);
	DirMatrix& operator=(const DirMatrix& other);
private:
	void make(void);
public:
	inline bool findVec3f(const Dir& key, Vec3f& dir) const {
		if ( !(key.theta == 0 && key.phi != 0) || (key.theta == maxTheta && key.phi != 0) ) {
			auto it = find(key);
			if (it != end()) {
				dir = it->v;
				return true;
			}
		}
		return false;
	}
	inline void getRandomDirHemisphereCosineDistr(const Vec3f& normal, Vec3f& dir) const {
		rand_distr_t distr(-1.0, 1.0);
		Dir norm(normal, maxPhi, maxTheta);
		Dir key;
		int maxPhi_4 = maxPhi / 4, maxTheta_4 = maxTheta / 4;
		while (true) {
			float phi = distr(rand_gen);
			int sign = phi < 0 ? -1 : 1;
			key.phi = loop((int)(norm.phi + sign * cosineDistr(phi) * maxPhi_4), maxPhi);
			float theta = distr(rand_gen);
			sign = theta < 0 ? -1 : 1;
			key.theta = reverse((int)(norm.theta + sign * cosineDistr(theta) * maxTheta_4), maxTheta);
			if (findVec3f(key, dir))
				break;
		}
	}
	void randomSample(int n, const Position& pos, const Power& pow, photonRays_t& rays) const;
	void randSampleHemisphere(int n, const Position& pos, const Power& pow, photonRays_t& rays) const;
	void randSampleHemisphereCosineDistr(int n, const Position& pos, const Power& pow, photonRays_t& rays) const;
	void randOneHemisphereCosineDistr(int n, const Position& pos, const Power& pow, photonRays_t& rays) const;
	friend std::ostream& operator<<(std::ostream& o, const DirMatrix& dM);
};

#endif /* DIRMATRIX_HPP */
