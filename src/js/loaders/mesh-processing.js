import * as THREE from "three";
import { setActiveMesh, state } from "../state/state.js";
import { get2Min, getMax } from "../utils/math-utils.js";
import { vShader, fShader, dyn_vShader, dyn_fShader } from "../main.js";
import { scene, camera, controls } from "../main.js";

export function processFile(file) {
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

			geometry.setAttribute(
				"value",
				new THREE.BufferAttribute(valueSets[state.activeQuality], 1),
			);

			const [absMin, min] = get2Min(valueSets[state.activeQuality]);
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
						value: getMax(valueSets[state.activeQuality]),
					},
				},
				vertexShader: vShader,
				fragmentShader: fShader,
				side: THREE.DoubleSide,
			});

			const heart = new THREE.Mesh(geometry, material);

			state.meshes.forEach((meshData) => {
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

			state.meshes.push({
				mesh: heart,
				filename: file.name,
				valueSets: valueSets,
				center: center,
				radius: radius,
			});

			setActiveMesh(state.meshes.length - 1);

			let meshValue = 0;
			document.getElementById("loaded-meshes").innerHTML = "";
			for (const m of state.meshes) {
				let corners = "";
				let checked = "";
				if (state.activeMesh === 0 && state.meshes.length - 1 === 0) {
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
