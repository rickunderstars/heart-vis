export function getMax(array) {
	let max = -Infinity;

	for (let i = 0; i < array.length; i++) {
		if (array[i] > max) {
			max = array[i];
		}
	}
	return max;
}

export function get2Min(array) {
	let absMin = Infinity;
	let min = Infinity;

	const valSet = new Set(array);
	const moreThan2 = valSet.size > 2;

	for (let i = 0; i < array.length; i++) {
		if (array[i] < absMin) {
			min = absMin;
			absMin = array[i];
		} else if (array[i] < min && array[i] > absMin) {
			min = array[i];
		}
	}

	if (!moreThan2) {
		min = absMin;
	}

	return [absMin, min];
}
