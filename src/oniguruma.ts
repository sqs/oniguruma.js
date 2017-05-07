import * as nbind from 'nbind';
import * as LibTypes from './lib';

const platform = 'asmjs'; // (process.env.EMSCRIPTEN ? 'asmjs' : 'native');
const binding = nbind.init<typeof LibTypes>('dist/' + platform);
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
}

// tslint:disable-next-line:space-before-function-paren
OnigScanner.prototype.findNextMatchSync = function (s: string | OnigString, startPosition: number = 0): LibTypes.OnigNextMatchResult | null {
	return this.FindNextMatchSync(convertToOnigString(s), convertToPositiveCountableInteger(startPosition));
};

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