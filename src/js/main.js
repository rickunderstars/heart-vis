import * as THREE from "three";
import { OrbitControls } from "three/addons/controls/OrbitControls.js";
import { getMax, get2Min } from "./utils/math-utils.js";
import { initScene } from "./core/scene.js";
import { createRenderer } from "./core/renderer.js";
import {
	state,
	getActiveMesh,
	setActiveMesh,
	setActiveQuality,
} from "./state/state.js";
import { processFile } from "./loaders/mesh-processing.js";
import {
	setupBrowseFile,
	setupDragAndDrop,
} from "./interaction/file-handler.js";

/////// load shaders ///////
export let vShader = null;
export let fShader = null;
export let dyn_vShader = null;
export let dyn_fShader = null;

try {
	document.getElementById("viewport").innerHTML = "Loading shaders...";
	console.log("Loading shaders...");
	[vShader, fShader, dyn_vShader, dyn_fShader] = await Promise.all([
		fetch("../glsl/static-vertex.glsl").then((r) => {
			if (!r.ok) throw new Error("Shader static-vertex.glsl not found");
			return r.text();
		}),
		fetch("../glsl/static-fragment.glsl").then((r) => {
			if (!r.ok) throw new Error("Shader static-fragment.glsl not found");
			return r.text();
		}),
		fetch("../glsl/dynamic-animation-vertex.glsl").then((r) => {
			if (!r.ok)
				throw new Error(
					"Shader dynamic-animation-vertex.glsl not found",
				);
			return r.text();
		}),
		fetch("../glsl/dynamic-animation-fragment.glsl").then((r) => {
			if (!r.ok)
				throw new Error(
					"Shader dynamic-animation-fragment.glsl not found",
				);
			return r.text();
		}),
	]);
	console.log("Shaders loaded successfully");
	document.getElementById("viewport").innerHTML = "";
} catch (err) {
	console.error("Error encountered while loading shaders: ", err);
	document.getElementById("viewport").innerHTML =
		"Error encountered while loading shaders: " + err;
}

/////// three.js ///////

export const scene = initScene();
const viewport = document.getElementById("viewport");
export const camera = new THREE.PerspectiveCamera(
	50,
	viewport.clientWidth / viewport.clientHeight,
);
camera.position.z = 5;

const renderer = createRenderer(viewport);
viewport.append(renderer.domElement);

const mouse = new THREE.Vector2();
const raycaster = new THREE.Raycaster();

export const controls = new OrbitControls(camera, renderer.domElement);
controls.addEventListener("change", () => {
	if (!state.timeMode) {
		renderer.render(scene, camera);
	}
});

if (!state.timeMode) {
	renderer.render(scene, camera);
}

window.addEventListener("resize", onViewportResize);

window.addEventListener("mousemove", onMouseMove, false);

const clock = new THREE.Clock();

/////// model upload ///////

state.meshes = [];
const qualities = [
	"unipolar",
	"bipolar",
	"lat",
	"eml",
	"exteml",
	"scar",
	"groupid",
];

setActiveMesh(-1);

if (state.timeMode) {
	dynamicAnimate();
}

setActiveQuality(
	document.querySelector('.qualities-container input[name="quality"]:checked')
		.value,
);

setupBrowseFile(processFile);
setupDragAndDrop(processFile, viewport);

let then = 0;
const fps = 120;
const interval = 1000 / fps;

function dynamicAnimate(timeStamp) {
	if (!state.timeMode) {
		return;
	}

	requestAnimationFrame(dynamicAnimate);

	if (!then) then = timeStamp;

	const delta = timeStamp - then;

	if (delta > interval) {
		then = timeStamp - (delta % interval);

		if (state.activeMesh !== -1 && getActiveMesh()) {
			getActiveMesh().mesh.material.uniforms.uTime.value =
				clock.getElapsedTime();
		}

		renderer.render(scene, camera);
	}
}

function onViewportResize() {
	camera.aspect = viewport.clientWidth / viewport.clientHeight;
	camera.updateProjectionMatrix();
	renderer.setSize(viewport.clientWidth, viewport.clientHeight);
	if (!state.timeMode) {
		renderer.render(scene, camera);
	}
}

function onMouseMove(e) {
	const rect = renderer.domElement.getBoundingClientRect();
	mouse.x = ((e.clientX - rect.left) / rect.width) * 2 - 1;
	mouse.y = -(((e.clientY - rect.top) / rect.height) * 2 - 1);
	vertexPicker();
}

