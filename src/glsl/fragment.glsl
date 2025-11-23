uniform vec3 uColorLow;
uniform vec3 uColorHigh;

varying float vNormalized;

void main() {
	vec3 color = mix(uColorLow, uColorHigh, vNormalized);

	gl_FragColor = vec4(color, 1.0);
}