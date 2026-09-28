import { ARTEMIS_ERRNO, ArtemisErrnoError } from './errno.js';
import type { ArtemisMemory } from './memory.js';

/**
 * Time conversion: `localtime_r`, `gmtime_r`, `mktime`, `strftime`, and the clock family.
 *
 * The struct layout is musl's `struct tm` as the glue writes it, and the offsets are not
 * negotiable — the compiled formatter reads `tm_year` at +20 and nothing else. The whole thing is
 * reproduced from the official bodies rather than from a C header, because two details differ from
 * what a C programmer would expect:
 *
 * - **`tm_gmtoff` is at +36 and `tm_isdst` at +32**, not the other way round. The glue writes
 *   `+36 gmtoff, +40 zone ptr`, and `strftime`'s `%z`/`%Z` read exactly those.
 * - **`tm_zone` is a pointer to UTF-8, not a 16-byte inline array.** The glue gets it from
 *   has a `tm_zone` pointer that points at UTF-8 bytes **inside the guest's own heap**, so the
 *   pointer is only meaningful to the guest. This host has no `asm` object, so `TimeZoneNames`
 *   takes the allocator the host was given and does the same allocation itself: the two names and
 *   the two-word pointer array are malloc'd once and never freed, exactly as the glue's
 *   `allocateUTF8` calls are, which is what makes a pointer handed out once stay valid.
 */

/** `sizeof(struct tm)` in the glue's layout: 10 ints plus two pointers. */
export const TM_SIZE = 44;

/** Byte offsets in the guest's `struct tm`. */
export const TM_OFFSET = {
  sec: 0,
  min: 4,
  hour: 8,
  mday: 12,
  mon: 16,
  year: 20,
  wday: 24,
  yday: 28,
  isdst: 32,
  gmtoff: 36,
  zone: 40,
} as const;

const MONTH_DAYS_LEAP = [31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31] as const;
const MONTH_DAYS_REGULAR = [31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31] as const;
const WEEKDAYS = [
  'Sunday',
  'Monday',
  'Tuesday',
  'Wednesday',
  'Thursday',
  'Friday',
  'Saturday',
] as const;
const MONTHS = [
  'January',
  'February',
  'March',
  'April',
  'May',
  'June',
  'July',
  'August',
  'September',
  'October',
  'November',
  'December',
] as const;

function isLeapYear(year: number): boolean {
  return (year % 4 === 0 && year % 100 !== 0) || year % 400 === 0;
}

function monthDays(year: number): readonly number[] {
  return isLeapYear(year) ? MONTH_DAYS_LEAP : MONTH_DAYS_REGULAR;
}

function addDays(date: Date, days: number): Date {
  const next = new Date(date.getTime());
  next.setDate(next.getDate() + days);
  return next;
}

function compareByDay(a: Date, b: Date): number {
  const delta = a.getFullYear() - b.getFullYear();
  if (delta !== 0) return delta < 0 ? -1 : 1;
  const month = a.getMonth() - b.getMonth();
  if (month !== 0) return month < 0 ? -1 : 1;
  const day = a.getDate() - b.getDate();
  return day < 0 ? -1 : day > 0 ? 1 : 0;
}

/**
 * Zone-name storage standing in for the glue's `__get_tzname()`.
 *
 * The official path is `_tzset()` → `allocateUTF8` for each of the two names → a two-word array →
 * `Module.asm.Uc()` returns a pointer to that array. Without an `asm` object the host owns the
 * allocation, but it must own it in the GUEST's heap: `tm_zone` is read by the guest, and a
 * pointer into any buffer of ours is meaningless there. So the two UTF-8 names and the two-word
 * array are malloc'd once through the host's `malloc` hook and never freed — exactly the lifetime
 * the glue's own `allocateUTF8` calls have.
 */
export class TimeZoneNames {
  /** One malloc'd copy of each name, plus the two-word pointer array `__get_tzname` returns. */
  private names: [number, number] | null = null;
  private label: string;

  /**
   * @param allocate The guest's allocator, from the host's `malloc` hook. A pointer into any buffer
   *   of our own is meaningless to the guest, which dereferences it in ITS heap. A host that lent
   *   no `malloc` gets pointers of 0, which is the "no zone name" case `strftime` already handles.
   * @param zone The zone label to report, as the glue's `_tzset` would have set it.
   */
  constructor(
    private readonly allocate: ((bytes: number) => number) | undefined,
    zone?: string,
  ) {
    this.label = zone ?? 'UTC';
  }

