import * as THREE from "three";

export function createRenderer(viewport) {
	const renderer = new THREE.WebGLRenderer({ alpha: true });
	renderer.setSize(viewport.clientWidth, viewport.clientHeight);

	return renderer;
}
