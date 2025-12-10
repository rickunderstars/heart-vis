import * as THREE from "three";
import { getMax, get2Min } from "../utils/math-utils.js";
import { vShader, fShader, dyn_vShader, dyn_fShader } from "@js/main.js";

export function updateActiveMaterial(state) {
	if (!state.timeMode) {
		const activeMesh = state.getActiveMesh();
		const quality = state.activeQuality;
		const [absMin, min] = get2Min(
			activeMesh.valueSets[state.activeQuality],
		);
		activeMesh.mesh.geometry.setAttribute(
			"value",
			new THREE.BufferAttribute(activeMesh.valueSets[quality], 1),
		);
		activeMesh.mesh.material.dispose();
		activeMesh.mesh.material = new THREE.ShaderMaterial({
			uniforms: {
				uOnlyTwo: {
					value: absMin - min == 0 ? 1.0 : 0.0,
				},
				uAbsMin: {
					value: absMin,
				},
				uMin: {
					value: min,
				},
				uMax: {
					value: getMax(activeMesh.valueSets[quality]),
				},
			},
			vertexShader: vShader,
			fragmentShader: fShader,
			side: THREE.DoubleSide,
		});
	} else {
	}
}
