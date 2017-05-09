import * as nbind from 'nbind';
import * as LibTypes from './lib';
import { init as asmjsInit } from './asmjs';

const platform = 'asmjs'; // (process.env.EMSCRIPTEN ? 'asmjs' : 'native');
let binding: nbind.Binding<typeof LibTypes>;
if (nbind.init) {
	binding = nbind.init<typeof LibTypes>('dist/' + platform);
} else {
	const Module = {
		TOTAL_MEMORY: 16 * 1024 * 1024 * 2,
	};
	binding = asmjsInit(nbind, Module);
}
export const lib = binding.lib;

// binding.toggleLightGC(true);

export interface OnigCaptureIndex extends LibTypes.OnigCaptureIndex {
	index: number;
	start: number;
	end: number;
	length: number;
	match?: string; // only populated by OnigRegExp.captureIndicesForMatch
}

export class OnigCaptureIndexImpl implements LibTypes.OnigCaptureIndex {
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

binding.bind('OnigNextMatchResult', OnigNextMatchResult);

export class OnigRegExp {
	// TODO(sqs): Call OnigString.prototype.free when needed for OnigStrings owned by this class.

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
		s = convertToOnigString(s);
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

	dispose(): void;
}

// tslint:disable-next-line:space-before-function-paren
OnigScanner.prototype.findNextMatchSync = function (s: string | OnigString, startPosition: number = 0): OnigNextMatchResult | null {
	const os: LibTypes.OnigString = convertToOnigString(s) as any;
	const result = this.FindNextMatchSync(os, convertToPositiveCountableInteger(startPosition));
	if (typeof s === 'string') {
		os.free!();
	}
	if (result) {
		const matchResult = new OnigNextMatchResult(result.index, result.captureIndices.map(c => ({
			index: c.index,
			start: c.start,
			end: c.end,
			length: c.length,
		})));
		result.free!();
		return matchResult;
	}
	return null;
};

OnigScanner.prototype._findNextMatchSync = OnigScanner.prototype.findNextMatchSync;
OnigScanner.prototype.dispose = OnigScanner.prototype.free!;

function convertToPositiveCountableInteger(value: any): number {
	value = parseInt(value, 10);
	if (!isFinite(value)) { value = 0; }
	value = Math.max(value, 0);
	return value;
}

function convertToOnigString(value: any): OnigString {
	if (value instanceof OnigString) { return value; }
	return new OnigString(value ? value.toString() : value);
}

// export const OnigString: OnigStringCtor = lib.OnigString as any;

export class OnigString extends lib.OnigString {
	constructor(value: string) {
		super(encodeStringAsUCS2Array(value));
	}
}

export interface OnigStringCtor {
	new (value: string): OnigString;
	prototype: OnigString;
}

export interface OnigString extends LibTypes.OnigString {
	length: number;
	slice(start?: number, end?: number): string;
	substring(start: number, end?: number): string;
	toString(): string;
	dispose(): void;
}

Object.defineProperty(lib.OnigString.prototype, 'length', {
	// tslint:disable-next-line:object-literal-shorthand space-before-function-paren
	get: function (this: OnigString): number {
		return this.utf8_length();
	},
});
// tslint:disable-next-line:space-before-function-paren
OnigString.prototype.slice = function (this: OnigString, start?: number, end?: number): string {
	return this.toString().slice(start, end);
};
// tslint:disable-next-line:space-before-function-paren
OnigString.prototype.substring = function (this: OnigString, start: number, end?: number): string {
	return this.toString().substring(start, end);
};
// tslint:disable-next-line:space-before-function-paren
OnigString.prototype.toString = function (this: OnigString): string {
	return this.utf8_value()!;
};
OnigString.prototype.dispose = OnigString.prototype.free!;

function encodeStringAsUCS2Array(str: string): ArrayBuffer {
	const buf = new ArrayBuffer(str.length * 2);
	const bufView = new Uint16Array(buf);
	for (let i = 0, strLen = str.length; i < strLen; i++) {
		bufView[i] = str.charCodeAt(i);
	}
	return buf;
}