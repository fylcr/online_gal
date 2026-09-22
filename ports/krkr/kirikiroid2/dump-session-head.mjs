// Read-only diagnostic: a DSH session store is a concatenation of zstd frames,
// one per appended batch. Node's one-shot decompressor only decodes the first
// frame, so frames are located by their magic and decoded individually.
import { readFileSync } from "node:fs";
import { zstdDecompressSync } from "node:zlib";

const MAGIC = Buffer.from([0x28, 0xb5, 0x2f, 0xfd]);

const file = process.argv[2];
const buffer = readFileSync(file);
const starts = [];
for (let i = 0; i + 4 <= buffer.length; i++) {
  if (buffer.compare(MAGIC, 0, 4, i, i + 4) === 0) starts.push(i);
}
console.log("frames: " + starts.length);

let text = "";
for (let f = 0; f < starts.length; f++) {
  const end = f + 1 < starts.length ? starts[f + 1] : buffer.length;
  try {
    text += zstdDecompressSync(buffer.subarray(starts[f], end)).toString("utf8");
  } catch (error) {
    console.log("frame " + f + " failed: " + error.message);
  }
}
const lines = text.split("\n").filter((line) => line.trim() !== "");
console.log("events: " + lines.length);
for (const line of lines.slice(0, 5)) {
  const event = JSON.parse(line);
  const copy = { ...event };
  if (copy.data !== undefined && JSON.stringify(copy.data).length > 160) copy.data = "<large>";
  console.log(JSON.stringify(copy));
}
console.log("--- last 2 ---");
for (const line of lines.slice(-2)) {
  const event = JSON.parse(line);
  console.log(JSON.stringify(event).slice(0, 300));
}
