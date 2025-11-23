import * as THREE from "three";
import { OrbitControls } from "three/addons/controls/OrbitControls.js";

/////// load shaders ///////
let vShader = null;
let fShader = null;

try {
	document.getElementById("viewport").innerHTML = "Loading shaders...";
	console.log("Loading shaders...");
	[vShader, fShader] = await Promise.all([
		fetch("../glsl/vertex.glsl").then((r) => {
			if (!r.ok) throw new Error("Shader vertex.glsl not found");
			return r.text();
		}),
		fetch("../glsl/fragment.glsl").then((r) => {
			if (!r.ok) throw new Error("Shader fragment.glsl not found");
			return r.text();
		}),
	]);
	console.log("Shaders loaded successfully");
	document.getElementById("viewport").innerHTML = "";
} catch (err) {
	console.error("Error encountered while loading shaders: ", err);
	document.getElementById("viewport").innerHTML =
		"Error encountered while loading shaders: " + err;
}

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

controls.addEventListener("change", () => {
	renderer.render(scene, camera);
});

renderer.render(scene, camera);

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
scene.add(new THREE.AmbientLight(0xffffff, 3));

/////// model upload ///////

const meshes = [];
const qualities = ["unipolar", "bipolar", "lat", "eml", "exteml", "scar"];
let activeMesh = -1;
let activeQuality = document.querySelector(
	'.qualities-container input[name="quality"]:checked'
).value;

const rawMeshElement = document.getElementById("raw-mesh");
const fileElement = document.getElementById("filename");

rawMeshElement.addEventListener("change", function (event) {
	if (event.target.files.length > 0) {
		const file = event.target.files[0];
		processFile(file);
	}
});

["dragenter", "dragover", "dragleave", "drop"].forEach((eventName) => {
	viewport.addEventListener(eventName, (e) => {
		e.preventDefault();
		e.stopPropagation();
	});
});

["dragenter", "dragover"].forEach((eventName) => {
	viewport.addEventListener(eventName, () => {
		viewport.style.opacity = "0.7";
	});
});

["dragleave", "drop"].forEach((eventName) => {
	viewport.addEventListener(eventName, () => {
		viewport.style.opacity = "1";
	});
});

viewport.addEventListener("drop", (e) => {
	const files = e.dataTransfer.files;
	if (files.length > 0) {
		Array.from(files).forEach((file) => {
			processFile(file);
		});
	}
});

