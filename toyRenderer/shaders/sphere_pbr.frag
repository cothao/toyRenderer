#version 330 core

out vec4 FragColor;

in vec3 Normal;

in vec3 FragPos;

#define PI 3.14f

float GGX(vec3 N, vec3 H, float a);

float Smith(vec3 L, vec3 N, vec3 V, float a);

vec3 Schlick(vec3 V, vec3 H, vec3 F0);

float G1(vec3 N, vec3 X, float a);

float PBR(vec3 lightEmission, float BRDF, vec3 lightColor, float cosTheta);

float BRDF(float GGX, float Smith, float Schlick);

vec3 F0 = vec3(0.4);

vec3 bReflectivity = vec3(0.4);

uniform vec3 lightPosition[4];

uniform vec3 lightColor[4];

uniform vec3 albedo;

uniform float roughness;
uniform float metallic;
uniform float ao;
uniform vec3 camPos;

void main()
{
	
	vec3 N = normalize(Normal);
	vec3 V = normalize(camPos - FragPos);
	
	vec3 Lo = vec3(0.);
	vec3 fLambert = albedo/PI;
	
	for (int i = 0; i < 4; i++)
	{
		vec3 L = normalize(lightPosition[i] - FragPos);
		vec3 H = normalize(V + L);
		float distance = length(lightPosition[i] - FragPos);
		float attenuation = 1. / (distance * distance);
		vec3 radiance = lightColor[i] * attenuation;

		F0 = mix(F0, albedo, metallic);


		float D = GGX(N, H, roughness);

		float G = Smith(L, N, V, roughness);

		vec3 F = Schlick(V, H, F0);

		vec3 ks = F;
		vec3 kd = vec3(1.) - ks;
		kd *= 1.0 - metallic; 

		vec3 num = D * G * F;
		float denom = 4 * (max(dot(N ,V), 0.0)*max(dot(N, L), 0.) + 0.0001);

		vec3 diff = kd * fLambert;
		vec3 spec = num/denom;

		vec3 brdf = diff + spec;

		float nDotL = max(dot(N, L), 0.0);

		Lo += (kd * albedo/PI + spec) * radiance * nDotL;
	
	};

	vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color = ambient + Lo;
	
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));  

	FragColor = vec4(color, 1.);

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
