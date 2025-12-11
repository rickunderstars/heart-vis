// css
import "@css/styles.css";

// three
import * as THREE from "three";
import { OrbitControls } from "three/addons/controls/OrbitControls.js";

// my stuff
import { initLights, createScene } from "@js/visualization/scene.js";
import { createRenderer } from "@js/visualization/renderer.js";
import state from "@js/state/state.js";
import { setupFileHandlers } from "@js/interaction/file-handlers.js";
import { vertexPicker } from "@js/interaction/vertex-picker.js";
import { updateActiveMaterial } from "./visualization/material-update";
import {
	loadShaders,
	reloadShaderMaterial,
} from "./visualization/shader-update";

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
let shaders = await loadShaders();

state.meshes = [];

state.setActiveMesh(-1);

state.setActiveQuality(
	document.querySelector('.qualities-container input[name="quality"]:checked')
		.value,
);

setupFileHandlers({
	state,
	shaders,
	scene,
	camera,
	controls,
	viewport,
	renderer,
});

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

document.addEventListener("keydown", (k) => {
	if (k.key === "s") {
		reloadShaderMaterial(state);
	}
});

document.getElementById("dynamic-animation").addEventListener("click", () => {
	if (state.activeMesh === -1 || !state.getActiveMesh()) return;

	if (!state.timeMode) {
		state.toggleTimeMode();
		updateActiveMaterial({ state, shaders });
		clock.start();
		lastTime = 0;
		dynamicAnimate();
	} else {
		state.toggleTimeMode();
		updateActiveMaterial({ state, shaders });
	}
});

document
	.querySelector(".qualities-container")
	.addEventListener("change", function (e) {
		if (e.target.name === "quality") {
			state.setActiveQuality(e.target.value);
			updateActiveMaterial({ state, shaders });
			renderer.render(scene, camera);
		}
	});

document
	.querySelector(".meshes-container")
	.addEventListener("change", function (e) {
		if (e.target.name === "loaded-mesh") {
			state.setActiveMesh(e.target.value);
			updateActiveMaterial({ state, shaders });

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
