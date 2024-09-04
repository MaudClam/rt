//
//  Ray.cpp
//  rt
//
//  Created by uru on 01/08/2024.
//

#include "Ray.hpp"

// struct Ray

Ray::Ray(void) :
recursion(0),
pov(),
dir(),
dirFromCam(),
dirToLight(),
norm(),
dist(0),
d1(0),
d2(0),
light(),
shine(),
color(),
hit(FRONT)
{}

Ray::~Ray(void) {}

Ray::Ray(const Ray& other) :
recursion(other.recursion),
pov(other.pov),
dir(other.dir),
dirFromCam(other.dirFromCam),
dirToLight(other.dirToLight),
norm(other.norm),
dist(other.dist),
d1(other.d1),
d2(other.d2),
light(other.light),
shine(other.shine),
color(other.color),
hit(other.hit)
{}

Ray& Ray::operator=(const Ray& other) {
	if (this != & other) {
		recursion = other.recursion;
		pov = other.pov;
		dir = other.dir;
		dirFromCam = other.dirFromCam;
		dirToLight = other.dirToLight;
		norm = other.norm;
		dist = other.dist;
		d1 = other.d1;
		d2 = other.d2;
		light = other.light;
		shine = other.shine;
		color = other.color;
		hit = other.hit;
	}
	return *this;
}

Ray& Ray::set_hit(Hit hit) {
	this->hit = hit;
	return *this;
}

Ray& Ray::changePov(void) {
	pov.addition( pov, dir * dist );
	if (!recursion) {
		dirFromCam = dir;
	}
	return *this;
}

Ray& Ray::partRestore(const Ray& other) {
//	recursion = other.recursion;
	pov = other.pov;
	dir = other.dir;
	dirFromCam = other.dirFromCam;
	dirToLight = other.dirToLight;
	norm = other.norm;
	dist = other.dist;
	light = other.light;
//	shine = other.shine;
//	color = other.color;
//	hit = other.hit;
	return *this;
}

Ray& Ray::movePovByNormal(float distance) {
	pov.addition(pov, norm * distance);
	return *this;
}

Ray& Ray::collectLight(const ARGBColor& sceneryColor, float k) {
	color.addition(color, light * sceneryColor * k);
	return *this;
}

Ray& Ray::collectShine(int specular) {
	if (specular != -1) {
		float k = dirToLight.reflect(norm) * dirFromCam;
		if (k > 0) {
			k = std::pow(k, specular);
			shine.addition(shine, light * k);
		}
	}
	return *this;
}

Ray& Ray::collectReflectiveLight(int _color, int _shine, float reflective) {
	float previous = 1. - reflective;
	light.val = _color;
	color.addition(color.product(reflective), light.product(previous));
	light.val = _shine;
	shine.addition(shine.product(reflective), light.product(previous));
	return *this;
}

Ray& Ray::collectRefractiveLight(const ARGBColor& sceneryColor, int _color, float refractive) {
	float previous = 1. - refractive;
	light.val = _color;
	color.addition(color.product(color,sceneryColor).product(refractive), light.product(previous));
	return *this;
}


// struct Combine

Combine::Combine(Ray& ray, A_Scenery* scenery, Hit hit) :
ray(ray),
hit(hit),
t1(scenery->combineType),
t2(END),
segments1(),
del(),
segment2()
{
	if ( scenery->intersection(ray.set_hit(hit)) ) {
		segments1.emplace_front(ray.d1, false, scenery,
							   ray.d2, ray.d1 == ray.d2 ? false : true, scenery);
	}
}

Combine::~Combine(void) {}

Combine::Combine(const Combine& other) :
ray(other.ray),
hit(other.hit),
t1(other.t1),
t2(other.t2),
segments1(other.segments1),
del(other.del),
segment2(other.segment2)
{}

Combine& Combine::operator=(const Combine& other) {
	ray = other.ray;
	hit = other.hit;
	t1 = other.t1;
	t2 = other.t2;
	segments1 = other.segments1;
	del = other.del;
	segment2 = other.segment2;
	return *this;
}

void Combine::emplace(const Segment& segment) {
	segments1.emplace_front(segment.a.d, segment.a.inside, segment.a.s,
							segment.b.d, segment.b.inside, segment.b.s );
}

void Combine::emplace(const Point& a, const Point& b) {
	segments1.emplace_front(a.d, a.inside, a.s, b.d, b.inside, b.s );
}

