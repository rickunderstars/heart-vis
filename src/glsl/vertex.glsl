attribute float value;

uniform float uAbsMin;
uniform float uMin;
uniform float uMax;
uniform float uOnlyTwo;

varying float vNormalized;
varying float vIsNull;

float normalizeValue(float value, float minVal, float maxVal) {
	return (value - minVal) / (maxVal - minVal);
}

void main() {

	float n = normalizeValue(value, uMin, uMax);
	vNormalized = clamp(n, 0.0, 1.0);

	float epsilon = 0.0001;
	vIsNull = step(value, uMin + epsilon);
	vIsNull = vIsNull - uOnlyTwo;

	gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0);
}