  /**
   * Allocates both names and the pointer array, once, and returns `[std, dst]` — the two NAME
   * pointers, not the two words of the array.
   *
   * The distinction is load-bearing and easy to get backwards. The glue writes
   * `HEAP32[__get_tzname()>>>2] = winterNamePtr`, and `_localtime_r` then reads
   * `zonePtr = HEAP32[__get_tzname() + (dst?4:0)]` — so `__get_tzname()` yields the ARRAY address
   * and `tm_zone` receives a NAME pointer taken out of it. Returning the array word itself (as an
   * earlier revision of this class did) makes `tm_zone` point at two pointers instead of at a
   * string, and `strftime`'s `%Z` then prints whatever bytes happen to sit there.
   *
   * A zero address at any step is not an error: it means the names could not be interned, and
   * `tm_zone` stays 0, which is exactly what a musl `struct tm` carries when no zone was set.
   */
  private intern(text: string): [number, number] {
    const allocate = this.allocate;
    if (!allocate) return [0, 0];
    const encoded = new TextEncoder().encode(text);
    const write = this.writeHook;
    if (!write) return [0, 0];
    const reserve = (): number => {
      const at = allocate(encoded.length + 1);
      if (at === 0) return 0;
      write(at, encoded);
      return at;
    };
    const std = reserve();
    const dst = reserve();
    const array = allocate(8);
    if (array === 0 || std === 0 || dst === 0) return [0, 0];
    // A `DataView` rather than `Int32Array`, because the memory's buffer is `ArrayBufferLike` and
    // wasm is little-endian regardless of what the host is.
    const view = new DataView(this.memoryView().buffer, array, 8);
    view.setInt32(0, std, true);
    view.setInt32(4, dst, true);
    return [std, dst];
  }

  /** Installed by the host so `intern` can write into the guest's memory. */
  private writeHook: ((at: number, bytes: Uint8Array) => void) | null = null;
  private memoryView: () => { buffer: ArrayBufferLike } = () => ({ buffer: new ArrayBuffer(0) });

  /**
   * Gives the class the guest's live views. Kept separate from the constructor because the
   * allocator and the memory are both host-injected and the order is not fixed. Re-attaching drops
   * the previous interning: those pointers belonged to the previous memory.
   */
  attach(memory: ArtemisMemory): void {
    this.memoryView = () => memory.u8;
    this.writeHook = (at: number, bytes: Uint8Array): void => {
      memory.u8.set(bytes, at);
    };
    this.names = null;
  }

  /** The name to report for a given DST flag, as a pointer into the guest's heap. */
  pointerFor(_memory: ArtemisMemory, isDst: boolean): number {
    if (!this.names) {
      this.names = this.intern(this.label);
    }
    return isDst ? this.names[1] : this.names[0];
  }

  /** The name itself, for `strftime`'s `%Z`. */
  textFor(_isDst: boolean): string {
    return this.label;
  }
}

/** Read a `struct tm` out of the heap into plain fields. */
export interface TmFields {
  sec: number;
  min: number;
  hour: number;
  mday: number;
  mon: number;
  year: number;
  wday: number;
  yday: number;
  isdst: number;
  gmtoff: number;
  zone: number;
}

export function readTm(memory: ArtemisMemory, tmPtr: number): TmFields {
  memory.assertRange(tmPtr, TM_SIZE);
  const at = tmPtr >>> 0;
  return {
    sec: memory.u32[at >>> 2] ?? 0,
    min: memory.u32[(at + 4) >>> 2] ?? 0,
    hour: memory.u32[(at + 8) >>> 2] ?? 0,
    mday: memory.u32[(at + 12) >>> 2] ?? 0,
    mon: memory.u32[(at + 16) >>> 2] ?? 0,
    year: memory.u32[(at + 20) >>> 2] ?? 0,
    wday: memory.u32[(at + 24) >>> 2] ?? 0,
    yday: memory.u32[(at + 28) >>> 2] ?? 0,
    isdst: memory.u32[(at + 32) >>> 2] ?? 0,
    gmtoff: memory.i32[(at + 36) >>> 2] ?? 0,
    zone: memory.u32[(at + 40) >>> 2] ?? 0,
  };
}

