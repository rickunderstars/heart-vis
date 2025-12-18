import { updateActiveMaterial } from "./material-update";

export async function loadShaders() {
	const timestamp = Date.now();

	const loadShader = async (path) => {
		const response = await fetch(`${path}?t=${timestamp}`);
		return await response.text();
	};

	const [vShader, fShader, dynVShader, dynFShader] = await Promise.all([
		loadShader("/glsl/static-vertex.glsl"),
		loadShader("/glsl/static-fragment.glsl"),
		loadShader("/glsl/dynamic-vertex.glsl"),
		loadShader("/glsl/dynamic-fragment.glsl"),
	]);

	return { vShader, fShader, dynVShader, dynFShader };
}

export async function reloadShaderMaterial(state) {
	const shaders = await loadShaders();

	updateActiveMaterial({ state, shaders });
	return;
}
