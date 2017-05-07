import * as nbind from 'nbind';
import * as LibTypes from './lib';

// used only for asmjs
declare var XMLHttpRequest: any;
declare var require: any;

const platform = 'asmjs'; // (process.env.EMSCRIPTEN ? 'asmjs' : 'native');
let binding: nbind.Binding<typeof LibTypes>;
if (nbind.init) {
	binding = nbind.init<typeof LibTypes>('dist/' + platform);
} else {
	const emModule = {
		// tslint:disable-next-line:ban-types
		readAsync: function readAsync(url: string, onload: Function, onerror: Function) {
			if (/\.mem$/.test(url)) {
				onload(require('arraybuffer-loader!../asmjs/nbind.js.mem'));
				return;
			}

			const xhr = new XMLHttpRequest();
			xhr.open('GET', url, true);
			xhr.responseType = 'arraybuffer';
			xhr.onload = function xhr_onload() {
				if (xhr.status === 200 || xhr.status === 0 && xhr.response) {
					onload(xhr.response);
				} else {
					onerror();
				}
			}
				;
			xhr.onerror = onerror;
			xhr.send(null);
		},
		TOTAL_MEMORY: 268435456,
	};
	(nbind as any)(emModule, (err: any, binding2: any) => {
		console.log('from nbind');
		if (err) { console.log('nbind ERR', err); }
		if (binding2) {
			console.log('nbind BINDING', binding2);
			// tslint:disable-next-line:no-debugger
			//debugger;
		}
		binding = binding2;
	});
}
export const lib = binding.lib;

export interface OnigCaptureIndex extends LibTypes.OnigCaptureIndex {
	index: number;
	start: number;
	end: number;
	length: number;
	match?: string; // only populated by OnigRegExp.captureIndicesForMatch
}

class OnigCaptureIndexImpl implements LibTypes.OnigCaptureIndex {
	constructor(
		public index: number,
		public start: number,
		public end: number,
		public length: number,
	) { }

	fromJS(output: (index: number, start: number, end: number, length: number) => void): void {
		output(this.index, this.start, this.end, this.length);
	}
}

binding.bind('OnigCaptureIndex', OnigCaptureIndexImpl);

class OnigNextMatchResult implements LibTypes.OnigNextMatchResult {
	constructor(
		public index: number,
		public captureIndices: LibTypes.OnigCaptureIndex[],
	) { }

	fromJS(output: (index: number, captureIndices: LibTypes.OnigCaptureIndex[]) => void): void {
		output(this.index, this.captureIndices);
	}
}

export class OnigRegExp {
	private scanner: OnigScanner;

	constructor(source: string) {
		this.scanner = new OnigScanner([source]);
	}

	static captureIndicesForMatch(s: string | OnigString, match: LibTypes.OnigNextMatchResult): OnigCaptureIndex[] | null {
		if (match) {
			const { captureIndices }: { captureIndices: OnigCaptureIndex[] } = match;
			const onigString = convertToOnigString(s);
			for (const capture of Array.from(captureIndices)) {
				capture.match = s.slice(capture.start, capture.end);
			}
			return captureIndices;
		}
		return null;
	}

	searchSync(s: string | OnigString, startPosition: number = 0): OnigCaptureIndex[] | null {
		const match = this.scanner.findNextMatchSync(s, startPosition);
		if (match) { return OnigRegExp.captureIndicesForMatch(s, match); }
		return null;
	}

	testSync(s: string | OnigString) {
		return this.searchSync(s) != null;
	}
}

export const OnigScanner: OnigScannerCtor = lib.OnigScanner as any;

export interface OnigScannerCtor {
	new (sources: string[]): OnigScanner;
	prototype: OnigScanner;
}

export interface OnigScanner extends LibTypes.OnigScanner {
	findNextMatchSync(s: string | OnigString, startPosition: number): LibTypes.OnigNextMatchResult | null;

	// vscode-textmate's oniguruma.d.ts wants to call _findNextMatchSync (with an
	// underscore) for some reason.
	_findNextMatchSync(s: string | OnigString, startPosition: number): LibTypes.OnigNextMatchResult | null;
}

// tslint:disable-next-line:space-before-function-paren
OnigScanner.prototype.findNextMatchSync = function (s: string | OnigString, startPosition: number = 0): LibTypes.OnigNextMatchResult | null {
	return this.FindNextMatchSync(convertToOnigString(s), convertToPositiveCountableInteger(startPosition));
};

OnigScanner.prototype._findNextMatchSync = OnigScanner.prototype.findNextMatchSync;

function convertToPositiveCountableInteger(value: any): number {
	value = parseInt(value, 10);
	if (!isFinite(value)) { value = 0; }
	value = Math.max(value, 0);
	return value;
}

function convertToOnigString(value: any): LibTypes.OnigString {
	if (value instanceof OnigString) { return getOnigStringPointer(value); }
	if (typeof value === 'string') { return getOnigStringPointer(new OnigString(value)); }
	throw new Error('convertToOnigString: invalid value: ' + value);
}

/**
 * getOnigStringPointer returns an OnigString's C pointer. The pointer field is
 * intentionally private in the exported class definition.
 */
function getOnigStringPointer(s: OnigString): LibTypes.OnigString {
	return (s as any).ptr;
}

export class OnigString {
	private ptr: LibTypes.OnigString;

	constructor(private value: string) {
		if (typeof value !== 'string') {
			throw new Error('OnigString: invalid non-string value of type ' + typeof value);
		}
		this.ptr = new lib.OnigString(value);
	}

	// $str is the (private, undocumented) property name used by
	// node-oniguruma. Reuse it so this class's instances are compatible.
	get $str(): string { return this.value; }
	set $str(_value: string) {
		if (this.value !== _value) {
			throw new Error('OnigString: unable to update immutable value');
		}
	}

	get length(): number { return this.value.length; }
	slice(start?: number, end?: number): string { return this.value.slice(start, end); }
	substring(start: number, end?: number): string { return this.value.substring(start, end); }
	toString(): string { return this.value; }
}