export function writeTm(memory: ArtemisMemory, tmPtr: number, fields: TmFields): void {
  memory.assertRange(tmPtr, TM_SIZE);
  const at = tmPtr >>> 2;
  memory.u32[at] = fields.sec;
  memory.u32[at + 1] = fields.min;
  memory.u32[at + 2] = fields.hour;
  memory.u32[at + 3] = fields.mday;
  memory.u32[at + 4] = fields.mon;
  memory.u32[at + 5] = fields.year;
  memory.u32[at + 6] = fields.wday;
  memory.u32[at + 7] = fields.yday;
  memory.u32[at + 8] = fields.isdst;
  memory.i32[at + 9] = fields.gmtoff;
  memory.u32[at + 10] = fields.zone;
}

/** `localtime_r(const time_t *restrict, struct tm *restrict)`. Returns `tmPtr`. */
export function localtimeR(memory: ArtemisMemory, timePtr: number, tmPtr: number, zones: TimeZoneNames): number {
  memory.assertRange(timePtr, 4);
  const seconds = memory.i32[timePtr >>> 2] ?? 0;
  const date = new Date(seconds * 1000);
  const winterOffset = new Date(date.getFullYear(), 0, 1).getTimezoneOffset();
  const summerOffset = new Date(date.getFullYear(), 6, 1).getTimezoneOffset();
  const raw = date.getTimezoneOffset();
  const isDst = summerOffset !== winterOffset && raw === Math.min(winterOffset, summerOffset) ? 1 : 0;
  const jan1 = new Date(date.getFullYear(), 0, 1);
  writeTm(memory, tmPtr, {
    sec: date.getSeconds(),
    min: date.getMinutes(),
    hour: date.getHours(),
    // The glue writes `date.getDate()` unshifted, so `tm_mday` here is 1-BASED — off by one from
    // C. `_mktime` reads the same field and passes it straight to `new Date(...)`, which cancels
    // the error out inside the pair. Copying the glue is the contract; "fixing" either half would
    // break the round trip the guest actually performs.
    mday: date.getDate(),
    mon: date.getMonth(),
    year: date.getFullYear() - 1900,
    wday: date.getDay(),
    yday: Math.floor((date.getTime() - jan1.getTime()) / 86400000),
    isdst: isDst,
    gmtoff: -raw * 60,
    zone: zones.pointerFor(memory, isDst === 1),
  });
  return tmPtr;
}

/** `gmtime_r`. Same layout, UTC fields, `gmtoff` 0, `isdst` 0. */
export function gmtimeR(memory: ArtemisMemory, timePtr: number, tmPtr: number, zones: TimeZoneNames): number {
  memory.assertRange(timePtr, 4);
  const seconds = memory.i32[timePtr >>> 2] ?? 0;
  const date = new Date(seconds * 1000);
  const jan1 = new Date(date.getUTCFullYear(), 0, 1);
  writeTm(memory, tmPtr, {
    sec: date.getUTCSeconds(),
    min: date.getUTCMinutes(),
    hour: date.getUTCHours(),
    mday: date.getUTCDate(),
    mon: date.getUTCMonth(),
    year: date.getUTCFullYear() - 1900,
    wday: date.getUTCDay(),
    yday: Math.floor((date.getTime() - jan1.getTime()) / 86400000),
    isdst: 0,
    gmtoff: 0,
    zone: zones.pointerFor(memory, false),
  });
  return tmPtr;
}

/**
 * `mktime(struct tm *)`. Normalises the fields in place and returns seconds since the epoch.
 *
 * The DST adjustment is the fiddly part and is copied from the glue: JavaScript's `Date` resolves
 * an ambiguous local time to the earlier offset, so the glue compares the constructed date's offset
 * against the standard-time offset for that half of the year and steps an hour if they disagree.
 *
 * `tm_mday` arrives 1-based (see {@link localtimeR}) and is passed to `new Date(...)` unshifted,
 * exactly as the glue does — which is what makes the `localtime_r` → `mktime` round trip agree with
 * itself. The write-back below is the same 1-based convention.
 */
