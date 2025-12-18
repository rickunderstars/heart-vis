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
import { updateActiveMaterial } from "@js/visualization/material-update";
import { loadShaders } from "@js/visualization/shader-update";
import { setupEventHandlers } from "@js/interaction/event-handlers";
import { colorizeGradient } from "./visualization/color-gauge";

/////// three.js ///////

const scene = createScene();
initLights(scene);
const viewport = document.getElementById("viewport");
const camera = new THREE.PerspectiveCamera(
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

setupEventHandlers({ camera, controls, renderer, scene, mouse, state });

colorizeGradient();

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
