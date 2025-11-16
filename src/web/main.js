import * as THREE from "three";
import { OrbitControls } from "three/addons/controls/OrbitControls.js";

/////// three.js ///////

var scene = new THREE.Scene();
var viewport = document.getElementById("viewport");
var camera = new THREE.PerspectiveCamera(
	50,
	viewport.clientWidth / viewport.clientHeight
);
var renderer = new THREE.WebGLRenderer({ alpha: true });
var mouse = new THREE.Vector2();
var raycaster = new THREE.Raycaster();

renderer.setSize(viewport.clientWidth, viewport.clientHeight);
const controls = new OrbitControls(camera, renderer.domElement);
renderer.setAnimationLoop(animate);

viewport.append(renderer.domElement);

window.addEventListener("resize", onViewportResize);

window.addEventListener("mousemove", onMouseMove, false);

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

/////// model upload ///////

const meshes = [];
const qualities = ["unipolar", "bipolar", "lat", "eml", "exteml", "scar"];
let activeMesh = -1;
let activeQuality = document.querySelector(
	'.qualities-container input[name="quality"]:checked'
).value;

HeartModule().then((cpp) => {
	const rawMeshElement = document.getElementById("raw-mesh");
	const fileElement = document.getElementById("filename");

	rawMeshElement.addEventListener("change", function (event) {
		if (event.target.files.length > 0) {
			const file = event.target.files[0];

			if (meshes.some((item) => item.filename === file.name)) {
				console.log("Mesh already uploaded");
				return;
			}

			const reader = new FileReader();

			reader.onload = function (e) {
				const fileContent = e.target.result;
				let mesh;
				try {
					mesh = cpp.importMesh(fileContent);
				} catch (e) {
					console.error("Error: ", e.message);
					fileElement.innerHTML = "Could not load: <br/>" + file.name;
					return;
				}
				fileElement.innerHTML = "Last upload: <br/>" + file.name;

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
					new THREE.BufferAttribute(turboSets[activeQuality], 3)
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
					center: center,
					radius: radius,
				});

				activeMesh = meshes.length - 1;

				let meshValue = 0;
				document.getElementById("loaded-meshes").innerHTML = "";
				for (const m of meshes) {
					let corners = "";
					let checked = "";
					if (activeMesh === 0 && meshes.length - 1 === 0) {
						corners = " class='mesh-top mesh-bottom' ";
						checked = "checked";
					} else if (meshValue === 0) {
						corners = " class='mesh-top' ";
					} else if (activeMesh === meshes.length - 1) {
						corners = " class='mesh-bottom' ";
						checked = "checked";
					}
					document.getElementById("loaded-meshes").innerHTML +=
						"<label" +
						corners +
						">" +
						"<input type='radio' name='loaded-mesh' value='" +
						meshValue +
						"' " +
						checked +
						"/>" +
						"<span>" +
						m.filename +
						"</span>" +
						"</label¨>";
					meshValue++;
				}

				console.log(
					"Mesh loaded successfully. Meshes loaded:",
					meshes.length
				);
			};
			reader.readAsText(file);
		}
	});
});

function animate() {
	controls.update();
	renderer.render(scene, camera);
}

function onViewportResize() {
	camera.aspect = viewport.clientWidth / viewport.clientHeight;
	camera.updateProjectionMatrix();
	renderer.setSize(viewport.clientWidth, viewport.clientHeight);
}

function onMouseMove(event) {
	const rect = renderer.domElement.getBoundingClientRect();
	mouse.x = ((event.clientX - rect.left) / rect.width) * 2 - 1;
	mouse.y = -(((event.clientY - rect.top) / rect.height) * 2 - 1);
	vertexPicker();
}

function vertexPicker() {
	raycaster.setFromCamera(mouse, camera);
	const intersects = raycaster.intersectObjects(scene.children);
	if (intersects.length > 0) {
		const face = intersects[0].face;
		const vertexIndices = [face.a, face.b, face.c];
		console.log(vertexIndices);
	}
}

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

document
	.querySelector(".qualities-container")
	.addEventListener("change", function (e) {
		if (e.target.name === "quality") {
			activeQuality = e.target.value;
			setColorVariant(activeMesh, activeQuality);
		}
	});

document
	.querySelector(".meshes-container")
	.addEventListener("change", function (e) {
		if (e.target.name === "loaded-mesh") {
			activeMesh = e.target.value;
			setColorVariant(activeMesh, activeQuality);
			for (let i = 0; i < meshes.length; i++) {
				if (i != activeMesh) {
					meshes[i].mesh.visible = false;
				} else {
					meshes[i].mesh.visible = true;
				}
			}
			camera.position.set(
				meshes[activeMesh].center.x,
				meshes[activeMesh].center.y,
				meshes[activeMesh].center.z + meshes[activeMesh].radius * 2.5
			);
			controls.target.set(
				meshes[activeMesh].center.x,
				meshes[activeMesh].center.y,
				meshes[activeMesh].center.z
			);
			controls.update();
		}
	});
