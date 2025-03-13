//
//  Untitled.swift
//  rt
//
//  Created by uru on 11/03/2025.
//

bool Camera::tracePath(Ray& ray, int r, bool shader) {
	if ((ray.recursion = r) < depth) {
		if (ray.closestScenery(scenerys, _INFINITY)) {
			if (ray.isAlbedo())
				return true;
			ray.getNormal();
			if (!shader)
				ambientLighting(ray, ray);
			A_Scenery*	scnr(ray.scnr);
			int			attenuation(scnr->get_iColor(ray));
			int			color(ray.color.val); ray.color.val = 0;
			float		intensity = 1;
			double		chance = random_double();
			if (chance <= scnr->reflective) {	// reflections
				ray.dir.reflect(ray.norm);
				ray.path.set_reflection();
			} else if (chance <= scnr->reflective + scnr->refractive) {	// refractions
				float	eta = ray.hit == INSIDE ? scnr->matIOR : scnr->matOIR;
				float	cos_theta = -(ray.dir * ray.norm);
				float	schlick_ = shader ? 0 : schlick(cos_theta, eta);
				if (!ray.dir.refract_(ray.norm, cos_theta, eta))
					schlick_ = 1.0;
				chance = random_double();
				if (chance > _1_255 && chance < schlick_) {	// reflections
					ray.dir.reflect(ray.norm);
					ray.path.set_reflection();
				} else {									// refractions
					ray.path.set_refraction();
				}
			} else {										// diffusions
				ray.path.set_diffusion();
				ray.dir.randomInUnitHemisphere(ray.norm);
			}
			if (ray.path.isReflection()) {
				intensity = shader ? ray.dir * ray.norm : 1;
				ray.movePovByNormal(EPSILON);
				tracePath(ray, ++r);
				ray.color.product(intensity);
				ray.path.set_reflection();
			} else if (ray.path.isRefraction()) {
				intensity = shader ? -(ray.dir * ray.norm) : 1;
				ray.movePovByNormal(-EPSILON);
				tracePath(ray, ++r);
				ray.color.iProduct(attenuation).product(intensity);
//				ray.color.product(intensity);
				ray.path.set_refraction();
			} else if (ray.path.isDiffusion()) {
				intensity = ray.dir * ray.norm;
				ray.movePovByNormal(EPSILON);
				tracePath(ray, ++r);
				ray.color.iProduct(attenuation).product(intensity);
				ray.path.set_diffusion();
			}
			ray.color.iAddition(color);
			ray.scnr = scnr;
			return true;
		} else {
			ray.color = background.light;
			return false;
		}
	}
	ray.color.val = 0;
	return false;
	ray.phMapLightings(phMap.get_sqr(), phMap.estimate, rec.scnr->get_id(),
					   rec.scnr->get_iColor(ray));
}
}
}
