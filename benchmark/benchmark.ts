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
			if (scanner.findNextMatchSync(line, position)) { matches++ }
		}
	}

	console.log(`sync:  ${matches} matches in ${Date.now() - startTime}ms`);
}

// console.log('medium.go');
// runBenchmarkSync(
//   fs.readFileSync(path.join(__dirname, 'medium.go'), 'utf8').split('\n'),
//   new OnigScanner(['\\(', '\\)', '\\{', '\\}', '\\/\\/'])
// );

for (let i = 0; i < 10; i++) {
	console.log('large.js');
	runBenchmarkSync(
		fs.readFileSync(path.join(__dirname, 'large.js'), 'utf8').split('\n'),
		new OnigScanner(['this', 'var', 'selector', 'window'])
	);
}

// console.log('oneline.js');
// runBenchmarkSync(
//   fs.readFileSync(path.join(__dirname, 'oneline.js'), 'utf8').split('\n'),
//   new OnigScanner(['\\[', '\\]', '\\{', '\\}'])
// );