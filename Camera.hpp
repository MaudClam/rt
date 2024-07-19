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
	float				fov;
	float				tan;
	Position			pos;
	float				roll;
	float				flybyRadius;
public:
	std::vector<Ray>	pixels;
	Camera(const MlxImage& img);
	~Camera(void);
	Camera(const MlxImage& img, const Position& pos, float fov);
	Camera(const Camera& other);
	Camera& operator=(const Camera& other);
	float		get_fov(void) const;
	Position	get_pos(void) const;
	float		get_roll(void) const;
	float		get_flybyRadius(void) const;
	bool		set_fov(float fov);
	void		set_pos(const Position& pos);
	void		set_roll(float roll);
	void		set_flybyRadius(float flybyRadius);
	void		initPixels(void);
	void		resetPixels(void);
	void		move(int ctrl);
	void		flyby(int ctrl);
	void		rotate(int ctrl);
	friend std::ostream& operator<<(std::ostream& o, Camera& camera);
	friend std::istringstream& operator>>(std::istringstream& is, Camera& camera);
};

// Non member functions

float fovToTan(float& fov);
float tanToFov(float tan);

#endif /* CAMERA_HPP */
