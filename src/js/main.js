import * as THREE from "three";
import { OrbitControls } from "three/addons/controls/OrbitControls.js";
import { getMax, get2Min } from "./utils/math-utils.js";
import { initScene } from "./core/scene.js";
import { createRenderer } from "./core/renderer.js";
/////// load shaders ///////
let vShader = null;
let fShader = null;
let dyn_vShader = null;
let dyn_fShader = null;

try {
	document.getElementById("viewport").innerHTML = "Loading shaders...";
	console.log("Loading shaders...");
	[vShader, fShader, dyn_vShader, dyn_fShader] = await Promise.all([
		fetch("../glsl/static-vertex.glsl").then((r) => {
			if (!r.ok) throw new Error("Shader static-vertex.glsl not found");
			return r.text();
		}),
		fetch("../glsl/static-fragment.glsl").then((r) => {
			if (!r.ok) throw new Error("Shader static-fragment.glsl not found");
			return r.text();
		}),
		fetch("../glsl/dynamic-lat-vertex.glsl").then((r) => {
			if (!r.ok)
				throw new Error("Shader dynamic-lat-vertex.glsl not found");
			return r.text();
		}),
		fetch("../glsl/dynamic-lat-fragment.glsl").then((r) => {
			if (!r.ok)
				throw new Error("Shader dynamic-lat-fragment.glsl not found");
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

var scene = initScene();
var viewport = document.getElementById("viewport");
var camera = new THREE.PerspectiveCamera(
	50,
	viewport.clientWidth / viewport.clientHeight,
);
var renderer = createRenderer(viewport);
var mouse = new THREE.Vector2();
var raycaster = new THREE.Raycaster();

const controls = new OrbitControls(camera, renderer.domElement);

let timeMode = false;

controls.addEventListener("change", () => {
	if (!timeMode) {
		renderer.render(scene, camera);
	}
});

if (!timeMode) {
	renderer.render(scene, camera);
}

viewport.append(renderer.domElement);

window.addEventListener("resize", onViewportResize);

window.addEventListener("mousemove", onMouseMove, false);

camera.position.z = 5;

const clock = new THREE.Clock();

/////// model upload ///////

const meshes = [];
const qualities = [
	"unipolar",
	"bipolar",
	"lat",
	"eml",
	"exteml",
	"scar",
	"groupid",
];

let activeMesh = -1;

if (timeMode) {
	latAnimate();
}

let activeQuality = document.querySelector(
	'.qualities-container input[name="quality"]:checked',
).value;

const rawMeshElement = document.getElementById("raw-mesh");
const fileElement = document.getElementById("filename");

rawMeshElement.addEventListener("change", function (e) {
	if (e.target.files.length > 0) {
		const file = e.target.files[0];
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

let then = 0;
const fps = 120;
const interval = 1000 / fps;

function latAnimate(timeStamp) {
	if (!timeMode) {
		return;
	}

	requestAnimationFrame(latAnimate);

	if (!then) then = timeStamp;

	const delta = timeStamp - then;

	if (delta > interval) {
		then = timeStamp - (delta % interval);

		if (activeMesh !== -1 && meshes[activeMesh]) {
			meshes[activeMesh].mesh.material.uniforms.uTime.value =
				clock.getElapsedTime();
		}

		renderer.render(scene, camera);
	}
}
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
				groupid: mesh.Float32ArrayOfGroupID(),
				eml: mesh.Float32ArrayOfEML(),
				exteml: mesh.Float32ArrayOfExtEML(),
				scar: mesh.Float32ArrayOfSCAR(),
			};

			const geometry = new THREE.BufferGeometry();
			geometry.setAttribute(
				"position",
				new THREE.BufferAttribute(vertices, 3),
			);
			geometry.setIndex(new THREE.BufferAttribute(triangles, 1));
			geometry.computeVertexNormals();

			geometry.setAttribute(
				"value",
				new THREE.BufferAttribute(valueSets[activeQuality], 1),
			);

			const [absMin, min] = get2Min(valueSets[activeQuality]);
			const material = new THREE.ShaderMaterial({
				uniforms: {
					uOnlyTwo: {
						value: absMin - min == 0 ? 1.0 : 0.0,
					},
					uAbsMin: {
						value: absMin,
					},
					uMin: {
						value: min,
					},
					uMax: {
						value: getMax(valueSets[activeQuality]),
					},
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
				meshes.length,
			);
		};
		reader.readAsText(file);
	});
}

function onViewportResize() {
	camera.aspect = viewport.clientWidth / viewport.clientHeight;
	camera.updateProjectionMatrix();
	renderer.setSize(viewport.clientWidth, viewport.clientHeight);
	if (!timeMode) {
		renderer.render(scene, camera);
	}
}

function onMouseMove(e) {
	const rect = renderer.domElement.getBoundingClientRect();
	mouse.x = ((e.clientX - rect.left) / rect.width) * 2 - 1;
	mouse.y = -(((e.clientY - rect.top) / rect.height) * 2 - 1);
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
		const face = firstHit.face;
		const geometry = firstHit.object.geometry;
		const positions = geometry.attributes.position;
		const object = firstHit.object;

		const v0 = new THREE.Vector3().fromBufferAttribute(positions, face.a);
		const v1 = new THREE.Vector3().fromBufferAttribute(positions, face.b);
		const v2 = new THREE.Vector3().fromBufferAttribute(positions, face.c);

		v0.applyMatrix4(object.matrixWorld);
		v1.applyMatrix4(object.matrixWorld);
		v2.applyMatrix4(object.matrixWorld);

		const bary = new THREE.Vector3();
		THREE.Triangle.getBarycoord(firstHit.point, v0, v1, v2, bary);

		const active = meshes[activeMesh].valueSets;

		const unipolar =
			active.unipolar[face.a] * bary.x +
			active.unipolar[face.b] * bary.y +
			active.unipolar[face.c] * bary.z;

		const bipolar =
			active.bipolar[face.a] * bary.x +
			active.bipolar[face.b] * bary.y +
			active.bipolar[face.c] * bary.z;

		const lat =
			active.lat[face.a] * bary.x +
			active.lat[face.b] * bary.y +
			active.lat[face.c] * bary.z;

		let intValuesIndex = face.a;
		if (bary.y > bary.x && bary.y > bary.z) {
			intValuesIndex = face.b;
		} else if (bary.z > bary.x && bary.z > bary.y) {
			intValuesIndex = face.c;
		}

		const eml = active.eml[intValuesIndex];
		const exteml = active.exteml[intValuesIndex];
		const scar = active.scar[intValuesIndex];
		const groupid = active.groupid[intValuesIndex];

		document.getElementById("vertex-info").innerHTML =
			"<div class='vertex-info'>Unipolar: " +
			unipolar.toFixed(3) +
			"</br>Bipolar: " +
			bipolar.toFixed(3) +
			"</br>LAT: " +
			lat.toFixed(3) +
			"</br>EML: " +
			eml +
			"</br>ExtEML: " +
			exteml +
			"</br>SCAR: " +
			scar +
			"</br>groupID: " +
			groupid +
			"</div>";
	} else {
		document.getElementById("vertex-info").innerHTML = "";
	}
}

function setData(meshIndex, dataSet) {
	if (meshes[meshIndex] && !timeMode) {
		const mesh = meshes[meshIndex].mesh;
		const valueSets = meshes[meshIndex].valueSets;
		const [absMin, min] = get2Min(valueSets[dataSet]);

		mesh.material.uniforms.uOnlyTwo.value = absMin - min == 0 ? 1.0 : 0.0;
		mesh.material.uniforms.uAbsMin.value = absMin;
		mesh.material.uniforms.uMin.value = min;
		mesh.material.uniforms.uMax.value = getMax(valueSets[dataSet]);

		mesh.geometry.deleteAttribute("value");
		mesh.geometry.setAttribute(
			"value",
			new THREE.BufferAttribute(valueSets[dataSet], 1),
		);

		renderer.render(scene, camera);
	} else {
		const meshData = meshes[activeMesh];
		const currentMesh = meshData.mesh;
		const valueSets = meshData.valueSets;
		timeMode = false;

		const [absMin, min] = get2Min(valueSets[activeQuality]);

		currentMesh.material = new THREE.ShaderMaterial({
			uniforms: {
				uOnlyTwo: { value: absMin - min == 0 ? 1.0 : 0.0 },
				uAbsMin: { value: absMin },
				uMin: { value: min },
				uMax: { value: getMax(valueSets[activeQuality]) },
			},
			vertexShader: vShader,
			fragmentShader: fShader,
			side: THREE.DoubleSide,
		});

		setData(activeMesh, activeQuality);
	}
}

function cameraReset() {
	const center = meshes[activeMesh].center;
	const radius = meshes[activeMesh].radius;
	camera.position.set(center.x, center.y, center.z + radius * 2.5);
	controls.target.set(center.x, center.y, center.z);
	controls.update();
}

document.getElementById("camera-reset").addEventListener("click", () => {
	cameraReset();
});

document.addEventListener("keydown", (k) => {
	if (k.key === "r") {
		cameraReset();
	}
});

document.getElementById("dynamic-lat").addEventListener("click", () => {
	if (activeMesh === -1 || !meshes[activeMesh]) return;

	const meshData = meshes[activeMesh];
	const currentMesh = meshData.mesh;
	const valueSets = meshData.valueSets;
	const lat = valueSets["lat"];

	if (currentMesh.material) {
		currentMesh.material.dispose();
	}

	if (!timeMode) {
		timeMode = true;

		clock.start();
		then = 0;

		then = 0;

		latAnimate();

		currentMesh.geometry.deleteAttribute("value");
		currentMesh.geometry.setAttribute(
			"value",
			new THREE.BufferAttribute(lat, 1),
		);

		const [absMin, min] = get2Min(lat);

		currentMesh.material = new THREE.ShaderMaterial({
			uniforms: {
				uAbsMin: { value: absMin },
				uMin: { value: min },
				uMax: { value: getMax(lat) },
				uTime: { value: 0 },
			},
			vertexShader: dyn_vShader,
			fragmentShader: dyn_fShader,
			side: THREE.DoubleSide,
		});
	} else {
		timeMode = false;

		const [absMin, min] = get2Min(valueSets[activeQuality]);

		currentMesh.material = new THREE.ShaderMaterial({
			uniforms: {
				uOnlyTwo: { value: absMin - min == 0 ? 1.0 : 0.0 },
				uAbsMin: { value: absMin },
				uMin: { value: min },
				uMax: { value: getMax(valueSets[activeQuality]) },
			},
			vertexShader: vShader,
			fragmentShader: fShader,
			side: THREE.DoubleSide,
		});

		setData(activeMesh, activeQuality);
	}
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
			if (!timeMode) {
				setData(activeMesh, activeQuality);
			} else {
				const meshData = meshes[activeMesh];
				const currentMesh = meshData.mesh;
				const valueSets = meshData.valueSets;
				const lat = valueSets["lat"];
				clock.start();
				then = 0;

				then = 0;

				latAnimate();

				currentMesh.geometry.deleteAttribute("value");
				currentMesh.geometry.setAttribute(
					"value",
					new THREE.BufferAttribute(lat, 1),
				);

				const [absMin, min] = get2Min(lat);

				currentMesh.material = new THREE.ShaderMaterial({
					uniforms: {
						uAbsMin: { value: absMin },
						uMin: { value: min },
						uMax: { value: getMax(lat) },
						uTime: { value: 0 },
					},
					vertexShader: dyn_vShader,
					fragmentShader: dyn_fShader,
					side: THREE.DoubleSide,
				});
			}
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
				meshes[activeMesh].center.z + meshes[activeMesh].radius * 2.5,
			);
			controls.target.set(
				meshes[activeMesh].center.x,
				meshes[activeMesh].center.y,
				meshes[activeMesh].center.z,
			);
			controls.update();
		}
	});
