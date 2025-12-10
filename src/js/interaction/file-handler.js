import * as THREE from "three";
import { vShader, fShader, dyn_vShader, dyn_fShader } from "../main.js";
import { updateActiveMaterial } from "../visualization/material-switch.js";

export function setupFileHandlers(dependencies) {
	const { state, scene, camera, controls, viewport, renderer } = dependencies;

	function processFile(file) {
		const fileElement = document.getElementById("filename");
		HeartModule().then((cpp) => {
			if (state.meshes.some((item) => item.filename === file.name)) {
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

				/// -- test state.meshes fixes -- ///

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

				const material = new THREE.MeshBasicMaterial();

				const heart = new THREE.Mesh(geometry, material);

				const box = new THREE.Box3().setFromObject(heart);
				const boundingSphere = new THREE.Sphere();
				box.getBoundingSphere(boundingSphere);
				const center = boundingSphere.center;
				const radius = boundingSphere.radius;

				camera.position.set(
					center.x,
					center.y,
					center.z + radius * 2.5,
				);
				controls.target.set(center.x, center.y, center.z);
				controls.update();

				state.meshes.push({
					mesh: heart,
					filename: file.name,
					valueSets: valueSets,
					center: center,
					radius: radius,
				});

				state.setActiveMesh(state.meshes.length - 1);

				updateActiveMaterial(state);

				state.meshes.forEach((meshData) => {
					meshData.mesh.visible = false;
				});
				scene.add(heart);

				heart.visible = true;

				renderer.render(scene, camera);

				let meshValue = 0;
				document.getElementById("loaded-meshes").innerHTML = "";
				for (const m of state.meshes) {
					let corners = "";
					let checked = "";
					if (
						state.activeMesh === 0 &&
						state.meshes.length - 1 === 0
					) {
						corners = " class='mesh-top mesh-bottom' ";
						checked = "checked";
					} else if (meshValue === 0) {
						corners = " class='mesh-top' ";
					} else if (state.activeMesh === state.meshes.length - 1) {
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
					"Mesh loaded successfully. state.meshes loaded:",
					state.meshes.length,
				);
			};
			reader.readAsText(file);
		});
	}

	document
		.getElementById("raw-mesh")
		.addEventListener("change", function (e) {
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
}
