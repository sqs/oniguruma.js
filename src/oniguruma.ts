import * as nbind from 'nbind';
import * as LibTypes from './lib';
import { init as asmjsInit } from './asmjs';

const platform = 'asmjs'; // (process.env.EMSCRIPTEN ? 'asmjs' : 'native');
let binding: nbind.Binding<typeof LibTypes>;
if (nbind.init) {
	binding = nbind.init<typeof LibTypes>('dist/' + platform);
} else {
	binding = asmjsInit(nbind, {
		TOTAL_MEMORY: 16 * 1024 * 1024 * 8,
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
}

// tslint:disable-next-line:space-before-function-paren
OnigScanner.prototype.findNextMatchSync = function (s: string | OnigString, startPosition: number = 0): LibTypes.OnigNextMatchResult | null {
	return this.FindNextMatchSync(convertToOnigString(s) as any, convertToPositiveCountableInteger(startPosition));
};

OnigScanner.prototype._findNextMatchSync = OnigScanner.prototype.findNextMatchSync;

function convertToPositiveCountableInteger(value: any): number {
	value = parseInt(value, 10);
	if (!isFinite(value)) { value = 0; }
	value = Math.max(value, 0);
	return value;
}

function convertToOnigString(value: any): OnigString {
	if (value instanceof OnigString) { return value; }
	if (typeof value === 'string') { return new OnigString(value, 0); }
	throw new Error('convertToOnigString: invalid value: ' + value);
}

export class OnigString {
	constructor(private value: string, x: number = 0) {
		if (typeof value !== 'string') {
			throw new Error('OnigString: invalid non-string value of type ' + typeof value);
		}
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

	fromJS(output: (value: string, x: number) => void): void {
		console.count('OnigString fromJS');
		output(this.value, 0);
	}
}

binding.bind('OnigString', OnigString);