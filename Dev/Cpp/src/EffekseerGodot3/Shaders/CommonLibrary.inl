
R"(
vec4 ColorMap(sampler2D tex, vec2 uv, vec4 color) {
	vec4 texel = texture(tex, uv);
	return texel * color;
}
)"

#if LIGHTING
R"(
vec3 NormalMap(sampler2D tex, vec2 uv, vec3 normal, vec3 tangent, vec3 binormal) {
	vec4 texel = texture(tex, uv) * 2.0 - 1.0;
	return normalize(tangent * texel.x + binormal * texel.y + normal * texel.z);
}
)"
#endif

#if DISTORTION
R"(
vec2 DistortionMap(sampler2D screenTex, vec2 uv, float intencity, vec2 offset, vec3 tangent, vec3 binormal) {
	vec4 texel = texture(screenTex, uv);
	vec2 posU = binormal.xy;
	vec2 posR = tangent.xy;
	vec2 scale = (texel.xy * 2.0 - 1.0) * offset * intencity * 4.0;
	return posR * scale.x + posU * scale.y;
}
)"
#endif

#if SOFT_PARTICLE
R"(
float SoftParticle(sampler2D depthTex, vec2 uv, float fragZ, vec4 params, vec4 reconstruct1, vec4 reconstruct2) {
	float backgroundZ = texture(depthTex, uv).x;
	float distanceFar = params.x;
	float distanceNear = params.y;
	float distanceNearOffset = params.z;
	vec2 zs = vec2(backgroundZ, fragZ) * reconstruct1.x + reconstruct1.y;
	vec2 depth = ((zs * reconstruct2.w) - vec2(reconstruct2.y)) / (vec2(reconstruct2.x) - (zs * reconstruct2.z));
	float alphaFar = (depth.y - depth.x) / distanceFar;
	float alphaNear = ((-distanceNearOffset) - depth.y) / distanceNear;
	return min(max(min(alphaFar, alphaNear), 0.0), 1.0);
}
)"
#endif
