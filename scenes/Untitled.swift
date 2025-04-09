//
//  Untitled.swift
//  rt
//
//  Created by uru on 28/03/2025.
//

#include <iostream>
#include <cmath>
#include <array>

struct Vector3 {
	double x, y, z;

	Vector3 operator-(const Vector3& other) const {
		return {x - other.x, y - other.y, z - other.z};
	}

	Vector3 operator+(const Vector3& other) const {
		return {x + other.x, y + other.y, z + other.z};
	}

	Vector3 operator*(double scalar) const {
		return {x * scalar, y * scalar, z * scalar};
	}

	double dot(const Vector3& other) const {
		return x * other.x + y * other.y + z * other.z;
	}

	Vector3 cross(const Vector3& other) const {
		return {
			y * other.z - z * other.y,
			z * other.x - x * other.z,
			x * other.y - y * other.x
		};
	}
};

bool intersectRayRectangle(const Vector3& rayPov,
						   const Vector3& rayDir,
						   const Vector3& center,
						   const Vector3& normal,
						   double width,
						   double height,
						   Vector3& iPoint) {
	
	// Step 1: Compute the intersection of the ray with the plane of the rectangle
	double t = (normal * center - (normal * rayPov)) / (normal * rayDir);
	
	double d = rectNormal.dot(rectCenter);
		double t = (d - rectNormal.dot(rayOrigin)) / rectNormal.dot(rayDirection);


	if (t < 0) {
		return false; // The rectangle is behind the ray
	}

	iPoint = rayPov + rayDir * t;

	// Step 2: Check if the intersection point is inside the rectangle
	Vector3 localPoint = iPoint - rectCenter;

	// Get the rectangle's local axes
	Vector3 u = {1, 0, 0}; // Example: local X axis
	Vector3 v = {0, 1, 0}; // Example: local Y axis
	Vector3 w = rectNormal; // Normal vector

	// Project the point onto the rectangle's local axes
	double p_u = localPoint.dot(u);
	double p_v = localPoint.dot(v);

	// Check if the point is within the bounds of the rectangle
	return (std::abs(p_u) <= rectWidth / 2 && std::abs(p_v) <= rectHeight / 2);
}

int main() {
	Vector3 rayOrigin = {0, 0, 0};
	Vector3 rayDirection = {0, 0, -1}; // Must be a unit vector
	Vector3 rectCenter = {0, 0, -5};
	Vector3 rectNormal = {0, 0, 1}; // Must be a unit vector
	double rectWidth = 2.0;
	double rectHeight = 2.0;
	Vector3 intersectionPoint;

	if (intersectRayRectangle(rayOrigin, rayDirection, rectCenter, rectNormal, rectWidth, rectHeight, intersectionPoint)) {
		std::cout << "Intersection at: (" << intersectionPoint.x << ", " << intersectionPoint.y << ", " << intersectionPoint.z << ")\n";
	} else {
		std::cout << "No intersection.\n";
	}

	return 0;
}


#include <iostream>
#include <cstdlib>

using namespace std;

// Функция для перехода из первой системы координат в вторую
void firstToSecond(int w, int h, int x1, int y1, int &x2, int &y2) {
	x2 = x1 - w / 2;
	y2 = h / 2 - y1;
}

// Функция для перехода из второй системы координат в первую
void secondToFirst(int w, int h, int x2, int y2, int &x1, int &y1) {
	x1 = x2 + w / 2;
	y1 = h / 2 - y2;
}

// Функция для перехода из первой системы координат в "построчную"
void firstToRow(int w, int h, int x1, int y1, size_t &l) {
	l = y1 * w + x1;
}

// Функция для перехода из "построчной" системы в первую
void rowToFirst(int w, size_t l, int &x1, int &y1) {
	y1 = l / w;
	x1 = l % w;
}

// Функция для перехода из второй системы координат в "построчную"
void secondToRow(int w, int h, int x2, int y2, size_t &l) {
	int x1, y1;
	secondToFirst(w, h, x2, y2, x1, y1);
	firstToRow(w, h, x1, y1, l);
}

// Функция для перехода из "построчной" системы в вторую
void rowToSecond(int w, int h, size_t l, int &x2, int &y2) {
	int x1, y1;
	rowToFirst(w, l, x1, y1);
	// Переход из первой системы во вторую
	firstToSecond(w, h, x1, y1, x2, y2);
}

int main() {
	int w = 800; // Ширина экрана
	int h = 600; // Высота экрана

	// Пример использования функций
	int x1 = 100, y1 = 200;
	int x2, y2;
	size_t l;

	// Перевод из первой системы во вторую
	firstToSecond(w, h, x1, y1, x2, y2);
	cout << "First to Second: (" << x1 << ", " << y1 << ") -> (" << x2 << ", " << y2 << ")\n";

	// Перевод из первой системы в "построчную"
	firstToRow(w, h, x1, y1, l);
	cout << "First to Row: (" << x1 << ", " << y1 << ") -> " << l << "\n";

	// Перевод из второй системы в "построчную"
	secondToRow(w, h, x2, y2, l);
	cout << "Second to Row: (" << x2 << ", " << y2 << ") -> " << l << "\n";

	// Перевод из "построчной" системы в первую
	rowToFirst(w, l, x1, y1);
	cout << "Row to First: " << l << " -> (" << x1 << ", " << y1 << ")\n";

	// Перевод из "построчной" системы во вторую
	rowToSecond(w, h, l, x2, y2);
	cout << "Row to Second: " << l << " -> (" << x2 << ", " << y2 << ")\n";

	return 0;
}


#include <cstdlib>
#include <cmath>

struct Point {
	double x, y, z;
};

