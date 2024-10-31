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
typedef	std::vector<Ray>	photonRays_t;

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
	DirMatrix(void);
	DirMatrix(float maxPhi, float maxTheta);
	~DirMatrix(void);
	DirMatrix& operator=(const DirMatrix& other);
private:
	void make(void);
	void makeOneRay(photonRays_t& s, const Position& pos, const Dir& key) const;
public:
	void	randomSample(int n, const Position& pos, photonRays_t& rays) const;
	void	randomSampleHemisphere(int n, const Position& pos, photonRays_t& rays) const;
	void	randomSampleHemisphereCosineDistribution(int n, const Position& pos, photonRays_t& rays) const;
	friend std::ostream& operator<<(std::ostream& o, const DirMatrix& dM);
};

#endif /* DIRMATRIX_HPP */
