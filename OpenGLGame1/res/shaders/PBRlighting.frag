#version 450 core

in vec2 texCoord0;
in vec3 fragPos;
in vec3 normal0;

uniform vec3 camPos;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;

uniform vec3 albedoCol = vec3(-1);
//uniform vec3 normal = vec3(-1);
//uniform float metallic = -1;
//uniform float roughness = -1;
//uniform float ao = -1;

uniform float uvMultiplier = 1;

uniform vec3 lightPositions[4];
uniform vec3 lightColours[4];

const float PI = 3.14159265358979323846;

vec3 getNormalFromMap(){
    vec3 tangentNormal = texture(normalMap, texCoord0 * uvMultiplier).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(fragPos);
    vec3 Q2  = dFdy(fragPos);
    vec2 st1 = dFdx(texCoord0);
    vec2 st2 = dFdy(texCoord0);

    vec3 N   = normalize(normal0);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0){
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness){
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH*NdotH;

	float num = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness){

	float r = (roughness + 1.0);
	float k = (r*r) / 8.0;

	float num = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness){
	
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

void main(){
	vec3 albedo;
	if(albedoCol == vec3(-1)){
		albedo = pow(texture(albedoMap, texCoord0 * uvMultiplier).rgb, vec3(2.2));
	}else{
		albedo = albedoCol;
	}
	float metallic = texture(metallicMap, texCoord0 * uvMultiplier).r;
	float roughness = texture(roughnessMap, texCoord0 * uvMultiplier).r;
	float ao = texture(aoMap, texCoord0).r;

	vec3 N = getNormalFromMap();
	vec3 V = normalize(camPos - fragPos);

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);

	vec3 Lo = vec3(0.0);
	for(int i = 0; i < 4; i++){
		vec3 L = normalize(lightPositions[i] - fragPos);
		vec3 H = normalize(V + L);

		float distance = length(lightPositions[i] - fragPos);
		float attenuation = 1.0 / (distance * distance);
		vec3 radiance = lightColours[i] * attenuation;

		vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

		float NDF = DistributionGGX(N, H, roughness);
		float G = GeometrySmith(N, V, L, roughness);

		vec3 numerator = NDF * G * F;
		float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
		vec3 specular = numerator / max(denominator, 0.001);

		vec3 kS = F;
		vec3 kD = vec3(1.0) - kS;

		kD *= 1.0 - metallic;

		float NdotL = max(dot(N, L), 0.0);        
		Lo += (kD * albedo / PI + specular) * radiance * NdotL;
	}

	vec3 ambient = vec3(0.03) * albedo * ao;

	vec3 colour = ambient + Lo;

	colour = colour / (colour + vec3(1.0));
	colour = pow(colour, vec3(1.0/2.2));

	gl_FragColor = vec4(colour, 1.0);
}