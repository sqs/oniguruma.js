import * as nbind from 'nbind';
import * as LibTypes from './lib';

/**
 * The Emscripten Module configuration. See
 * https://kripken.github.io/emscripten-site/docs/api_reference/module.html for documentation.
 */
export interface EmscriptenModule {
	[key: string]: any;
}

type InitFunc = (Module: EmscriptenModule, cb: InitCallback) => void;

type InitCallback = (err: any, loadedBinding: nbind.Binding<typeof LibTypes>) => void;

/**
 * Called when running in a web browser to initialize the asm.js module and return the binding.
 *
 * @param nbindModule The dist/asmjs/nbind.js module
 * @param Module The Emscripten Module configuration. See
 * https://kripken.github.io/emscripten-site/docs/api_reference/module.html for documentation.
 */
export function init(nbindModule: typeof nbind, Module: EmscriptenModule): nbind.Binding<typeof LibTypes> {
	// When loaded in the browser, this is actually a function, for some reason.
	const nbindInit: InitFunc = nbindModule as any;
	let binding: nbind.Binding<typeof LibTypes> | undefined;
	nbindInit(Module, (err, loadedBinding) => {
		if (err) { throw err; }
		binding = loadedBinding;
	});
	return binding!;
}