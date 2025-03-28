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
	Vector3 localPoint = intersectionPoint - rectCenter;

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
