//
//  DirMatrix.cpp
//  rt
//
//  Created by uru on 22/10/2024.
//

#include "DirMatrix.hpp"


// struct DirPair

Dir::Dir(void) : phi(0), theta(0), v() {}

Dir::Dir(int _phi, int _theta, int maxPhi, int maxTheta) : phi(_phi), theta(_theta), v() {
	phi = loop(phi, maxPhi);
	theta = reverse(theta, maxTheta);
	v.sphericalDirection2cartesian(M_2PI / maxPhi * phi, M_PI / maxTheta * theta);
}

Dir::Dir(const Vec3f& _v, int maxPhi, int maxTheta) : phi(0), theta(0), v(_v)  {
	float _phi = 0., _theta = 0.;
	v.cartesian2sphericalDirection(_phi, _theta);
	phi = loop((int)std::round(_phi / M_2PI * maxPhi), maxPhi);
	theta = reverse((int)std::round(_theta / M_PI * maxTheta), maxTheta);
}

Dir::Dir(const Dir& other) : phi(other.phi), theta(other.theta), v(other.v) {}

Dir::~Dir(void) {}

Dir& Dir::operator=(const Dir& other) {
	if (this != &other) {
		phi = other.phi;
		theta = other.theta;
		v = other.v;
	}
	return *this;
}

void Dir::get_degree(float& _phi, float& _theta) const {
	v.cartesian2sphericalDirection(_phi, _theta);
	_phi *= M_180_PI;
	_theta *= M_180_PI;
	_phi = loop(_phi, 360.);
	_theta = reverse(_theta, 180.);
}

void Dir::get_degree(float& _phi, float& _theta, int maxPhi, int maxTheta) const {
	_phi = 360. / maxPhi * phi;
	_theta = 180. / maxTheta * theta;
}

std::string Dir::roundedOutputDegree(int factor) const {
	std::ostringstream o;
	float _phi = 0., _theta = 0.;
	get_degree(_phi, _theta);
	o	<< "{"
		<< std::setw(factor + 4) << std::right << std::setfill(' ')
		<< roundedString(_phi, factor) << "º,"
		<< std::setw(factor + 4) << std::right << std::setfill(' ')
		<< roundedString(_theta, factor) << "º}";
	return o.str();
}

std::string Dir::roundedOutputDegree(int maxPhi, int maxTheta, int factor) const {
	std::ostringstream o;
	float _phi = 0., _theta = 0.;
	get_degree(_phi, _theta, maxPhi, maxTheta);
	o	<< "{"
		<< std::setw(factor + 4) << std::right << std::setfill(' ')
		<< roundedString(_phi, factor) << "º,"
		<< std::setw(factor + 4) << std::right << std::setfill(' ')
		<< roundedString(_theta, factor) << "º}";
	return o.str();
}

std::ostream& operator<<(std::ostream& o, const Dir& d) {
	std::ostringstream os;
	os	<< d.phi << ",";
	o	<< std::setw(6) << std::right << std::setfill(' ') << os.str()
		<< std::setw(5) << std::left << std::setfill(' ') << d.theta;
	return o;
}

bool operator==(const Dir& left, const Dir& right) {
	return left.phi == right.phi && left.theta == right.theta;
}

bool operator!=(const Dir& left, const Dir& right) {
	return !(left == right);
}

bool operator<(const Dir& left, const Dir& right) {
	return left.phi < right.phi || (left.phi == right.phi && left.theta < right.theta);
}

bool operator>(const Dir& left, const Dir& right) {
	return left != right && !(left < right);
}

bool operator>=(const Dir& left, const Dir& right) {
	return !(left < right);
}

bool operator<=(const Dir& left, const Dir& right) {
	return !(left > right);
}


// struct DirMatrix

DirMatrix::DirMatrix(void) : maxPhi(0), maxTheta(0) {}

DirMatrix::DirMatrix(float _maxPhi, float _maxTheta) :
maxPhi(_maxPhi), maxTheta(_maxTheta) { make(); }

DirMatrix::~DirMatrix(void) {}

DirMatrix& DirMatrix::operator=(const DirMatrix& other) {
	if (this != &other) {
		maxPhi = other.maxPhi;
		maxTheta = other.maxTheta;
		make();
	}
	return *this;
}

void DirMatrix::make(void) {
	clear();
	emplace(0, 0, maxPhi, maxTheta);
	emplace(0, maxTheta, maxPhi, maxTheta);
	for (int phi = 0; phi < maxPhi; phi++)
		for (int theta = 1; theta < maxTheta; theta++)
			emplace(phi, theta, maxPhi, maxTheta);
}

void	DirMatrix::randomSample(photonRays_t& s, const Position& pos, int n) const {
	Dir key;
	std::random_device rd;
	std::mt19937 generator(rd());
	std::uniform_real_distribution<float> distr(0.90, 1.0);
	while (s.size() < (size_t)n) {
		key.phi = distr(generator) * maxPhi;
		key.theta = distr(generator) * maxTheta;
		makeOneRay(s, pos, key);
	}
}

void	DirMatrix::randomSampleHemisphere(photonRays_t& s, const Position& pos, int n) const {
	int maxPhi_4 = maxPhi / 4, maxTheta_4 = maxTheta / 4;
	Dir norm(pos.n, maxPhi, maxTheta);
	Dir key;
	std::random_device rd;
	std::mt19937 generator(rd());
	std::uniform_real_distribution<float> distr(0.0, 1.0);
	while (s.size() < (size_t)n / 2) {
		key.phi = loop((int)(norm.phi + distr(generator) * maxPhi_4), maxPhi);
		key.theta = reverse((int)(norm.theta + distr(generator) * maxTheta_4), maxTheta);
		makeOneRay(s, pos, key);
	}
	while (s.size() < (size_t)n) {
		key.phi = loop((int)(norm.phi - distr(generator) * maxPhi_4), maxPhi);
		key.theta = reverse((int)(norm.theta - distr(generator) * maxTheta_4), maxTheta);
		makeOneRay(s, pos, key);
	}
}

void	DirMatrix::randomSampleHemisphereCosineDistribution(photonRays_t& s, const Position& pos, int n) const {
	int maxPhi_4 = maxPhi / 4, maxTheta_4 = maxTheta / 4;
	Dir norm(pos.n, maxPhi, maxTheta);
	Dir key;
	std::random_device rd;
	std::mt19937 generator(rd());
	std::uniform_real_distribution<float> distr(0.0, 1.0);
	while (s.size() < (size_t)n / 2) {
		key.phi = loop((int)(norm.phi + cosineDistribution(distr(generator)) * maxPhi_4), maxPhi);
		key.theta = reverse((int)(norm.theta - cosineDistribution(distr(generator)) * maxTheta_4), maxTheta);
		makeOneRay(s, pos, key);
	}
	while (s.size() < (size_t)n) {
		key.phi = loop((int)(norm.phi - cosineDistribution(distr(generator)) * maxPhi_4), maxPhi);
		key.theta = reverse((int)(norm.theta + cosineDistribution(distr(generator)) * maxTheta_4), maxTheta);
		makeOneRay(s, pos, key);
	}
}

std::ostream& operator<<(std::ostream& o, const DirMatrix& dM) {
	int i = 0;
	for (auto it = dM.begin(), end = dM.end(); it != end; ++it, i++) {
		o << *it << " ";
		o << it->roundedOutputDegree() << " ";
		o << it->roundedOutputDegree(dM.maxPhi, dM.maxTheta) << " ";
		o << it->v.roundedOutput() << " ";
//		o << it->v;
		o << std::endl;
	}
	return o;
}
