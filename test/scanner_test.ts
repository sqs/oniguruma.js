import * as assert from 'assert';
import { OnigScanner } from '../src/oniguruma';

describe('OnigScanner', () => {
	describe('::findNextMatchSync', () => {
		it('returns the index of the matching pattern', () => {
			const scanner = new OnigScanner(['a', 'b', 'c']);
			assert.equal(scanner.findNextMatchSync('x', 0), null);
			assert.equal(scanner.findNextMatchSync('xxaxxbxxc', 0)!.index, 0);
			assert.equal(scanner.findNextMatchSync('xxaxxbxxc', 4)!.index, 1);
			assert.equal(scanner.findNextMatchSync('xxaxxbxxc', 7)!.index, 2);
			assert.equal(scanner.findNextMatchSync('xxaxxbxxc', 9), null);
		});

		describe('when the string searched contains unicode characters', () =>
			it('returns the correct matching pattern', () => {
				let scanner = new OnigScanner(['1', '2']);
				let match = scanner.findNextMatchSync('ab…cde21', 5)!;
				assert.equal(match.index, 1);

				scanner = new OnigScanner(['\"']);
				match = scanner.findNextMatchSync('{"…": 1}', 1)!;
				assert.deepEqual(match.captureIndices, [{ index: 0, start: 1, end: 2, length: 1 }]);
			}));

		describe('when the string searched contains surrogate pairs', () =>
			it('counts paired characters as 2 characters in both arguments and return values', () => {
				const scanner = new OnigScanner(['Y', 'X']);
				let match = scanner.findNextMatchSync('a💻bYX', 0)!;
				assert.deepEqual(match.captureIndices, [{ index: 0, start: 4, end: 5, length: 1 }]);

				match = scanner.findNextMatchSync('a💻bYX', 1)!;
				assert.deepEqual(match.captureIndices, [{ index: 0, start: 4, end: 5, length: 1 }]);

				match = scanner.findNextMatchSync('a💻bYX', 3)!;
				assert.deepEqual(match.captureIndices, [{ index: 0, start: 4, end: 5, length: 1 }]);

				match = scanner.findNextMatchSync('a💻bYX', 4)!;
				assert.deepEqual(match.captureIndices, [{ index: 0, start: 4, end: 5, length: 1 }]);

				match = scanner.findNextMatchSync('a💻bYX', 5)!;
				assert.equal(match.index, 1);
				assert.deepEqual(match.captureIndices, [{ index: 0, start: 5, end: 6, length: 1 }]);
			}));

		it("returns false when the input string isn't a string", () => {
			const scanner = new OnigScanner(['1']);
			assert.throws(() => (scanner.findNextMatchSync as any)(), /Type mismatch/);
			assert.throws(() => (scanner.findNextMatchSync as any)(null), /Type mismatch/);
			assert.throws(() => (scanner.findNextMatchSync as any)(undefined), /Type mismatch/);
			assert.equal((scanner.findNextMatchSync as any)(2), null);
			assert.equal((scanner.findNextMatchSync as any)(false), null);
		});

		it("uses 0 as the start position when the input start position isn't a valid number", () => {
			const scanner = new OnigScanner(['1']);
			assert.equal(scanner.findNextMatchSync('a1', Infinity)!.index, 0);
			assert.equal(scanner.findNextMatchSync('a1', -1)!.index, 0);
			assert.equal(scanner.findNextMatchSync('a1', false as any)!.index, 0);
			assert.equal(scanner.findNextMatchSync('a1', 'food' as any)!.index, 0);
		});
	});

	describe('when the regular expression contains double byte characters', () =>
		it('returns the correct match length', () => {
			const scanner = new OnigScanner(['Возврат']);
			const match = scanner.findNextMatchSync('Возврат long_var_name;', 0)!;
			assert.deepEqual(match.captureIndices, [{ index: 0, start: 0, end: 7, length: 7 }]);
		}));

	describe('when the input string contains invalid surrogate pairs', () =>
		it.only('interprets them as a code point', () => {
			console.log('--------------------------------------------');
			const scanner = new OnigScanner(['X']);
			// let match = scanner.findNextMatchSync(`X${String.fromCharCode(0xd83c)}X`, 0)!;
			// assert.deepEqual(match.captureIndices, [{ index: 0, start: 0, end: 1, length: 1 }]);

			// match = scanner.findNextMatchSync(`X${String.fromCharCode(0xd83c)}X`, 1)!;
			// assert.deepEqual(match.captureIndices, [{ index: 0, start: 2, end: 3, length: 1 }]);

			// match = scanner.findNextMatchSync(`X${String.fromCharCode(0xd83c)}X`, 2)!;
			// assert.deepEqual(match.captureIndices, [{ index: 0, start: 2, end: 3, length: 1 }]);

			// match = scanner.findNextMatchSync(`X${String.fromCharCode(0xdfff)}X`, 0)!;
			// assert.deepEqual(match.captureIndices, [{ index: 0, start: 0, end: 1, length: 1 }]);

			// match = scanner.findNextMatchSync(`X${String.fromCharCode(0xdfff)}X`, 1)!;
			// assert.deepEqual(match.captureIndices, [{ index: 0, start: 2, end: 3, length: 1 }]);

			// match = scanner.findNextMatchSync(`X${String.fromCharCode(0xdfff)}X`, 2)!;
			// assert.deepEqual(match.captureIndices, [{ index: 0, start: 2, end: 3, length: 1 }]);

			let match = scanner.findNextMatchSync(
				`X${String.fromCharCode(0xd800)}${String.fromCharCode(0xdc00)}X`, 2)!;
			assert.deepEqual(match.captureIndices, [{ index: 0, start: 3, end: 4, length: 1 }]);


			// These are actually valid, just testing the min & max
			// let match = scanner.findNextMatchSync(`X${String.fromCharCode(0xd800)}${String.fromCharCode(0xdc00)}X`, 2)!;
			// assert.deepEqual(match.captureIndices, [{ index: 0, start: 3, end: 4, length: 1 }]);

			// match = scanner.findNextMatchSync(`X${String.fromCharCode(0xdbff)}${String.fromCharCode(0xdfff)}X`, 2)!;
			// assert.deepEqual(match.captureIndices, [{ index: 0, start: 3, end: 4, length: 1 }]);
		}));

	describe('when the start offset is out of bounds', () =>
		it('it gets clamped', () => {
			const scanner = new OnigScanner(['X']);
			let match = scanner.findNextMatchSync('X💻X', -1000)!;
			assert.deepEqual(match.captureIndices, [{ index: 0, start: 0, end: 1, length: 1 }]);

			match = scanner.findNextMatchSync('X💻X', 1000)!;
			assert.equal(match, null);
		}),
	);
});
