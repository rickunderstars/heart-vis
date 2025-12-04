varying float vNormalized;
varying float vIsNull;

vec3 TurboColormap(in float x) {
	const vec4 kRedVec4 =
		vec4(0.13572138, 4.61539260, -42.66032258, 132.13108234);
	const vec4 kGreenVec4 =
		vec4(0.09140261, 2.19418839, 4.84296658, -14.18503333);
	const vec4 kBlueVec4 =
		vec4(0.10667330, 12.64194608, -60.58204836, 110.36276771);
	const vec2 kRedVec2 = vec2(-152.94239396, 59.28637943);
	const vec2 kGreenVec2 = vec2(4.27729857, 2.82956604);
	const vec2 kBlueVec2 = vec2(-89.90310912, 27.34824973);

	x = clamp(x, 0.0, 1.0);
	vec4 v4 = vec4(1.0, x, x * x, x * x * x);
	vec2 v2 = v4.zw * v4.z;
	return vec3(dot(v4, kRedVec4) + dot(v2, kRedVec2),
				dot(v4, kGreenVec4) + dot(v2, kGreenVec2),
				dot(v4, kBlueVec4) + dot(v2, kBlueVec2));
}

void main() {

	vec3 nullColor = vec3(0.45, 0.45, 0.45); // grey

	/* checker
		vec2 checker = floor(gl_FragCoord.xy / 3.0);
		float pattern = mod(checker.x + checker.y, 2.0);
		vec3 nullColor = mix(vec3(0.4), vec3(0.6), pattern);
	*/

	vec3 color = TurboColormap(vNormalized);

	vec3 finalColor = mix(color, nullColor, vIsNull);

	gl_FragColor = vec4(finalColor, 1.0);

	/* deletes fragment
	if (vIsNull > 0.5) {
		discard;
	}

	vec3 color = TurboColormap(vNormalized);
	gl_FragColor = vec4(color, 1.0);
	*/
}