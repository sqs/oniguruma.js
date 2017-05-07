// Entrypoint for web browsers

// tslint:disable-next-line:no-var-requires
const nbind = (require as any)('../asmjs/nbind.js');

nbind.init((err: any, binding: any) => {
	console.log('>> nbind.init');
	// tslint:disable-next-line:no-debugger
	debugger;
});