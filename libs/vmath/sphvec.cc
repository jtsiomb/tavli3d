/*
libvmath - a vector math library
Copyright (C) 2004-2015 John Tsiombikas <nuclear@member.fsf.org>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published
by the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "sphvec.h"
#include "vector.h"

/* theta: 0 <= theta <= 2pi, the angle around Y axis.
 * phi: 0 <= phi <= pi, the angle from Y axis.
 * r: radius.
 */
SphVector::SphVector(scalar_t theta, scalar_t phi, scalar_t r) {
	this->theta = theta;
	this->phi = phi;
	this->r = r;
}

/* Constructs a spherical coordinate vector from a cartesian vector */
SphVector::SphVector(const Vector3 &cvec) {
	*this = cvec;
}

/* Assignment operator that converts cartesian to spherical coords */
SphVector &SphVector::operator =(const Vector3 &cvec) {
	r = cvec.length();
	//theta = atan2(cvec.y, cvec.x);
	theta = atan2(cvec.z, cvec.x);
	//phi = acos(cvec.z / r);
	phi = acos(cvec.y / r);
	return *this;
}
