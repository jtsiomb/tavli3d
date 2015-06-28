/* vi: set ft=glsl */
uniform sampler2D tex;
uniform sampler2DShadow shadowmap;

varying vec3 vdir, ldir, normal;
varying vec4 shadow_tc;

#define KD gl_FrontMaterial.diffuse.rgb
#define KS gl_FrontMaterial.specular.rgb
#define SPOW gl_FrontMaterial.shininess

void main()
{
	float shadow = shadow2DProj(shadowmap, shadow_tc).x;
	vec4 texel = texture2D(tex, gl_TexCoord[0].st);

	vec3 n = normalize(normal);
	vec3 v = normalize(vdir);
	vec3 l = normalize(ldir);
	vec3 h = normalize(l + v);

	float ndotl = max(dot(n, l), 0.0);
	float ndoth = max(dot(n, h), 0.0);

	vec3 albedo = KD * texel.rgb;
	vec3 diffuse = albedo * gl_LightSource[0].diffuse.rgb * ndotl;
	vec3 specular = KS * gl_LightSource[0].specular.rgb * pow(ndoth, SPOW);

	vec3 ambient = gl_LightModel.ambient.rgb * albedo;
	gl_FragColor.rgb = ambient + (diffuse + specular) * shadow;
	gl_FragColor.a = gl_FrontMaterial.diffuse.a * texel.a;
}
