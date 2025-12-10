class State {
	meshes = [];
	activeMesh = -1;
	activeQuality = "unipolar";
	timeMode = false;

	getActiveMesh() {
		return this.meshes[this.activeMesh] ?? null;
	}

	setActiveMesh(index) {
		this.activeMesh = index;
	}

	setActiveQuality(quality) {
		this.activeQuality = quality;
	}
}

const state = new State();

export default state;
