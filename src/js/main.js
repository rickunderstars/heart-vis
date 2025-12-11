// css
import "@css/styles.css";

// shaders
import vertexShader from "@glsl/static-vertex.glsl";
import fragmentShader from "@glsl/static-fragment.glsl";
import dynamicVertexShader from "@glsl/dynamic-animation-vertex.glsl";
import dynamicFragmentShader from "@glsl/dynamic-animation-fragment.glsl";

// three
import * as THREE from "three";
import { OrbitControls } from "three/addons/controls/OrbitControls.js";

// my stuff
import { getMax, get2Min } from "@js/utils/math-utils.js";
import { initLights, createScene } from "@js/visualization/scene.js";
import { createRenderer } from "@js/visualization/renderer.js";
import state from "@js/state/state.js";
import { setupFileHandlers } from "@js/interaction/file-handler.js";
import { vertexPicker } from "@js/interaction/vertex-picker.js";
import { updateActiveMaterial } from "./visualization/material-switch";

/////// load shaders ///////
export let vShader = vertexShader;
export let fShader = fragmentShader;
export let dyn_vShader = dynamicVertexShader;
export let dyn_fShader = dynamicFragmentShader;

/////// three.js ///////

export const scene = createScene();
initLights(scene);
const viewport = document.getElementById("viewport");
export const camera = new THREE.PerspectiveCamera(
	50,
	viewport.clientWidth / viewport.clientHeight,
);
camera.position.z = 5;

const renderer = createRenderer(viewport);
viewport.append(renderer.domElement);

const mouse = new THREE.Vector2();

export const controls = new OrbitControls(camera, renderer.domElement);
controls.addEventListener("change", () => {
	if (!state.timeMode) {
		renderer.render(scene, camera);
	}
});

if (!state.timeMode) {
	renderer.render(scene, camera);
} else {
	dynamicAnimate();
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

state.setActiveMesh(-1);

state.setActiveQuality(
	document.querySelector('.qualities-container input[name="quality"]:checked')
		.value,
);

setupFileHandlers({ state, scene, camera, controls, viewport, renderer });

let lastTime = 0;
const fps = 120;
const interval = 1000 / fps;

function dynamicAnimate(timeStamp) {
	if (!state.timeMode) {
		renderer.render(scene, camera);
		return;
	}

	requestAnimationFrame(dynamicAnimate);

	if (!lastTime) lastTime = timeStamp;

	const delta = timeStamp - lastTime;

	if (delta > interval) {
		lastTime = timeStamp - (delta % interval);

		if (state.activeMesh !== -1 && state.getActiveMesh()) {
			state.getActiveMesh().mesh.material.uniforms.uTime.value =
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
	vertexPicker({ state, mouse, camera });
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
		const meshData = state.getActiveMesh();
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
	const center = state.getActiveMesh().center;
	const radius = state.getActiveMesh().radius;
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
	if (state.activeMesh === -1 || !state.getActiveMesh()) return;

	if (!state.timeMode) {
		state.toggleTimeMode();
		updateActiveMaterial(state);
		clock.start();
		lastTime = 0;
		dynamicAnimate();
	} else {
		state.toggleTimeMode();
		updateActiveMaterial(state);
	}
});

document
	.querySelector(".qualities-container")
	.addEventListener("change", function (e) {
		if (e.target.name === "quality") {
			state.setActiveQuality(e.target.value);
			updateActiveMaterial(state);
			renderer.render(scene, camera);
		}
	});

document
	.querySelector(".meshes-container")
	.addEventListener("change", function (e) {
		if (e.target.name === "loaded-mesh") {
			state.setActiveMesh(e.target.value);
			updateActiveMaterial(state);

			for (let i = 0; i < state.meshes.length; i++) {
				if (i != state.activeMesh) {
					state.meshes[i].mesh.visible = false;
				} else {
					state.meshes[i].mesh.visible = true;
				}
			}
			const activeMesh = state.getActiveMesh();

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
