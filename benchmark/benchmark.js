#!/usr/bin/env node

const fs = require('fs')
const path = require('path')
const { OnigScanner } = require('..')

function runBenchmarkSync(lines, scanner) {
  let startTime = Date.now()
  let matches = 0

  for (let row = 0, rowCount = lines.length; row < rowCount; row++) {
    const line = lines[row]
    for (let position = 0, length = line.length; position <= length; position++) {
      if (scanner.findNextMatchSync(line, position)) matches++
    }
  }

  console.log(`sync:  ${matches} matches in ${Date.now() - startTime}ms`)
}

console.log('medium.go')
runBenchmarkSync(
  fs.readFileSync(path.join(__dirname, 'medium.go'), 'utf8').split('\n'),
  new OnigScanner(['\\(', '\\)', '\\{', '\\}', '\\/\\/'])
)

console.log('large.js')
runBenchmarkSync(
  fs.readFileSync(path.join(__dirname, 'large.js'), 'utf8').split('\n'),
  new OnigScanner(['this', 'var', 'selector', 'window'])
)

console.log('oneline.js')
runBenchmarkSync(
  fs.readFileSync(path.join(__dirname, 'oneline.js'), 'utf8').split('\n'),
  new OnigScanner(['\\[', '\\]', '\\{', '\\}'])
)