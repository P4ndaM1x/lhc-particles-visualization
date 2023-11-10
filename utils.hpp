#pragma once

namespace utils{

	struct Spherical{
		float distance, theta, phi;

		float getX(){
			return distance * cos(phi) * cos(theta);
		}
		float getY(){
			return distance * sin(phi);
		}
		float getZ(){
			return distance * cos(phi) * sin(theta);
		}
	};

	struct ParticleProperties{
		float phi, pt, q;
	};

}