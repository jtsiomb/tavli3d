#ifndef MESHGEN_H_
#define MESHGEN_H_

class Mesh;

void gen_sphere(Mesh *mesh, float rad, int usub, int vsub, float urange = 1.0, float vrange = 1.0);
void gen_cylinder(Mesh *mesh, float rad, float height, int usub, int vsub, int capsub = 0, float urange = 1.0, float vrange = 1.0);
void gen_cone(Mesh *mesh, float rad, float height, int usub, int vsub, int capsub = 0, float urange = 1.0, float vrange = 1.0);
void gen_plane(Mesh *mesh, float width, float height, int usub = 1, int vsub = 1);
void gen_heightmap(Mesh *mesh, float width, float height, int usub, int vsub, float (*hf)(float, float, void*), void *hfdata = 0);
void gen_box(Mesh *mesh, float xsz, float ysz, float zsz);

#endif	// MESHGEN_H_
