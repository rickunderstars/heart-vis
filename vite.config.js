import { defineConfig } from "vite";
import glsl from "vite-plugin-glsl";
import { resolve } from "path";
import { fileURLToPath } from "url";

const __dirname = fileURLToPath(new URL(".", import.meta.url));

export default defineConfig({
	root: "src",
	plugins: [glsl()],
	resolve: {
		alias: {
			"@js": resolve(__dirname, "src/js"),
			"@glsl": resolve(__dirname, "src/glsl"),
			"@css": resolve(__dirname, "src/css"),
			"@assets": resolve(__dirname, "src/assets"),
			"@": resolve(__dirname, "src"),
		},
	},
	server: {
		port: 3000,
		open: true,
		host: true,
	},
});
