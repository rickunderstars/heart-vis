import * as THREE from "three";
import { OrbitControls } from "three/addons/controls/OrbitControls.js";

HeartModule().then(module => {
			console.log('Module loaded!');

			const mesh = module.Mesh.simpleShape();
			console.log(mesh.toString()) ;

			mesh.delete();
		})

const scene = new THREE.Scene();
const viewport = document.getElementById("viewport");
const camera = new THREE.PerspectiveCamera(
	75,
	viewport.clientWidth / viewport.clientHeight,
	0.1,
	1000
);

const renderer = new THREE.WebGLRenderer();
renderer.setSize(viewport.clientWidth, viewport.clientHeight);
const controls = new OrbitControls(camera, renderer.domElement);
renderer.setAnimationLoop(animate);

viewport.append(renderer.domElement);

const geometry = new THREE.BoxGeometry(1, 1, 1);
const material = new THREE.MeshBasicMaterial({ color: 0x00ff00 });
const cube = new THREE.Mesh(geometry, material);
scene.add(cube);

camera.position.z = 5;

function animate() {
	controls.update();

	renderer.render(scene, camera);
}
