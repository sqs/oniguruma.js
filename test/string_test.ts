import * as assert from 'assert';
import { OnigString } from '../src/oniguruma';

describe('OnigString', () => {
	it('has a length property', () => {
		assert.equal(new OnigString('abc').length, 3);
	});

	it('can be converted back into a string', () => {
		assert.equal(new OnigString('abc').toString(), 'abc');
	});

	it('can retrieve substrings (for conveniently inspecting captured text)', () => {
		const s = 'abcdef';
		const onigString = new OnigString(s);
		assert.equal(onigString.substring(2, 3), s.substring(2, 3));
		assert.equal(onigString.substring(2), s.substring(2));
		assert.equal((onigString.substring as any)(), (s.substring as any)());
		assert.equal(onigString.substring(-1), s.substring(-1));
		assert.equal(onigString.substring(-1, -2), s.substring(-1, -2));

		(onigString.substring as any)({});
		(onigString.substring as any)(null, undefined);
	});

	it('handles invalid arguments', () => {
		assert.throws(() => (OnigString as any)(undefined).toString(), /invalid non-string value/);
	});
});
