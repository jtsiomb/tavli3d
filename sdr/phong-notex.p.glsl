/* vi: set ft=glsl */
varying vec3 vdir, ldir, normal;

#define KD gl_FrontMaterial.diffuse.rgb
#define KS gl_FrontMaterial.specular.rgb
#define SPOW gl_FrontMaterial.shininess

void main()
{
	vec3 n = normalize(normal);
	vec3 v = normalize(vdir);
	vec3 l = normalize(ldir);
	vec3 h = normalize(l + v);

	float ndotl = max(dot(n, l), 0.0);
	float ndoth = max(dot(n, h), 0.0);

	vec3 diffuse = KD * gl_LightSource[0].diffuse.rgb * ndotl;
	vec3 specular = KS * gl_LightSource[0].specular.rgb * pow(ndoth, SPOW);

	vec3 ambient = gl_LightModel.ambient.rgb * KD;
	gl_FragColor.rgb = ambient + diffuse + specular;
	gl_FragColor.a = gl_FrontMaterial.diffuse.a;
}
