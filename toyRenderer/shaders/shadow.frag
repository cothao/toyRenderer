#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;


uniform vec3 lightPos;
uniform vec3 viewPos;

uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform samplerCube depthMap;
uniform sampler2D   brdfLUT;  

uniform vec3 lightPosition[4];
uniform vec3 lightColor[4];
uniform sampler2D base_map;
uniform sampler2D metallic_map;
uniform sampler2D normal_map;
uniform sampler2D roughness_map;
uniform sampler2D aoMap;
uniform float far_plane;
uniform vec3 camPos;

#define PI 3.14f

float GGX(vec3 N, vec3 H, float a);

float Smith(vec3 L, vec3 N, vec3 V, float a);

vec3 Schlick(vec3 V, vec3 H, vec3 F0);

float G1(vec3 N, vec3 X, float a);

float PBR(vec3 lightEmission, float BRDF, vec3 lightColor, float cosTheta);

float BRDF(float GGX, float Smith, float Schlick);

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);

vec3 F0 = vec3(0.4);

vec3 bReflectivity = vec3(0.4);

vec3 PBR(vec3 albedo, float metallic, float roughness, float ao);

vec3 getNormalFromMap();

float ShadowCalculation(vec3 fragPos)
{
    vec3 fragToLight = fragPos - lightPos; 
    float closestDepth = texture(depthMap, fragToLight).r;
	closestDepth *= far_plane;  
	float currentDepth = length(fragToLight);  
	float bias = 0.05; 

	float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0; 

	return shadow;
}

void main()
{           

	vec3 albedo = vec3(texture(base_map, fs_in.TexCoords));
	float metallic = texture(metallic_map, fs_in.TexCoords).r;
	float roughness = texture(roughness_map, fs_in.TexCoords).r;
	float ao = texture(aoMap, fs_in.TexCoords).r;

	vec3 pbr = PBR(albedo, metallic, roughness, ao);

    FragColor = vec4(pbr, 1.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}   

vec3 Schlick(vec3 V, vec3 H, vec3 F0)
{

	float cosTheta = max(dot(H,V), 0.00);

	return F0 + (1 - F0) * pow(clamp(1- cosTheta, 0., 1.), 5.);

};

float GGX(vec3 N, vec3 H, float a)
{
	
	a = a * a;	

	float numerator = a * a;

	float denom = PI * pow((pow(max(dot(N, H), 0.00), 2.) * ((a * a) - 1) + 1), 2.);

	return numerator/denom;

};


float G1(vec3 N, vec3 X, float a)
{

	float r = a + 1.;
	float k = (r * r)/8.;

	float numerator = max(dot(N, X), 0.0);

	float denominator = max(dot(N,X), 0.0) * (1.- k) + k;

	return numerator/denominator;

}

float Smith(vec3 L, vec3 N, vec3 V, float a)
{

	return G1(N, L, a) * G1(N, V, a);

}
;

vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(normal_map, fs_in.TexCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(fs_in.FragPos);
    vec3 Q2  = dFdy(fs_in.FragPos);
    vec2 st1 = dFdx(fs_in.TexCoords);
    vec2 st2 = dFdy(fs_in.TexCoords);

    vec3 N   = normalize(fs_in.Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}


vec3 PBR(vec3 albedo, float metallic, float roughness, float ao)
{
	vec3 N = getNormalFromMap();
	vec3 V = normalize(camPos - fs_in.FragPos);
	vec3 R = normalize(reflect(-V, N));   

	vec3 Lo = vec3(0.);
	vec3 fLambert = albedo/PI;
	
	for (int i = 0; i < 1; i++)
	{
		vec3 L = normalize(lightPosition[i] - fs_in.FragPos);
		vec3 H = normalize(V + L);
		float distance = length(lightPosition[i] - fs_in.FragPos);
		float attenuation = 1. / (distance * distance);
		vec3 radiance = lightColor[i] * attenuation;

		F0 = mix(F0, albedo, vec3(1.));

		float D = GGX(N, H, 1.);

		float G = Smith(L, N, V, 1.);

		vec3 F = Schlick(V, H, F0);

		vec3 ks = F;
		vec3 kd = vec3(1.) - ks;
		kd *= 1.0 - vec3(1.); 

		vec3 num = D * G * F;
		float denom = 4 * (max(dot(N ,V), 0.0)*max(dot(N, L), 0.) + 0.0001);

		vec3 diff = kd * fLambert;
		vec3 spec = num/denom;

		vec3 brdf = diff + spec;

		float nDotL = max(dot(N, L), 0.0);

		Lo += (kd * albedo/PI + spec) * radiance * nDotL;
	
	};

	vec3 F = fresnelSchlickRoughness(max(dot(V,N), 0.), F0, roughness);
	vec3 kS = F;
	vec3 kD = 1.0 - kS;
	vec3 irradiance = texture(irradianceMap, N).rgb;
	vec3 diffuse    = irradiance * albedo;

	const float MAX_REFLECTION_LOD = 4.0;
	vec3 prefilteredColor = textureLod(prefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb;   
	vec2 envBRDF  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
	vec3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);

	vec3 ambient = (kD * diffuse + specular) * ao;

    vec3 color = ambient + Lo;
	

    float shadow = ShadowCalculation(fs_in.FragPos);       
	vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * albedo;
	color += lighting;
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));  


	return color;

}
