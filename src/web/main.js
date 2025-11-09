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

const light1 = new THREE.DirectionalLight(0xffffff, 1);
const light2 = new THREE.DirectionalLight(0xffffff, 1);
const light3 = new THREE.DirectionalLight(0xffffff, 1);
const light4 = new THREE.DirectionalLight(0xffffff, 1);
light1.position.set(100, 100, 100);
light2.position.set(-100, -100, -100);
light3.position.set(0, -100, -100);
light4.position.set(0, 100, 100);
scene.add(light1);
scene.add(light2);
scene.add(light3);
scene.add(light4);
scene.add(new THREE.AmbientLight(0x303030, 0.5));

function animate() {
	controls.update();
	renderer.render(scene, camera);
}

/////// model upload ///////

const meshesInfo = [];
const meshesGroup = new THREE.Group();

HeartModule().then((cpp) => {
	const rawMeshElement = document.getElementById("raw-mesh");
	const fileElement = document.getElementById("filename");

	rawMeshElement.addEventListener("change", function (event) {
		if (event.target.files.length > 0) {
			const file = event.target.files[0];
			fileElement.textContent = file.name;

			if (meshesInfo.some((item) => item.filename === file.name)) {
				console.log("Mesh already uploaded");
				return;
			}

			const reader = new FileReader();

			reader.onload = function (e) {
				const fileContent = e.target.result;
				let mesh = cpp.importMesh(fileContent);

				/// -- test meshes fixes -- ///

				if (file.name === "2-LA.mesh") {
					mesh.triangleFix(8703, 4559, 4538);
					mesh.fixNMEdges();
				} else if (file.name === "2-LA-FA.mesh") {
					mesh.triangleFix(25180, 12810, 12813);
					mesh.triangleFix(29108, 9930, 14703);
					mesh.triangleFix(21420, 10857, 10941);
					mesh.triangleFix(56, 38, 29);
					mesh.triangleFix(30812, 15492, 15447);
					mesh.triangleFix(30578, 14384, 14398);
					let fixTri = new cpp.Triangle(15417, 14398, 14381, -1);
					mesh.triangles.push_back(fixTri);
					mesh.fixNMEdges();
				}

				// -- end fix -- //

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
					flatShading: true,
					side: THREE.DoubleSide,
				});
				const heart = new THREE.Mesh(geometry, material);

				meshesGroup.add(heart);
				scene.add(meshesGroup);

				const box = new THREE.Box3();
				box.setFromObject(meshesGroup);
				const boundingSphere = new THREE.Sphere();
				box.getBoundingSphere(boundingSphere);
				const center = boundingSphere.center;
				const radius = boundingSphere.radius;

				camera.position.set(
					center.x,
					center.y,
					center.z + radius * 1.5
				);
				controls.target.set(center.x, center.y, center.z);
				controls.update();

				meshesInfo.push({
					mesh: heart,
					filename: file.name,
				});

				console.log(
					"Mesh loaded successfully. Meshes loaded:",
					meshesInfo.length
				);

				console.log("1: ", mesh.vertices.get(10000).nUnipolar);
				console.log("2: ", mesh.vertices.get(10000).nBipolar);
				console.log("3: ", mesh.vertices.get(10000).nLAT);
				console.log("4: ", mesh.vertices.get(10000).nEML);
				console.log("5: ", mesh.vertices.get(10000).nExtEML);
				console.log("6: ", mesh.vertices.get(10000).nSCAR);
			};
			reader.readAsText(file);
		} else {
			fileElement.textContent = "No file selected";
		}
	});
});
