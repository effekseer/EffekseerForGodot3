
R"(
vec4 ColorMap(sampler2D tex, vec2 uv, vec4 color) {
	vec4 texel = texture(tex, uv);
	return texel * color;
}
)"

#if LIGHTING
R"(
vec3 NormalMap(sampler2D tex, vec2 uv, vec2 tangent) {
	vec4 texel = texture(tex, uv) * 2.0 - 1.0;
	vec2 binormal = vec2(tangent.y, -tangent.x);
	return normalize(vec3(tangent, 0.0) * texel.x + vec3(binormal, 0.0) * texel.y + vec3(0.0, 0.0, 1.0) * texel.z);
}
)"
#endif

#if DISTORTION
R"(
vec2 DistortionMap(sampler2D screenTex, vec2 uv, float intencity, vec2 offset, vec2 tangent) {
	vec4 texel = texture(screenTex, uv);
	vec2 posU = vec2(tangent.y, -tangent.x);
	vec2 posR = tangent.xy;
	vec2 scale = (texel.xy * 2.0 - 1.0) * offset * intencity * 4.0;
	return posR * scale.x + posU * scale.y;
}
)"
#endif
