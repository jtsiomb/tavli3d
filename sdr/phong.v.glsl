varying vec3 vdir, ldir, normal;

void main()
{
	gl_Position = ftransform();

	vec3 vpos = (gl_ModelViewMatrix * gl_Vertex).xyz;
	normal = gl_NormalMatrix * gl_Normal;
	vdir = -vpos;
	ldir = gl_LightSource[0].position.xyz - vpos;
	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
}
