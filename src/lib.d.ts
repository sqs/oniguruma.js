export class NBindBase { free?(): void }

export class OnigCaptureIndex extends NBindBase {
	/** OnigCaptureIndex(); */
	constructor();

	/** OnigCaptureIndex(const OnigCaptureIndex *); */
	constructor(p0: OnigCaptureIndex);

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

	/** int32_t index; -- Read-only */
	index: number;

	/** std::vector<OnigCaptureIndex> captureIndices; -- Read-only */
	captureIndices: OnigCaptureIndex[];
}

export class OnigRegExp extends NBindBase {
	/** OnigRegExp(std::string &); */
	constructor(p0: any);

	/** std::shared_ptr<OnigResult> Search(OnigString *, int32_t); */
	Search(p0: OnigString, p1: number): OnigResult | null;
}

export class OnigResult extends NBindBase {
	/** int32_t Count(); */
	Count(): number;

	/** int32_t LocationAt(int32_t); */
	LocationAt(p0: number): number;

	/** int32_t LengthAt(int32_t); */
	LengthAt(p0: number): number;

	/** int32_t Index(); */
	Index(): number;

	/** void SetIndex(int32_t); */
	SetIndex(p0: number): void;
}

export class OnigScanner extends NBindBase {
	/** OnigScanner(std::vector<std::string>); */
	constructor(p0: string[]);

	/** OnigNextMatchResult * FindNextMatchSync(OnigString *, int32_t); */
	FindNextMatchSync(p0: OnigString, p1: number): OnigNextMatchResult | null;

	/** static std::vector<OnigCaptureIndex> CaptureIndicesForMatch(OnigResult *, OnigString *); */
	static CaptureIndicesForMatch(p0: OnigResult, p1: OnigString): OnigCaptureIndex[];
}

export class OnigString extends NBindBase {
	/** OnigString(std::string); */
	constructor(p0: string);

	/** int32_t uniqueId(); */
	uniqueId(): number;

	/** const char * utf8_value(); */
	utf8_value(): string | null;

	/** uint64_t utf8_length(); */
	utf8_length(): number;

	/** int32_t ConvertUtf8OffsetToUtf16(int32_t); */
	ConvertUtf8OffsetToUtf16(p0: number): number;

	/** int32_t ConvertUtf16OffsetToUtf8(int32_t); */
	ConvertUtf16OffsetToUtf8(p0: number): number;
}