function processFile(file) {
	HeartModule().then((cpp) => {
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
				fileElement.innerHTML = "Could not load: " + file.name;
				return;
			}
			fileElement.innerHTML = "Last upload: " + file.name;

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
				let fixTri = new cpp.Triangle(15417, 14398, 14381);
				mesh.triangles.push_back(fixTri);
				mesh.fixNMEdges();
			}

			// -- end fix -- //

			const vertices = mesh.Float32ArrayOfVertices();
			const triangles = mesh.Uint32ArrayOfTriangles();

			const valueSets = {
				unipolar: mesh.Float32ArrayOfUnipolar(),
				bipolar: mesh.Float32ArrayOfBipolar(),
				lat: mesh.Float32ArrayOfLAT(),

				// todo: convert from Int32Array to Float32Array
				groupid: mesh.Int32ArrayOfGroupID(),
				eml: mesh.Int32ArrayOfEML(),
				exteml: mesh.Int32ArrayOfExtEML(),
				scar: mesh.Int32ArrayOfSCAR(),
			};

			const geometry = new THREE.BufferGeometry();
			geometry.setAttribute(
				"position",
				new THREE.BufferAttribute(vertices, 3)
			);
			geometry.setIndex(new THREE.BufferAttribute(triangles, 1));
			geometry.computeVertexNormals();

			geometry.setAttribute(
				"value",
				new THREE.BufferAttribute(valueSets[activeQuality], 1)
			);

			const material = new THREE.ShaderMaterial({
				uniforms: {
					uMin: {
						value: getMin(valueSets[activeQuality]),
					},
					uMax: {
						value: getMax(valueSets[activeQuality]),
					},
					uColorLow: { value: new THREE.Color(0x0000ff) },
					uColorHigh: { value: new THREE.Color(0x00ff00) },
				},
				vertexShader: vShader,
				fragmentShader: fShader,
				side: THREE.DoubleSide,
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

			camera.position.set(center.x, center.y, center.z + radius * 2.5);
			controls.target.set(center.x, center.y, center.z);
			controls.update();

			meshes.push({
				mesh: heart,
				filename: file.name,
				valueSets: valueSets,
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
	});
}

function onViewportResize() {
	camera.aspect = viewport.clientWidth / viewport.clientHeight;
	camera.updateProjectionMatrix();
	renderer.setSize(viewport.clientWidth, viewport.clientHeight);
	renderer.render(scene, camera);
}

function onMouseMove(event) {
	const rect = renderer.domElement.getBoundingClientRect();
	mouse.x = ((event.clientX - rect.left) / rect.width) * 2 - 1;
	mouse.y = -(((event.clientY - rect.top) / rect.height) * 2 - 1);
	vertexPicker();
}

function vertexPicker() {
	if (activeMesh === -1 || !meshes[activeMesh]) {
		document.getElementById("vertex-info").innerHTML = "no mesh loaded";
		return;
	}
	raycaster.setFromCamera(mouse, camera);
	const intersects = raycaster.intersectObject(meshes[activeMesh].mesh);

	if (intersects.length > 0) {
		const firstHit = intersects[0];
		const { point, faceIndex, object } = firstHit;

		const geometry = object.geometry;
		const positions = geometry.attributes.position;
		const index = geometry.index;

		const a = index.array[faceIndex * 3];
		const b = index.array[faceIndex * 3 + 1];
		const c = index.array[faceIndex * 3 + 2];

		const vA = new THREE.Vector3().fromBufferAttribute(positions, a);
		const vB = new THREE.Vector3().fromBufferAttribute(positions, b);
		const vC = new THREE.Vector3().fromBufferAttribute(positions, c);

		vA.applyMatrix4(object.matrixWorld);
		vB.applyMatrix4(object.matrixWorld);
		vC.applyMatrix4(object.matrixWorld);

		const distA = vA.distanceTo(point);
		const distB = vB.distanceTo(point);
		const distC = vC.distanceTo(point);

		let closestVertex = a;
		let minDist = distA;

		if (distB < minDist) {
			closestVertex = b;
			minDist = distB;
		}
		if (distC < minDist) {
			closestVertex = c;
		}

		document.getElementById("vertex-info").innerHTML =
			"<div class='vertex-info'>Unipolar: " +
			meshes[activeMesh].valueSets.unipolar[closestVertex].toFixed(3) +
			"</br>Bipolar: " +
			meshes[activeMesh].valueSets.bipolar[closestVertex].toFixed(3) +
			"</br>LAT: " +
			meshes[activeMesh].valueSets.lat[closestVertex].toFixed(3) +
			"</br>EML: " +
			meshes[activeMesh].valueSets.eml[closestVertex] +
			"</br>ExtEML: " +
			meshes[activeMesh].valueSets.exteml[closestVertex] +
			"</br>SCAR: " +
			meshes[activeMesh].valueSets.scar[closestVertex] +
			"</br>groupID: " +
			meshes[activeMesh].valueSets.groupid[closestVertex] +
			"</div>";
	} else {
		document.getElementById("vertex-info").innerHTML = "";
	}
}

function getMax(array) {
	const len = array.length;
	if (len === 0) {
		return -Infinity;
	}

	let max = array[0];

	for (let i = 1; i < len; i++) {
		if (array[i] > max) {
			max = array[i];
		}
	}
	return max;
}

function getMin(array) {
	const len = array.length;
	if (len === 0) {
		return Infinity;
	}

	let min = array[0];

	for (let i = 1; i < len; i++) {
		if (array[i] < min) {
			min = array[i];
		}
	}
	return min;
}

function setData(meshIndex, dataSet) {
	if (meshes[meshIndex]) {
		const mesh = meshes[meshIndex].mesh;
		const valueSets = meshes[meshIndex].valueSets;
		mesh.geometry.setAttribute(
			"value",
			new THREE.BufferAttribute(valueSets[dataSet], 1)
		);
		renderer.render(scene, camera);
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
			setData(activeMesh, activeQuality);
		}
	});

document
	.querySelector(".meshes-container")
	.addEventListener("change", function (e) {
		if (e.target.name === "loaded-mesh") {
			activeMesh = e.target.value;
			setData(activeMesh, activeQuality);
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