void Combine::next(A_Scenery* scenery) {
	t2 = scenery->combineType;
	if ( scenery->intersection(ray.set_hit(hit)) ) {
		segment2.a.d = ray.d1;
		segment2.b.d = ray.d2;
		segment2.a.inside = false;
		segment2.b.inside = ray.d1 == ray.d2 ? false : true;
		segment2.a.s = segment2.b.s = scenery;
	} else {
		segment2.a.s = segment2.b.s = NULL;
	}
	if (t1 == UNION) {
		if (first() && second()) {
			combination(t1);
		} else if (!first() && !second()) {
			noVisible();
		} else if (!first() && second()) {
			secondVisible();
		} else if (first() && !second()) {
			firstVisible();
		}
	} else if (t1 == SUBTRACTION) {
		if (first() && second()) {
			combination(t1);
		} else if (!first() && !second()) {
			noVisible();
		} else if (!first() && second()) {
			noVisible();
		} else if (first() && !second()) {
			firstVisible();
		}
	} else if (t1 == INTERSECTION) {
		if (first() && second()) {
			combination(t1);
		} else {
			noVisible();
		}
	}
	t1 = t2;
}

void Combine::combination(CombineType type) {
	for (auto segment1 = segments1.begin(), end = segments1.end(); segment1 != end; ++segment1) {
		bool _del = false;
		if (type == UNION) {
			_del = union_(*segment1, segment2);
		} else if (type == SUBTRACTION) {
			_del = subtraction(*segment1, segment2);
		} else if (type == INTERSECTION) {
			_del = intersection(*segment1, segment2);
		}
		if (_del) {
			del.push_back(segment1);
		}
	}
	if (type == UNION) {
		emplace(segment2);
	}
	for (auto it = del.begin(), end = del.end(); it != end; ++it) {
		segments1.erase(*it);
	}
	del.clear();
}

bool Combine::union_(Segment& segment1, Segment& segment2) {
	if (ab1_intersect_ab2(segment1.a.d, segment1.b.d, segment2.a.d, segment2.b.d)) {
		segment2.a = segment1.a;
		return true;
	} else if (ab2_intersect_ab1(segment1.a.d, segment1.b.d, segment2.a.d, segment2.b.d)) {
		segment2.b = segment1.b;
		return true;
	} else if (ab1_inside_ab2(segment1.a.d, segment1.b.d, segment2.a.d, segment2.b.d)) {
		return true;
	} else if (ab2_inside_ab1(segment1.a.d, segment1.b.d, segment2.a.d, segment2.b.d)) {
		segment2.a = segment1.a;
		segment2.b = segment1.b;
		return true;
	} else if (equal(segment1.a.d, segment1.b.d, segment2.a.d, segment2.b.d)) {
		return true;
	}
	return false;
}

bool Combine::subtraction(Segment& segment1, Segment& segment2) {
	if (ab1_intersect_ab2(segment1.a.d, segment1.b.d, segment2.a.d, segment2.b.d)) {
		segment1.b = segment2.a;
	} else if (ab2_intersect_ab1(segment1.a.d, segment1.b.d, segment2.a.d, segment2.b.d)) {
		segment1.a = segment2.b;
	} else if (ab2_inside_ab1(segment1.a.d, segment1.b.d, segment2.a.d, segment2.b.d)) {
		if (segment1.a.d != segment2.a.d) {
			emplace(segment1.a, segment2.a);
		}
		if (segment1.b.d != segment2.b.d) {
			emplace(segment2.b, segment1.b);
		}
		return true;
	} else if (ab1_inside_ab2(segment1.a.d, segment1.b.d, segment2.a.d, segment2.b.d)) {
		return true;
	}
	return false;
}

bool Combine::intersection(Segment& segment1, Segment& segment2) {
	if (ab1_intersect_ab2(segment1.a.d, segment1.b.d, segment2.a.d, segment2.b.d)) {
		segment1.a = segment2.a;
		return false;
	} else if (ab2_intersect_ab1(segment1.a.d, segment1.b.d, segment2.a.d, segment2.b.d)) {
		segment1.b = segment2.b;
		return false;
	} else if (ab1_inside_ab2(segment1.a.d, segment1.b.d, segment2.a.d, segment2.b.d)) {
		return false;
	} else if (ab2_inside_ab1(segment1.a.d, segment1.b.d, segment2.a.d, segment2.b.d)) {
		segment1 = segment2;
		return false;
	}
	return true;
}
