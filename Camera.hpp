//
//  Camera.hpp
//  rt
//
//  Created by uru on 13/07/2024.
//

#ifndef CAMERA_HPP
# define CAMERA_HPP

# include "Header.h"

class Camera {
protected:
	std::string			name = "camera";
	std::string			nick = "c";
	char*				data;
	int					width;
	int					height;
	int					bytespp;
	float				mult;
	float				tan;
	Position			pos;
public:
	std::vector<Ray>	pixels;
	Camera(const MlxImage& img);
	~Camera(void);
	Camera(const MlxImage& img, const Position& pos, float fov = 60);
	Camera(const Camera& other);
	Camera& operator=(const Camera& other);
	Position get_pos(void) const;
	void initPixels(void);
	void resetPixels(void);
	friend std::ostream& operator<<(std::ostream& o, Camera& c);
	friend std::istringstream& operator>>(std::istringstream& is, Camera& c);
};

// Non member functions

float fovToTan(float fov);
float tanToFov(float tan);

#endif /* CAMERA_HPP */