export function mktime(memory: ArtemisMemory, tmPtr: number): number {
  const fields = readTm(memory, tmPtr);
  const date = new Date(
    fields.year + 1900,
    fields.mon,
    fields.mday,
    fields.hour,
    fields.min,
    fields.sec,
    0,
  );
  const janOffset = new Date(date.getFullYear(), 0, 1).getTimezoneOffset();
  const julOffset = new Date(date.getFullYear(), 6, 1).getTimezoneOffset();
  const currentOffset = date.getTimezoneOffset();
  const standard = Math.max(janOffset, julOffset);
  if (currentOffset < standard) {
    date.setTime(date.getTime() + (standard - currentOffset) * 60000);
  }
  const seconds = Math.floor(date.getTime() / 1000);
  writeTm(memory, tmPtr, {
    ...fields,
    sec: date.getSeconds(),
    min: date.getMinutes(),
    hour: date.getHours(),
    mday: date.getDate(),
    mon: date.getMonth(),
    year: date.getFullYear() - 1900,
    wday: date.getDay(),
    yday: dayOfYear(date),
  });
  return seconds;
}

function dayOfYear(date: Date): number {
  const jan1 = new Date(date.getFullYear(), 0, 1);
  return Math.floor((date.getTime() - jan1.getTime()) / 86400000);
}

function leadingNulls(value: number, digits: number): string {
  let text = String(value);
  while (text.length < digits) {
    text = `0${text}`;
  }
  return text;
}

function leadingSpaces(value: number, digits: number): string {
  let text = String(value);
  while (text.length < digits) {
    text = ` ${text}`;
  }
  return text;
}

function firstWeekStartDate(janFourth: Date): Date {
  switch (janFourth.getDay()) {
    case 0:
      return new Date(janFourth.getFullYear() - 1, 11, 29);
    case 1:
      return janFourth;
    case 2:
      return new Date(janFourth.getFullYear(), 0, 3);
    case 3:
      return new Date(janFourth.getFullYear(), 0, 2);
    case 4:
      return new Date(janFourth.getFullYear(), 0, 1);
    case 5:
      return new Date(janFourth.getFullYear() - 1, 11, 31);
    default:
      return new Date(janFourth.getFullYear() - 1, 11, 30);
  }
}

function weekBasedYear(fields: TmFields, zones: TimeZoneNames): number {
  const thisDate = addDays(new Date(fields.year + 1900, 0, 1), fields.yday);
  const firstThisYear = firstWeekStartDate(new Date(thisDate.getFullYear(), 0, 4));
  const firstNextYear = firstWeekStartDate(new Date(thisDate.getFullYear() + 1, 0, 4));
  void zones;
  if (compareByDay(firstThisYear, thisDate) <= 0) {
    return compareByDay(firstNextYear, thisDate) <= 0 ? thisDate.getFullYear() + 1 : thisDate.getFullYear();
  }
  return thisDate.getFullYear() - 1;
}

