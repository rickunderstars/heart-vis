import * as THREE from "three";
import { OrbitControls } from "three/addons/controls/OrbitControls.js";

/////// three.js ///////

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

camera.position.z = 5;

const light1 = new THREE.DirectionalLight(0xffdddd, 1);
const light2 = new THREE.DirectionalLight(0xddddff, 1);
const light3 = new THREE.DirectionalLight(0xddffdd, 1);
light1.position.set(100, 100, 100);
light2.position.set(-100, -100, 100);
light2.position.set(0, -100, -100);
scene.add(light1);
scene.add(light2);
scene.add(light3);
scene.add(new THREE.AmbientLight(0x303030, 0.5));

function animate() {
	controls.update();
	renderer.render(scene, camera);
}

/////// model upload ///////

HeartModule().then((cpp) => {
	const rawMeshElement = document.getElementById("raw-mesh");
	const fileElement = document.getElementById("filename");

	rawMeshElement.addEventListener("change", function (event) {
		if (event.target.files.length > 0) {
			const file = event.target.files[0];
			fileElement.textContent = file.name;

			const reader = new FileReader();

			reader.onload = function (e) {
				const fileContent = e.target.result;
				const mesh = cpp.importMesh(fileContent);

				const verticesView = mesh.Float32ArrayOfVertices();
				const trianglesView = mesh.Uint32ArrayOfTriangles();

				const vertices = new Float32Array(verticesView);
				const triangles = new Uint32Array(trianglesView);

				const geometry = new THREE.BufferGeometry();
				geometry.setAttribute(
					"position",
					new THREE.BufferAttribute(vertices, 3)
				);

				geometry.setIndex(new THREE.BufferAttribute(triangles, 1));

				geometry.computeVertexNormals();

				const material = new THREE.MeshStandardMaterial({
					color: 0xddcccc,
					flatShading: false,
					side: THREE.DoubleSide,
				});
				const heart = new THREE.Mesh(geometry, material);
				scene.add(heart);

				geometry.computeBoundingSphere();
				const center = geometry.boundingSphere.center;
				const radius = geometry.boundingSphere.radius;
				camera.position.set(
					center.x,
					center.y,
					center.z + radius * 2.5
				);
				controls.target.set(center.x, center.y, center.z);
				controls.update();

				//const cubegeometry = new THREE.BoxGeometry(30, 30, 30);
				//const cube = new THREE.Mesh(cubegeometry, material);
				//scene.add(cube);
				//
				//cubegeometry.computeBoundingSphere();
				//const center = cubegeometry.boundingSphere.center;
				//const radius = cubegeometry.boundingSphere.radius;
				//camera.position.set(center.x, center.y, center.z + radius * 3);
				//controls.target.set(center.x, center.y, center.z);
				//controls.update();
			};
			reader.readAsText(file);
		} else {
			fileElement.textContent = "No file selected";
		}
	});
});
