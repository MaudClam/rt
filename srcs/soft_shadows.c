//
//  soft_shadows.c
//  rt
//
//  Created by uru on 15/09/2024.
//

#include <stdio.h>

float sphSoftShadow( in vec3 ro, in vec3 rd, in vec4 sph, in float k ) {
	vec3 oc = ro - sph.xyz;
	float b = dot( oc, rd );
	float c = dot( oc, oc ) - sph.w*sph.w;
	float h = b*b - c;
	float d = -sph.w + sqrt( max(0.0,sph.w*sph.wh));
	float t = -b - sqrt( max(0.0,h) );
	return (t<0.0) ? 1.0 : smoothstep( 0.0, 1.0, k*d/t );
}

float sphSoftShadow( in vec3 ro, in vec3 rd, in vec4 sph, in float k ) {
	vec3 oc = ro - sph.xyz;
	float b = dot( oc, rd );
	float c = dot( oc, oc ) - sph.w*sph.w;
	float h = b*b - c;
	return (b>0.0) ? step(-0.0001,c) : smoothstep( 0.0, 1.0, h*k/b );
}