/** `strftime(char *restrict s, size_t maxsize, const char *restrict format, const struct tm *restrict tm)`. */
export function strftime(
  memory: ArtemisMemory,
  s: number,
  maxsize: number,
  formatPtr: number,
  tmPtr: number,
  zones: TimeZoneNames,
): number {
  const fields = readTm(memory, tmPtr);
  const isDst = fields.isdst !== 0;
  const zoneName = fields.zone !== 0 ? memory.utf8(fields.zone) : zones.textFor(isDst);
  let pattern = memory.utf8(formatPtr);

  // Rule set 1 first: pure string substitution, so `%F` is a macro over `%Y-%m-%d` exactly as the
  // glue has it. Doing these in one pass would let a `%` inside an expansion be re-expanded.
  const rule1: Record<string, string> = {
    '%c': '%a %b %d %H:%M:%S %Y',
    '%D': '%m/%d/%y',
    '%F': '%Y-%m-%d',
    '%h': '%b',
    '%r': '%I:%M:%S %p',
    '%R': '%H:%M',
    '%T': '%H:%M:%S',
    '%x': '%m/%d/%y',
    '%X': '%H:%M:%S',
    '%Ec': '%c',
    '%EC': '%C',
    '%Ex': '%m/%d/%y',
    '%EX': '%H:%M:%S',
    '%Ey': '%y',
    '%EY': '%Y',
    '%Od': '%d',
    '%Oe': '%e',
    '%OH': '%H',
    '%OI': '%I',
    '%Om': '%m',
    '%OM': '%M',
    '%OS': '%S',
    '%Ou': '%u',
    '%OU': '%U',
    '%OV': '%V',
    '%Ow': '%w',
    '%OW': '%W',
    '%Oy': '%y',
  };
  for (const rule of Object.keys(rule1)) {
    pattern = pattern.split(rule).join(rule1[rule]!);
  }

  const rule2: Record<string, () => string> = {
    '%a': () => (WEEKDAYS[fields.wday] ?? '').slice(0, 3),
    '%A': () => WEEKDAYS[fields.wday] ?? '',
    '%b': () => (MONTHS[fields.mon] ?? '').slice(0, 3),
    '%B': () => MONTHS[fields.mon] ?? '',
    '%C': () => leadingNulls(Math.floor((fields.year + 1900) / 100), 2),
    '%d': () => leadingNulls(fields.mday, 2),
    '%e': () => leadingSpaces(fields.mday, 2),
    '%g': () => String(weekBasedYear(fields, zones)).slice(2),
    '%G': () => String(weekBasedYear(fields, zones)),
    '%H': () => leadingNulls(fields.hour, 2),
    '%I': () => {
      const twelve = fields.hour === 0 ? 12 : fields.hour > 12 ? fields.hour - 12 : fields.hour;
      return leadingNulls(twelve, 2);
    },
    '%j': () => {
      // `__arraySum(__MONTH_DAYS_*, tm_mon - 1)` sums the table *through* index `mon - 1`, so the
      // running total is the days before month `mon` — the correct prefix, reached by an off-by-one
      // that cancels. Written out here in the same shape so the two agree.
      const table = monthDays(fields.year + 1900);
      const before = fields.mon <= 0 ? 0 : table.slice(0, fields.mon).reduce((a, b) => a + b, 0);
      return leadingNulls(fields.mday + before, 3);
    },
    '%m': () => leadingNulls(fields.mon + 1, 2),
    '%M': () => leadingNulls(fields.min, 2),
    '%n': () => '\n',
    '%p': () => (fields.hour >= 0 && fields.hour < 12 ? 'AM' : 'PM'),
    '%S': () => leadingNulls(fields.sec, 2),
    '%t': () => '\t',
    '%u': () => String(fields.wday === 0 ? 7 : fields.wday),
    '%U': () => weekOfYear(fields, true),
    '%V': () => isoWeek(fields),
    '%w': () => String(fields.wday),
    '%W': () => weekOfYear(fields, false),
    '%y': () => String(fields.year + 1900).slice(2),
    '%Y': () => String(fields.year + 1900),
    '%z': () => formatOffset(fields.gmtoff),
    '%Z': () => zoneName,
    '%%': () => '%',
  };
  for (const rule of Object.keys(rule2)) {
    if (pattern.includes(rule)) {
      pattern = pattern.split(rule).join(rule2[rule]!());
    }
  }

  // The glue writes latin1, so a format producing a non-latin1 character is truncated rather than
  // UTF-8 encoded. Keeping that is what makes `%Z` with a CJK zone name produce mojibake instead of
  // a different byte count than the guest's buffer expects.
  //
  // `bytes` holds the VISIBLE bytes only. The glue's array also holds the terminator, because it
  // comes from `intArrayFromString(pattern, false)` — `dontAddNull` false means the NUL stays — so
  // both of the glue's tail comparisons are shifted by one relative to this array, and both shifts
  // have to be applied explicitly rather than by accident:
  const bytes = new Uint8Array(pattern.length);
  for (let i = 0; i < pattern.length; i += 1) {
    bytes[i] = pattern.charCodeAt(i) & 0xff;
  }
  // The glue tests `if (bytes.length > maxsize) return 0` on an array that still counts the NUL, so
  // it refuses a string whose visible length merely EQUALS maxsize — that is what makes the guest's
  // own "buffer full" contract exact, and it is why a pattern one byte too long is a 0, not a
  // silently truncated write.
  if (bytes.length + 1 > maxsize) {
    return 0;
  }
  memory.assertRange(s, bytes.length);
  memory.u8.set(bytes, s);
  // The glue returns `bytes.length - 1`, and the `-1` is precisely the terminator its array carried.
  // Subtracting it again would drop a real character, so the visible count is the right answer.
  return bytes.length;
}

function formatOffset(gmtoff: number): string {
  const ahead = gmtoff >= 0;
  const total = Math.abs(gmtoff);
  const hours = Math.floor(total / 3600);
  const minutes = Math.floor((total % 3600) / 60);
  return `${ahead ? '+' : '-'}${leadingNulls(hours, 2)}${leadingNulls(minutes, 2)}`;
}

