import { reloadShaderMaterial } from "@js/visualization/shader-update";
import { vertexPicker } from "@js/interaction/vertex-picker.js";
import { setGaugeLine } from "../visualization/color-gauge";

export function setupEventHandlers(dependencies) {
	const { camera, controls, renderer, mouse, state } = dependencies;

	document.getElementById("camera-reset").addEventListener("click", () => {
		cameraReset(camera, controls);
	});

	document.addEventListener("keydown", (k) => {
		if (k.key.toLowerCase() === "r") {
			cameraReset(camera, controls);
		}
	});

	document.addEventListener("keydown", (k) => {
		if (k.key.toLowerCase() === "s") {
			reloadShaderMaterial(state);
		}
	});

	window.addEventListener("resize", () => {
		onViewportResize(camera, renderer);
	});

	window.addEventListener("mousemove", (e) => {
		onMouseMove(e, camera, renderer, mouse, state);
	});
}

function cameraReset(camera, controls) {
	const center = state.getActiveMesh().center;
	const radius = state.getActiveMesh().radius;
	camera.position.set(center.x, center.y, center.z + radius * 2.5);
	controls.target.set(center.x, center.y, center.z);
	controls.update();
}

function onViewportResize(camera, renderer) {
	camera.aspect = viewport.clientWidth / viewport.clientHeight;
	camera.updateProjectionMatrix();
	renderer.setSize(viewport.clientWidth, viewport.clientHeight);
	if (!state.timeMode) {
		renderer.render(scene, camera);
	}
}

function onMouseMove(e, camera, renderer, mouse, state) {
	const rect = renderer.domElement.getBoundingClientRect();
	mouse.x = ((e.clientX - rect.left) / rect.width) * 2 - 1;
	mouse.y = -(((e.clientY - rect.top) / rect.height) * 2 - 1);
	const value = vertexPicker({ state, mouse, camera });
	setGaugeLine(value, state);
}
