varying vec3 vdir, ldir, normal;
varying vec4 shadow_tc;

void main()
{
	gl_Position = ftransform();

	vec3 vpos = (gl_ModelViewMatrix * gl_Vertex).xyz;
	normal = gl_NormalMatrix * gl_Normal;
	vdir = -vpos;
	ldir = gl_LightSource[0].position.xyz - vpos;
	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;

	mat4 offmat = mat4(0.5, 0.0, 0.0, 0.0,
			0.0, 0.5, 0.0, 0.0,
			0.0, 0.0, 0.5, 0.0,
			0.5, 0.5, 0.5, 1.0);
	mat4 tex_matrix = offmat * gl_TextureMatrix[1];

	shadow_tc = tex_matrix * vec4(vpos, 1.0);
}