function weekOfYear(fields: TmFields, sundayFirst: boolean): string {
  const year = fields.year + 1900;
  // `%W` in the glue builds its January with `new Date(date.tm_year, 0, 1)` — `tm_year` raw, with no
  // `+1900`. That lands in the year 1900 off for every date since, and it changes the weekday the
  // calculation pivots on. It is a bug in the glue, but the guest's own week numbers were produced
  // with it, so a "correct" week number here would be a different number from the one it expects.
  const janFirst = sundayFirst ? new Date(year, 0, 1) : new Date(fields.year, 0, 1);
  const firstDay = sundayFirst
    ? janFirst.getDay() === 0
      ? janFirst
      : addDays(janFirst, 7 - janFirst.getDay())
    : janFirst.getDay() === 1
      ? janFirst
      : addDays(janFirst, janFirst.getDay() === 0 ? 1 : 7 - janFirst.getDay() + 1);
  // `tm_mday` is 1-based here, and the glue passes it to `new Date(...)` unshifted.
  const end = new Date(year, fields.mon, fields.mday);
  if (compareByDay(firstDay, end) < 0) {
    // The glue sums the month table to `end.getMonth() - 1` and subtracts 31, i.e. the days from
    // February's 1st to the end of the month. `arraySum(table, index)` sums *through* the index.
    const table = monthDays(end.getFullYear());
    const untilEndMonth = table.slice(0, Math.max(end.getMonth(), 0)).reduce((a, b) => a + b, 0) - 31;
    const untilEndJanuary = 31 - firstDay.getDate();
    const total = untilEndJanuary + untilEndMonth + end.getDate();
    return leadingNulls(Math.ceil(total / 7), 2);
  }
  return compareByDay(firstDay, janFirst) === 0 ? '01' : '00';
}

function isoWeek(fields: TmFields): string {
  const year = fields.year + 1900;
  const firstThisYear = firstWeekStartDate(new Date(year, 0, 4));
  const firstNextYear = firstWeekStartDate(new Date(year + 1, 0, 4));
  const end = addDays(new Date(year, 0, 1), fields.yday);
  if (compareByDay(end, firstThisYear) < 0) {
    return '53';
  }
  if (compareByDay(firstNextYear, end) <= 0) {
    return '01';
  }
  const difference =
    firstThisYear.getFullYear() < year
      ? fields.yday + 32 - firstThisYear.getDate()
      : fields.yday + 1 - firstThisYear.getDate();
  return leadingNulls(Math.ceil(difference / 7), 2);
}

/**
 * `clock_gettime(clockid_t clk, struct timespec *tp)`.
 *
 * The guest passes the id as two words in some call sites (`Ob` has both `(i32,i32)` and
 * `(i32,i64,i32)` shapes), so the id is read as a plain `i32` here and the high word — which the
 * glue's `get64` itself discards — is ignored. That matches the official body exactly: it never
 * reassembles the 64-bit value.
 *
 * Only `CLOCK_REALTIME` (0), `CLOCK_MONOTONIC` (1) and `CLOCK_MONOTONIC_RAW` (4) are answered.
 * Everything else is `EINVAL`, as in the glue.
 */
export function clockGettime(
  memory: ArtemisMemory,
  clk: number,
  tp: number,
  now: () => number,
): number {
  if (clk !== 0 && clk !== 1 && clk !== 4) {
    return -ARTEMIS_ERRNO.EINVAL;
  }
  memory.assertRange(tp, 8);
  const millis = now();
  const at = tp >>> 0;
  memory.u32[at >>> 2] = Math.floor(millis / 1000) >>> 0;
  memory.u32[(at + 4) >>> 2] = Math.floor(millis % 1000) * 1e6 >>> 0;
  return 0;
}

/** `clock_gettime(clockid, time_t *tp)` — the `(i32,i32)` shape, which writes a bare seconds value. */
export function clockGettimeSeconds(
  memory: ArtemisMemory,
  clk: number,
  tp: number,
  now: () => number,
): number {
  if (clk !== 0 && clk !== 1 && clk !== 4) {
    return -ARTEMIS_ERRNO.EINVAL;
  }
  memory.assertRange(tp, 4);
  memory.i32[tp >>> 2] = Math.floor(now() / 1000);
  return 0;
}

/** Guard for a caller that must produce a 32-bit result. */
export function requireInt32(value: number, what: string): number {
  if (!Number.isInteger(value) || value < -2147483648 || value > 2147483647) {
    throw new ArtemisErrnoError(ARTEMIS_ERRNO.EOVERFLOW, `${what} = ${value}`);
  }
  return value;
}
