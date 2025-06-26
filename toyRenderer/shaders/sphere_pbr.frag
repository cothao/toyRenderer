#version 330 core

out vec4 FragColor;

in vec3 Normal;

in vec3 FragPos;

#define PI 3.14f

float GGX(vec3 N, vec3 H, float a);

float Smith(vec3 L, vec3 N, vec3 V, float a);

float Schlick(vec3 V, vec3 H, float F0);

float G1(vec3 N, vec3 X, float a);

float PBR(vec3 lightEmission, float BRDF, vec3 lightColor, float cosTheta);

float BRDF(float GGX, float Smith, float Schlick);

float F0 = 0.3;

vec3 bReflectivity = vec3(0.4);

vec3 lightPosition = vec3(1., 1., -1.);

vec3 lightColor = vec3(1.);

vec3 albedo = vec3(1., 0., 0.);

float roughness = 0.0;

uniform vec3 camPos;

void main()
{
	
	float a = pow(roughness, 2.);

	vec3 N = normalize(Normal);
	vec3 V = normalize(camPos - FragPos);
	vec3 L = normalize(lightPosition - FragPos);
	vec3 H = normalize(L + V);

	vec3 Le = vec3(0.);

	vec3 Lo = vec3(0.);

	vec3 fLambert = albedo/PI;

	float D = GGX(N, H, a);

	float G = Smith(L, N, V, a);

	float F = Schlick(V, H, F0);

	float ks = F;
	float kd = 1. - ks;

	float num = D * G * F;
	float denom = 4 * (dot(V, N)(V, L));

	vec3 diff = kd * fLambert;
	float spec = num/denom;

	vec3 brdf = diff + spec;

	Lo = Le + (brdf * lightColor * dot(L, N));

	FragColor = vec4(Lo, 1.);

}

float GGX(vec3 N, vec3 H, float a)
{
	
	float numerator = a;

	float denom = PI * pow((pow(max(dot(N, H), 0.01), 2.) * (a - 1) + 1), 2.);

	return numerator/denom;

};

float Smith(vec3 L, vec3 N, vec3 V, float a)
{

	return G1(N, L, a) * G1(N, V, a);

}
;

float G1(vec3 N, vec3 X, float a)
{

	float k = a/2;

	float numerator = dot(N, X);

	float denominator = max(dot(N,X), 0.01) * (1- k) + k;

	return numerator/denominator;

}

float Schlick(vec3 V, vec3 H, float F0)
{

	float cosTheta = max(dot(V,H), 0.01);

	return F0 + (1 - F0) * pow((1- cosTheta), 5.);

};

