import * as nbind from 'nbind';
import * as LibTypes from './lib';

const binding = nbind.init<typeof LibTypes>('dist');
export const lib = binding.lib;

export interface OnigCaptureIndex extends LibTypes.OnigCaptureIndex {
	match?: string;
}

// binding.bind('OnigCaptureIndex', OnigCaptureIndex);

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
		return this.searchSync(convertToOnigString(s)) != null;
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
	return this.FindNextMatchSync(convertToOnigString(s), startPosition);
};

function convertToOnigString(value: any): OnigString {
	if (value instanceof OnigString) { return value; }
	if (typeof value === 'string') { return new OnigString(value); }
	throw new Error('convertToOnigString: invalid value: ' + value);
}

export const OnigString: OnigStringCtor = lib.OnigString as any;

export interface OnigStringCtor {
	new (s: string): OnigString;
	prototype: OnigString;
}

export interface OnigString extends LibTypes.OnigString {
	length: number;
	slice(start?: number, end?: number): string;
	substring(start: number, end?: number): string;
	toString(): string;
}

Object.defineProperty(OnigString.prototype, 'length', {
	// tslint:disable-next-line:object-literal-shorthand space-before-function-paren
	get: function (this: OnigString): number {
		return this.utf8_length();
	},
	configurable: true,
	enumerable: true,
});

// tslint:disable-next-line:space-before-function-paren
OnigString.prototype.slice = function (start?: number, end?: number): string {
	return this.toString().slice(start, end);
};

// tslint:disable-next-line:space-before-function-paren
OnigString.prototype.substring = function (start: number, end?: number): string {
	return this.toString().substring(start, end);
};

// tslint:disable-next-line:space-before-function-paren
OnigString.prototype.toString = function (): string {
	return this.utf8_value()!;
};
