import * as assert from 'assert';
import { OnigRegExp } from '../src/oniguruma';

describe.skip('OnigRegExp', () => {
	describe('::searchSync(string, index)', () => {
		it('returns an array of the match and all capture groups', () => {
			const regex = new OnigRegExp('\\w(\\d+)');
			const result = regex.searchSync('----a123----');
			if (!result) { throw new Error('!result'); }
			assert.equal(result.length, 2);
			assert.equal(result[0].match, 'a123');
			assert.equal(result[0].start, 4);
			assert.equal(result[0].end, 8);
			assert.equal(result[0].index, 0);
			assert.equal(result[0].length, 4);
			assert.equal(result[1].match, '123');
			assert.equal(result[1].start, 5);
			assert.equal(result[1].end, 8);
			assert.equal(result[1].index, 1);
			assert.equal(result[1].length, 3);
		});

		it('returns null if it does not match', () => {
			const regex = new OnigRegExp('\\w(\\d+)');
			const result = regex.searchSync('--------');
			assert.equal(result, null);
		});

		describe('when the string being searched contains a unicode character', () =>
			it('returns correct indices and lengths', () => {
				const regex = new OnigRegExp('a');

				const firstMatch = regex.searchSync('ç√Ωa', 0)!;
				assert.equal(firstMatch[0].start, 3);
				assert.equal(firstMatch[0].match, 'a');

				const secondMatch = regex.searchSync('ç√Ωabcd≈ßåabcd', 5)!;
				assert.equal(secondMatch[0].start, 10);
				assert.equal(secondMatch[0].match, 'a');
			}),
		);

		describe('when the string being searched contains non-Basic Multilingual Plane characters', () =>
			it('returns correct indices and matches', () => {
				const regex = new OnigRegExp("'");

				let match = regex.searchSync("'\uD835\uDF97'", 0)!;
				assert.equal(match[0].start, 0);
				assert.equal(match[0].match, "'");

				match = regex.searchSync("'\uD835\uDF97'", 1)!;
				assert.equal(match[0].start, 3);
				assert.equal(match[0].match, "'");

				match = regex.searchSync("'\uD835\uDF97'", 2)!;
				assert.equal(match[0].start, 3);
				assert.equal(match[0].match, "'");

				match = regex.searchSync("'\uD835\uDF97'", 3)!;
				assert.equal(match[0].start, 3);
				assert.equal(match[0].match, "'");
			}),
		);
	});

	describe('::testSync(string)', () =>
		it('returns true if the string matches the pattern', () => {
			assert.equal(new OnigRegExp('a[b-d]c').testSync('aec'), false);
			assert.equal(new OnigRegExp('a[b-d]c').testSync('abc'), true);
		}),
	);
});