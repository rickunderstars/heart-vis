attribute float value;

uniform float uMin;
uniform float uMax;

varying float vNormalized;

float normalizeValue(float value, float minVal, float maxVal) {
	return (value - minVal) / (maxVal - minVal);
}

void main() {
	float n = normalizeValue(value, uMin, uMax);

	vNormalized = clamp(n, 0.0, 1.0);

	gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0);
}