function vertexPicker() {
	if (state.activeMesh === -1 || !getActiveMesh()) {
		document.getElementById("vertex-info").innerHTML = "no mesh loaded";
		return;
	}
	raycaster.setFromCamera(mouse, camera);
	const intersects = raycaster.intersectObject(getActiveMesh().mesh);

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

		const active = getActiveMesh().valueSets;

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

function setData(meshIndex, dataSet) {
	if (state.meshes[meshIndex] && !state.timeMode) {
		const mesh = state.meshes[meshIndex].mesh;
		const valueSets = state.meshes[meshIndex].valueSets;
		const [absMin, min] = get2Min(valueSets[dataSet]);

		mesh.material.uniforms.uOnlyTwo.value = absMin - min == 0 ? 1.0 : 0.0;
		mesh.material.uniforms.uAbsMin.value = absMin;
		mesh.material.uniforms.uMin.value = min;
		mesh.material.uniforms.uMax.value = getMax(valueSets[dataSet]);

		mesh.geometry.deleteAttribute("value");
		mesh.geometry.setAttribute(
			"value",
			new THREE.BufferAttribute(valueSets[dataSet], 1),
		);

		renderer.render(scene, camera);
	} else {
		const meshData = getActiveMesh();
		const currentMesh = meshData.mesh;
		const valueSets = meshData.valueSets;
		state.timeMode = false;

		const [absMin, min] = get2Min(valueSets[state.activeQuality]);

		currentMesh.material = new THREE.ShaderMaterial({
			uniforms: {
				uOnlyTwo: { value: absMin - min == 0 ? 1.0 : 0.0 },
				uAbsMin: { value: absMin },
				uMin: { value: min },
				uMax: { value: getMax(valueSets[state.activeQuality]) },
			},
			vertexShader: vShader,
			fragmentShader: fShader,
			side: THREE.DoubleSide,
		});

		setData(state.activeMesh, state.activeQuality);
	}
}

function cameraReset() {
	const center = getActiveMesh().center;
	const radius = getActiveMesh().radius;
	camera.position.set(center.x, center.y, center.z + radius * 2.5);
	controls.target.set(center.x, center.y, center.z);
	controls.update();
}

document.getElementById("camera-reset").addEventListener("click", () => {
	cameraReset();
});

document.addEventListener("keydown", (k) => {
	if (k.key === "r") {
		cameraReset();
	}
});

document.getElementById("dynamic-animation").addEventListener("click", () => {
	if (state.activeMesh === -1 || !getActiveMesh()) return;

	const meshData = getActiveMesh();
	const currentMesh = meshData.mesh;
	const valueSets = meshData.valueSets;
	const lat = valueSets[state.activeQuality];

	if (currentMesh.material) {
		currentMesh.material.dispose();
	}

	if (!state.timeMode) {
		state.timeMode = true;

		clock.start();
		then = 0;

		then = 0;

		dynamicAnimate();

		currentMesh.geometry.deleteAttribute("value");
		currentMesh.geometry.setAttribute(
			"value",
			new THREE.BufferAttribute(lat, 1),
		);

		const [absMin, min] = get2Min(lat);

		currentMesh.material = new THREE.ShaderMaterial({
			uniforms: {
				uAbsMin: { value: absMin },
				uMin: { value: min },
				uMax: { value: getMax(lat) },
				uTime: { value: 0 },
			},
			vertexShader: dyn_vShader,
			fragmentShader: dyn_fShader,
			side: THREE.DoubleSide,
		});
	} else {
		state.timeMode = false;

		const [absMin, min] = get2Min(valueSets[state.activeQuality]);

		currentMesh.material = new THREE.ShaderMaterial({
			uniforms: {
				uOnlyTwo: { value: absMin - min == 0 ? 1.0 : 0.0 },
				uAbsMin: { value: absMin },
				uMin: { value: min },
				uMax: { value: getMax(valueSets[state.activeQuality]) },
			},
			vertexShader: vShader,
			fragmentShader: fShader,
			side: THREE.DoubleSide,
		});

		setData(state.activeMesh, state.activeQuality);
	}
});

document
	.querySelector(".qualities-container")
	.addEventListener("change", function (e) {
		if (e.target.name === "quality") {
			setActiveQuality(e.target.value);
			setData(state.activeMesh, state.activeQuality);
		}
	});

document
	.querySelector(".meshes-container")
	.addEventListener("change", function (e) {
		if (e.target.name === "loaded-mesh") {
			setActiveMesh(e.target.value);
			if (!state.timeMode) {
				setData(state.activeMesh, state.activeQuality);
			} else {
				const meshData = getActiveMesh();
				const currentMesh = meshData.mesh;
				const valueSets = meshData.valueSets;
				const lat = valueSets[state.activeQuality];
				clock.start();
				then = 0;

				then = 0;

				dynamicAnimate();

				currentMesh.geometry.deleteAttribute("value");
				currentMesh.geometry.setAttribute(
					"value",
					new THREE.BufferAttribute(lat, 1),
				);

				const [absMin, min] = get2Min(lat);

				currentMesh.material = new THREE.ShaderMaterial({
					uniforms: {
						uAbsMin: { value: absMin },
						uMin: { value: min },
						uMax: { value: getMax(lat) },
						uTime: { value: 0 },
					},
					vertexShader: dyn_vShader,
					fragmentShader: dyn_fShader,
					side: THREE.DoubleSide,
				});
			}
			for (let i = 0; i < state.meshes.length; i++) {
				if (i != state.activeMesh) {
					state.meshes[i].mesh.visible = false;
				} else {
					state.meshes[i].mesh.visible = true;
				}
			}
			const activeMesh = getActiveMesh();

			camera.position.set(
				activeMesh.center.x,
				activeMesh.center.y,
				activeMesh.center.z + activeMesh.radius * 2.5,
			);
			controls.target.set(
				activeMesh.center.x,
				activeMesh.center.y,
				activeMesh.center.z,
			);
			controls.update();
		}
	});
