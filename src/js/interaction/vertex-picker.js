import * as THREE from "three";

export function vertexPicker(dependencies) {
	const { state, mouse, camera } = dependencies;

	if (state.activeMesh === -1 || !state.getActiveMesh()) {
		document.getElementById("vertex-info").innerHTML = "no mesh loaded";
		return;
	}
	const active = state.getActiveMesh().valueSets;
	const raycaster = new THREE.Raycaster();
	raycaster.setFromCamera(mouse, camera);
	const intersects = raycaster.intersectObject(state.getActiveMesh().mesh);

	if (intersects.length > 0) {
		const firstHit = intersects[0];
		const face = firstHit.face;
		const geometry = firstHit.object.geometry;
		const positions = geometry.attributes.position;
		const object = firstHit.object;

		const v0 = new THREE.Vector3().fromBufferAttribute(positions, face.a);
		const v1 = new THREE.Vector3().fromBufferAttribute(positions, face.b);
		const v2 = new THREE.Vector3().fromBufferAttribute(positions, face.c);

		v0.applyMatrix4(object.matrixWorld);
		v1.applyMatrix4(object.matrixWorld);
		v2.applyMatrix4(object.matrixWorld);

		const bary = new THREE.Vector3();
		THREE.Triangle.getBarycoord(firstHit.point, v0, v1, v2, bary);

		const unipolar =
			active.unipolar[face.a] * bary.x +
			active.unipolar[face.b] * bary.y +
			active.unipolar[face.c] * bary.z;

		const bipolar =
			active.bipolar[face.a] * bary.x +
			active.bipolar[face.b] * bary.y +
			active.bipolar[face.c] * bary.z;

		const lat =
			active.lat[face.a] * bary.x +
			active.lat[face.b] * bary.y +
			active.lat[face.c] * bary.z;

		let intValuesIndex = face.a;
		if (bary.y > bary.x && bary.y > bary.z) {
			intValuesIndex = face.b;
		} else if (bary.z > bary.x && bary.z > bary.y) {
			intValuesIndex = face.c;
		}

		const eml = active.eml[intValuesIndex];
		const exteml = active.exteml[intValuesIndex];
		const scar = active.scar[intValuesIndex];
		const groupid = active.groupid[intValuesIndex];

		document.getElementById("vertex-info").innerHTML =
			"<div class='vertex-info'>Unipolar: " +
			unipolar.toFixed(3) +
			"</br>Bipolar: " +
			bipolar.toFixed(3) +
			"</br>LAT: " +
			lat.toFixed(3) +
			"</br>EML: " +
			eml +
			"</br>ExtEML: " +
			exteml +
			"</br>SCAR: " +
			scar +
			"</br>groupID: " +
			groupid +
			"</div>";
	} else {
		document.getElementById("vertex-info").innerHTML = "";
	}
}
