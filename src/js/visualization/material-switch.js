import * as THREE from "three";
import { getMax, get2Min } from "../utils/math-utils.js";
import { vShader, fShader, dyn_vShader, dyn_fShader } from "@js/main.js";

export function updateActiveMaterial(state) {
	const activeMesh = state.getActiveMesh();
	const quality = state.activeQuality;
	const [absMin, min] = get2Min(activeMesh.valueSets[quality]);
	const max = getMax(activeMesh.valueSets[quality]);
	activeMesh.mesh.geometry.setAttribute(
		"value",
		new THREE.BufferAttribute(activeMesh.valueSets[quality], 1),
	);
	activeMesh.mesh.material.dispose();
	if (!state.timeMode) {
		activeMesh.mesh.material = new THREE.ShaderMaterial({
			uniforms: {
				uOnlyTwo: { value: absMin - min == 0 ? 1.0 : 0.0 },
				uAbsMin: { value: absMin },
				uMin: { value: min },
				uMax: { value: max },
			},
			vertexShader: vShader,
			fragmentShader: fShader,
			side: THREE.DoubleSide,
		});
	} else {
		activeMesh.mesh.material = new THREE.ShaderMaterial({
			uniforms: {
				uAbsMin: { value: absMin },
				uMin: { value: min },
				uMax: { value: max },
				uTime: { value: 0 },
			},
			vertexShader: dyn_vShader,
			fragmentShader: dyn_fShader,
			side: THREE.DoubleSide,
		});
	}
}
