import * as THREE from "three";
import { OrbitControls } from "three/addons/controls/OrbitControls.js";

/////// three.js ///////

const scene = new THREE.Scene();
const viewport = document.getElementById("viewport");
const camera = new THREE.PerspectiveCamera(
	50,
	viewport.clientWidth / viewport.clientHeight
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
light3.position.set(100, -100, 100);
light4.position.set(100, 100, -100);
scene.add(light1, light2, light3, light4);
scene.add(new THREE.AmbientLight(0xffffff, 1));

function animate() {
	controls.update();
	renderer.render(scene, camera);
}

/////// model upload ///////

const meshes = [];
const qualities = ["unipolar", "bipolar", "lat", "eml", "exteml", "scar"];
let activeMesh = -1;

HeartModule().then((cpp) => {
	const rawMeshElement = document.getElementById("raw-mesh");
	const fileElement = document.getElementById("filename");

	rawMeshElement.addEventListener("change", function (event) {
		if (event.target.files.length > 0) {
			const file = event.target.files[0];
			fileElement.textContent = "Last upload: " + file.name;

			if (meshes.some((item) => item.filename === file.name)) {
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
				const unipolarView = mesh.Float32ArrayOfTurboColors(
					qualities[0]
				);
				const bipolarView = mesh.Float32ArrayOfTurboColors(
					qualities[1]
				);
				const latView = mesh.Float32ArrayOfTurboColors(qualities[2]);
				const emlView = mesh.Float32ArrayOfTurboColors(qualities[3]);
				const extemlView = mesh.Float32ArrayOfTurboColors(qualities[4]);
				const scarView = mesh.Float32ArrayOfTurboColors(qualities[5]);

				const vertices = new Float32Array(verticesView);
				const triangles = new Uint32Array(trianglesView);

				const turboSets = {
					unipolar: new Float32Array(unipolarView),
					bipolar: new Float32Array(bipolarView),
					lat: new Float32Array(latView),
					eml: new Float32Array(emlView),
					exteml: new Float32Array(extemlView),
					scar: new Float32Array(scarView),
				};

				const geometry = new THREE.BufferGeometry();
				geometry.setAttribute(
					"position",
					new THREE.BufferAttribute(vertices, 3)
				);
				geometry.setIndex(new THREE.BufferAttribute(triangles, 1));
				geometry.computeVertexNormals();
				geometry.setAttribute(
					"color",
					new THREE.BufferAttribute(turboSets.unipolar, 3)
				);

				const material = new THREE.MeshStandardMaterial({
					vertexColors: true,
					flatShading: false,
					side: THREE.DoubleSide,
					roughness: 1,
				});

				const heart = new THREE.Mesh(geometry, material);

				meshes.forEach((meshData) => {
					meshData.mesh.visible = false;
				});

				scene.add(heart);

				const box = new THREE.Box3().setFromObject(heart);
				const boundingSphere = new THREE.Sphere();
				box.getBoundingSphere(boundingSphere);
				const center = boundingSphere.center;
				const radius = boundingSphere.radius;

				camera.position.set(
					center.x,
					center.y,
					center.z + radius * 2.5
				);
				controls.target.set(center.x, center.y, center.z);
				controls.update();

				meshes.push({
					mesh: heart,
					filename: file.name,
					colorSets: turboSets,
				});

				activeMesh = meshes.length - 1;
				console.log(activeMesh);

				console.log(
					"Mesh loaded successfully. Meshes loaded:",
					meshes.length
				);

				document.getElementById(
					"info"
				).innerHTML = `Loaded Meshes: ${meshes.length}`;

				for (const m of meshes) {
					document.getElementById("info").innerHTML +=
						"</br>- " + m.filename;
				}
			};
			reader.readAsText(file);
		} else {
			fileElement.textContent = "No file selected";
		}
	});
});

function setColorVariant(meshIndex, colorSet) {
	if (meshes[meshIndex]) {
		const mesh = meshes[meshIndex].mesh;
		const colorSets = meshes[meshIndex].colorSets;

		mesh.geometry.setAttribute(
			"color",
			new THREE.BufferAttribute(colorSets[colorSet], 3)
		);

		mesh.geometry.attributes.color.needsUpdate = true;
	}
}

document.getElementById("camera-reset").addEventListener("click", () => {
	const box = new THREE.Box3().setFromObject(meshes[activeMesh].mesh);
	const boundingSphere = new THREE.Sphere();
	box.getBoundingSphere(boundingSphere);
	const center = boundingSphere.center;
	const radius = boundingSphere.radius;
	camera.position.set(center.x, center.y, center.z + radius * 2.5);
	controls.target.set(center.x, center.y, center.z);
	controls.update();
});

document.getElementById("btn-unipolar").addEventListener("click", () => {
	setColorVariant(0, "unipolar");
});

document.getElementById("btn-bipolar").addEventListener("click", () => {
	setColorVariant(0, "bipolar");
});

document.getElementById("btn-lat").addEventListener("click", () => {
	setColorVariant(0, "lat");
});

document.getElementById("btn-eml").addEventListener("click", () => {
	setColorVariant(0, "eml");
});

document.getElementById("btn-exteml").addEventListener("click", () => {
	setColorVariant(0, "exteml");
});

document.getElementById("btn-scar").addEventListener("click", () => {
	setColorVariant(0, "scar");
});
