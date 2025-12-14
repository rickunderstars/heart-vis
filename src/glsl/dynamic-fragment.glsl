
uniform float uTime;
uniform float uAmbientLightIntensity;

const float TimeSpeed = 0.05;
const int NumWaves = 10;

varying float val;
varying float vIsNull;

vec3 gradientWave(float t, vec3 colorStart, vec3 colorEnd) {
	t = 1.0 - t;
	t = t * t * t;
	return mix(colorStart, colorEnd, t);
}

void main() {

	float wave = (uTime * TimeSpeed - val) * float(NumWaves);
	float phase = fract(wave);

	vec3 nullColor = vec3(0.45, 0.45, 0.45); // grey
	vec3 color = gradientWave(phase, vec3(0.2, 0.2, 0.2), vec3(0.3, 1.0, 1.0));

	vec3 finalColor =
		mix(color, nullColor, vIsNull) * vec3(uAmbientLightIntensity);
	gl_FragColor = vec4(finalColor, 1.0);

	/*
	if (dFdx(wave) * dFdx(wave) + dFdy(wave) * dFdy(wave) > 0.006) {
		gl_FragColor.rgb = vec3(1.0, 0.0, 1.0);
	}
	*/
}
