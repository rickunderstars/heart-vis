import * as THREE from "three";

export function createScene() {
	const scene = new THREE.Scene();

	return scene;
}

export function initLights(scene) {
	const light1 = new THREE.DirectionalLight(0xffffff, 1);
	const light2 = new THREE.DirectionalLight(0xffffff, 1);
	const light3 = new THREE.DirectionalLight(0xffffff, 1);
	const light4 = new THREE.DirectionalLight(0xffffff, 1);
	light1.position.set(100, 100, 100);
	light2.position.set(-100, -100, -100);
	light3.position.set(100, -100, 100);
	light4.position.set(100, 100, -100);

	scene.add(light1, light2, light3, light4);
	scene.add(new THREE.AmbientLight(0xffffff, 3));
}