Point randomPointInRectangle(double w, double h, Point c, Point n) {
	// Генерируем случайные смещения по осям X и Y
	double halfWidth = w / 2.0;
	double halfHeight = h / 2.0;

	// Случайные смещения
	double randomX = ((double)rand() / RAND_MAX) * w - halfWidth;
	double randomY = ((double)rand() / RAND_MAX) * h - halfHeight;

	// Вычисляем координаты точки
	Point p;
	p.x = c.x + randomX * n.x;
	p.y = c.y + randomY * n.y;
	p.z = c.z + randomY * n.z; // Замените на нужное вам смещение по Z

	return p;
}

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>

struct Point {
	float x, y, z;
};

Point randomPointInRectangle(float w, float h, Point c, Point n) {
	// Генерируем случайные смещения в пределах ширины и высоты
	float halfW = w / 2.0f;
	float halfH = h / 2.0f;

	// Генерация случайных смещений
	float offsetX = ((float)rand() / RAND_MAX) * w - halfW;
	float offsetY = ((float)rand() / RAND_MAX) * h - halfH;

	// Вычисляем координаты случайной точки
	Point p;
	p.x = c.x + offsetX * (1 - n.x);
	p.y = c.y + offsetY * (1 - n.y);
	p.z = c.z + (n.z > 0 ? 0 : (float)rand() / RAND_MAX); // Учитываем нормаль

	return p;
}

int main() {
	srand(static_cast<unsigned int>(time(0))); // Инициализация генератора случайных чисел

	Point center = {0.0f, 0.0f, 0.0f};
	Point normal = {0.5f, 0.5f, 0.5f}; // Нормаль должна быть нормализирована

	float width = 10.0f;
	float height = 5.0f;

	Point randomPoint = randomPointInRectangle(width, height, center, normal);
	std::cout << "Random Point: (" << randomPoint.x << ", " << randomPoint.y << ", " << randomPoint.z << ")\n";

	return 0;
}


#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>

struct Point {
	double x, y, z;
};

Point randomPointInRectangle(const Point& c, const Point& u, const Point& v, double w, double h) {
	// Инициализация генератора случайных чисел
	std::srand(static_cast<unsigned int>(std::time(0)));

	// Генерация случайных координат в пределах прямоугольника
	double halfWidth = w / 2.0;
	double halfHeight = h / 2.0;

	// Случайные смещения от центра
	double randomX = (static_cast<double>(std::rand()) / RAND_MAX) * w - halfWidth;
	double randomY = (static_cast<double>(std::rand()) / RAND_MAX) * h - halfHeight;

	// Вычисляем координаты точки p
	Point p;
	p.x = c.x + randomX * u.x + randomY * v.x;
	p.y = c.y + randomX * u.y + randomY * v.y;
	p.z = c.z + randomX * u.z + randomY * v.z;

	return p;
}

int main() {
	Point c = {0.0, 0.0, 0.0}; // Центр прямоугольника
	Point u = {1.0, 0.0, 0.0}; // Направление горизонтали
	Point v = {0.0, 1.0, 0.0}; // Направление вертикали
	double w = 4.0; // Длина горизонтали
	double h = 2.0; // Длина вертикали

	Point randomPoint = randomPointInRectangle(c, u, v, w, h);
	std::cout << "Random Point: (" << randomPoint.x << ", " << randomPoint.y << ", " << randomPoint.z << ")\n";

	return 0;
}


Vec3f r;
r.substract(rayPov, point);
float k = rayDir * normal;
return rayPlaneIntersection(k, r, normal, distance, min_t, max_t, rayHit);
}

bool	rayPlaneIntersection(float k,
						 const Vec3f r,
						 const Vec3f& normal,
						 float& distance,
						 float& min_t,
						 float& max_t,
						 Hit& rayHit) {
if ( !almostEqual(k, 0, EPSILON) ) {
	min_t  = max_t = r * normal / -k;
	if (min_t > 0) {
		distance = min_t;
		rayHit = OUTSIDE;
		return true;
	}
	
	
	distance = -((rayPov - pos.p) * pos.n) / (pos.n * rayDir);// ray-plane intersection


	set_any("R	800 600  SystemDemo VOLUME  500000  60  0.1");
	img.init(header(), _resolution);
	cameras.push_back(Camera(img));
	set_any("A				0.2 0xFFFFEE");
//	set_any("lsc	0,0,0	0.4 0xFFFFAA	3,3,5		-1,-1,0	0	2.0");
//	set_any("ldr	1,-0.7,0	0.4 txtr:system2	-5,1.49,4	1,0,0	0	5.0	5.0");
//	set_any("ldr	-1,-0.7,0	0.25 txtr:system3	5,1.49,4	1,0,0	0	5.0	5.0");
	set_any("lsc	0,0,0	0.8 0xFFFFBB	0,5,4	0,-1,0	0	2.0");
//	set_any("lsr	0,0,0	0.5 0xFFFFFF	0,2,8		0,-0.1,-1	0	5.0	5.0");
//	set_any("ld		-2,-2,0	0.4 0xFFFFFF");
//	set_any("ls		1,2,4	0.4 0xFFFFFF");
//	set_any("c	0,1,-8	0,0,1		60");
//	set_any("c	0,1,16		0,0,-1		60");
//	set_any("c	-5.5,0,3	1,0,0		60");
	set_any("c 12,1,3		-1,0,0		60");
	set_any("c	0,9,4		0,-1,0		90");
	set_any("sp	0,-1,3		2		0xFF1A1A	100		0.1");
	set_any("sp	2,1,4		4		0xFFFFFF	500		0.0		1.0		1.5");
	set_any("sp	-2,0,4		2		0x1A3480	10");
	set_any("sp	0,-5001,0	10000	0xFFFFEE	0.01		0.3");
