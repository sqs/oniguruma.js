export class NBindBase { free?(): void }

export class OnigCaptureIndex extends NBindBase {
	/** OnigCaptureIndex(); */
	constructor();

	/** OnigCaptureIndex(int32_t, int32_t, int32_t, int32_t); */
	constructor(p0: number, p1: number, p2: number, p3: number);

	/** int32_t index; -- Read-only */
	index: number;

	/** int32_t start; -- Read-only */
	start: number;

	/** int32_t end; -- Read-only */
	end: number;

	/** int32_t length; -- Read-only */
	length: number;
}

export class OnigNextMatchResult extends NBindBase {
	/** OnigNextMatchResult(); */
	constructor();

	/** OnigNextMatchResult(bool); */
	constructor(p0: boolean);

	/** OnigNextMatchResult(bool, int32_t, std::vector<OnigCaptureIndex>); */
	constructor(p0: boolean, p1: number, p2: OnigCaptureIndex[]);

	/** bool noMatch; -- Read-only */
	noMatch: boolean;

	/** int32_t index; -- Read-only */
	index: number;

	/** std::vector<OnigCaptureIndex> captureIndices; -- Read-only */
	captureIndices: OnigCaptureIndex[];
}

export class OnigRegExp extends NBindBase {
	/** OnigRegExp(std::string &); */
	constructor(p0: any);

	/** std::shared_ptr<OnigResult> Search(OnigString &, int32_t); */
	Search(p0: OnigString, p1: number): OnigResult | null;
}

export class OnigResult extends NBindBase {}

export class OnigScanner extends NBindBase {
	/** OnigScanner(std::vector<std::string>); -- Strict */
	constructor(p0: string[]);

	/** OnigNextMatchResult FindNextMatchSync(OnigString &, int32_t); -- Strict */
	FindNextMatchSync(p0: OnigString, p1: number): OnigNextMatchResult;
}

export class OnigString extends NBindBase {
	/** OnigString(Buffer); -- Strict */
	constructor(p0: number[] | ArrayBuffer | DataView | Uint8Array | Buffer);

	/** const char * utf8_value(); */
	utf8_value(): string | null;

	/** uint32_t utf8_length(); */
	utf8_length(): number;
}
