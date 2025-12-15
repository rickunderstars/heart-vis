class State {
	meshes = [];
	activeMesh = -1;
	activeQuality = "unipolar";
	timeMode = false;
	ambientLightIntensity = 0.6;

	getActiveMesh() {
		return this.meshes[this.activeMesh] ?? null;
	}

	setActiveMesh(index) {
		this.activeMesh = index;
	}

	setActiveQuality(quality) {
		this.activeQuality = quality;
	}

	toggleTimeMode() {
		state.timeMode = !state.timeMode;
	}

	setAmbientLightIntensity(intensity) {
		if (intensity > 1) {
			intensity = 1;
		} else if (intensity < 0) {
			intensity = 0;
		}
		this.getActiveMesh().mesh.material.uniforms.uAmbientLightIntensity.value =
			intensity;
	}
}

const state = new State();

export default state;
