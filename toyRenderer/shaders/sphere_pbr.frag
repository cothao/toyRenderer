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

vec3 F0 = vec3(0.3);

vec3 bReflectivity = vec3(0.4);

vec3 lightPosition = vec3(1., 1., -7.);

vec3 lightColor = vec3(1.);

vec3 albedo = vec3(.5, 0., 0.);

float roughness = 0.5;

uniform vec3 camPos;

void main()
{
	
	vec3 N = normalize(Normal);
	vec3 V = normalize(camPos - FragPos);
	vec3 L = normalize(lightPosition - FragPos);
	vec3 H = normalize(V + L);
	float distance = length(lightPosition - FragPos);
	float attenuation = 1. / (distance * distance);
	vec3 radiance = lightColor * attenuation;

	F0 = mix(F0, albedo, 0.4);

	vec3 Le = vec3(0.);

	vec3 Lo = vec3(0.);

	vec3 fLambert = albedo/PI;

	float D = GGX(N, H, roughness);

	float G = Smith(L, N, V, roughness);

	vec3 F = Schlick(V, H, F0);

	vec3 ks = F;
	vec3 kd = vec3(1.) - ks;

	vec3 num = D * G * F;
	float denom = 4 * max(dot(N ,V), 0.0)*max(dot(N, L), 0.) + 0.00001;

	vec3 diff = kd * fLambert;
	vec3 spec = num/denom;

	vec3 brdf = diff + spec;

	Lo += (brdf * radiance * max(dot(N, L), 0.000));

	vec3 ambient = vec3(0.03) * albedo * 1.;
    vec3 color = ambient + Lo;
	
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));  

	FragColor = vec4(color, 1.);

}

float GGX(vec3 N, vec3 H, float a)
{
	
	a = a * a;	

	float numerator = a * a;

	float denom = PI * pow((pow(max(dot(N, H), 0.00), 2.) * ((a * a) - 1) + 1), 2.);

	return numerator/denom;

};

float Smith(vec3 L, vec3 N, vec3 V, float a)
{

	return G1(N, L, a) * G1(N, V, a);

}
;

float G1(vec3 N, vec3 X, float a)
{

	float r = a + 1.;
	float k = (r * r)/8.;

	float numerator = max(dot(N, X), 0.0);

	float denominator = max(dot(N,X), 0.0) * (1.- k) + k;

	return numerator/denominator;

}

vec3 Schlick(vec3 V, vec3 H, vec3 F0)
{

	float cosTheta = max(dot(V,H), 0.00);

	return F0 + (1 - F0) * pow(clamp(1- cosTheta, 0., 1.), 5.);

};

