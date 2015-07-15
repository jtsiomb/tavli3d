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
#include "basis.h"
#include "vmath.h"

Basis::Basis()
{
	i = Vector3(1, 0, 0);
	j = Vector3(0, 1, 0);
	k = Vector3(0, 0, 1);
}

Basis::Basis(const Vector3 &i, const Vector3 &j, const Vector3 &k)
{
	this->i = i;
	this->j = j;
	this->k = k;
}

Basis::Basis(const Vector3 &dir, bool left_handed)
{
	k = dir;
	j = Vector3(0, 1, 0);
	i = cross_product(j, k);
	j = cross_product(k, i);
}

/** Rotate with euler angles */
void Basis::rotate(scalar_t x, scalar_t y, scalar_t z) {
	Matrix4x4 RotMat;
	RotMat.set_rotation(Vector3(x, y, z));
	i.transform(RotMat);
	j.transform(RotMat);
	k.transform(RotMat);
}

/** Rotate by axis-angle specification */
void Basis::rotate(const Vector3 &axis, scalar_t angle) {
	Quaternion q;
	q.set_rotation(axis, angle);
	i.transform(q);
	j.transform(q);
	k.transform(q);
}

/** Rotate with a 4x4 matrix */
void Basis::rotate(const Matrix4x4 &mat) {
	i.transform(mat);
	j.transform(mat);
	k.transform(mat);
}

/** Rotate with a quaternion */
void Basis::rotate(const Quaternion &quat) {
	i.transform(quat);
	j.transform(quat);
	k.transform(quat);
}

/** Extract a rotation matrix from the orthogonal basis */
Matrix3x3 Basis::create_rotation_matrix() const {
	return Matrix3x3(	i.x, j.x, k.x,
						i.y, j.y, k.y,
						i.z, j.z, k.z);
}
