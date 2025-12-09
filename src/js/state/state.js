export const state = {
	meshes: [],
	activeMesh: -1,
	activeQuality: "unipolar",
	timeMode: false,
};

export function getActiveMesh() {
	return state.meshes[state.activeMesh] ?? null;
}

export function setActiveMesh(index) {
	state.activeMesh = index;
}

export function setActiveQuality(quality) {
	state.activeQuality = quality;
}
