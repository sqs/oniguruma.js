import * as nbind from 'nbind';
import * as LibTypes from './lib';

export const lib = nbind.init<typeof LibTypes>('dist').lib;

// export class OnigRegExp extends lib.OnigRegExp {

// }

// class OnigRegExp {
// 	constructor(source) {
// 		this.source = source.toString();
// 		this.scanner = new OnigScanner([this.source]);
// 	}

// 	captureIndicesForMatch(string, match) {
// 		if (match) {
// 			const { captureIndices } = match;
// 			string = this.scanner.convertToString(string);
// 			for (const capture of Array.from(captureIndices)) {
// 				capture.match = string.slice(capture.start, capture.end);
// 			}
// 			return captureIndices;
// 		} else {
// 			return null;
// 		}
// 	}

// 	searchSync(string, startPosition) {
// 		if (startPosition == null) startPosition = 0;
// 		const match = this.scanner.findNextMatchSync(string, startPosition);
// 		return this.captureIndicesForMatch(string, match);
// 	}

// 	search(string, startPosition, callback) {
// 		if (startPosition == null) { startPosition = 0; }
// 		if (typeof startPosition === 'function') {
// 			callback = startPosition;
// 			startPosition = 0;
// 		}

// 		this.scanner.findNextMatch(string, startPosition, (error, match) => {
// 			callback(error, this.captureIndicesForMatch(string, match));
// 		});
// 	}

// 	testSync(string) {
// 		return this.searchSync(string) != null;
// 	}

// 	test(string, callback) {
// 		this.search(string, 0, (error, result) => callback(error, result != null));
// 	}
// }

// OnigScanner.prototype.findNextMatch = function (string, startPosition, callback) {
// 	if (startPosition == null) startPosition = 0;
// 	if (typeof startPosition === 'function') {
// 		callback = startPosition;
// 		startPosition = 0;
// 	}

// 	string = this.convertToString(string);
// 	startPosition = this.convertToNumber(startPosition);

// 	this._findNextMatch(string, startPosition, (error, match) => {
// 		if (match) match.scanner = this;
// 		return callback(error, match);
// 	});
// };

// OnigScanner.prototype.findNextMatchSync = function (string, startPosition) {
// 	if (startPosition == null) { startPosition = 0; }
// 	string = this.convertToString(string);
// 	startPosition = this.convertToNumber(startPosition);

// 	const match = this._findNextMatchSync(string, startPosition);
// 	if (match) match.scanner = this;
// 	return match;
// };

// OnigScanner.prototype.convertToString = function (value) {
// 	if (value === undefined) return 'undefined';
// 	if (value === null) return 'null';
// 	if (value.constructor === OnigString) return value;
// 	return value.toString();
// };

// OnigScanner.prototype.convertToNumber = function (value) {
// 	value = parseInt(value);
// 	if (!isFinite(value)) { value = 0; }
// 	value = Math.max(value, 0);
// 	return value;
// };

export const OnigString: OnigStringCtor = lib.OnigString as any;

export interface OnigStringCtor {
	new (s: string): OnigString;
	prototype: OnigString;
}

export interface OnigString extends LibTypes.OnigString {
	length: number;
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
OnigString.prototype.substring = function (start: number, end?: number): string {
	return this.utf8_value()!.substring(start, end);
};

// tslint:disable-next-line:space-before-function-paren
OnigString.prototype.toString = function (): string {
	return this.utf8_value()!;
};
