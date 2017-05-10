import * as fs from 'fs';
import * as path from 'path';
import { OnigString, OnigScanner } from '../src/oniguruma';
// import { OnigScanner } from '../../../bcoe/onigurumajs';

function runBenchmarkSync(lines: string[], scanner: OnigScanner) {
	const startTime = Date.now();
	let matches = 0;

	for (let row = 0, rowCount = lines.length; row < rowCount; row++) {
		let line: string | OnigString = lines[row];
		if (OnigString) {
			line = new OnigString(line);
		}
		for (let position = 0, length = line.length; position <= length; position++) {
			if (scanner.findNextMatchSync(line, position)) {
				// console.log('MATCH');
				matches++;
			}
		}
		if (line instanceof OnigString) { line.dispose(); }
	}

	console.log(`sync:  ${matches} matches in ${Date.now() - startTime}ms`);

	if (scanner.dispose) { scanner.dispose(); }
}

function fixturePath(name: string): string {
	// Support running from `benchmark/benchmark.js` dir using ts-node, or running from
	// `dist/benchmark` using node.
	if (path.basename(__dirname) === 'benchmark' && fs.existsSync(path.join(__dirname, name))) {
		return path.join(__dirname, name);
	}
	return path.join(__dirname, '..', '..', 'benchmark', name);
}

const max = 1000;
for (let i = 0; i < max; i++) {
	console.log((i + 1) + '/' + max);

	console.log('medium.go');
	runBenchmarkSync(
		fs.readFileSync(fixturePath('medium.go'), 'utf8').split('\n'),
		new OnigScanner(['\\(', '\\)', '\\{', '\\}', '\\/\\/']),
	);

	console.log('large.js');
	runBenchmarkSync(
		fs.readFileSync(fixturePath('large.js'), 'utf8').split('\n'),
		new OnigScanner(['this', 'var', 'selector', 'window']),
	);

	// console.log('oneline.js');
	// runBenchmarkSync(
	//   fs.readFileSync(fixturePath('oneline.js'), 'utf8').split('\n'),
	//   new OnigScanner(['\\[', '\\]', '\\{', '\\}']),
	// );
	console.log();
}