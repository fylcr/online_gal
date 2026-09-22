import { u32le } from '../bytes.js';
import { BaseArchiveReader, type ArchiveEntry } from './reader.js';
import type { ByteSource } from './zip.js';

interface AsarNode {
  readonly files?: Record<string, AsarNode>;
  readonly size?: number;
  readonly offset?: string | number;
  readonly unpacked?: boolean;
}

/**
 * Electron `asar` reader.
 *
 * Some visual novels ship as NW.js/Electron bundles; Tyranor-Next detects
 * `app.asar` the same way. The layout is an 8-byte header, a JSON index, then
 * a payload blob addressed by string byte offsets.
 */
export class AsarArchiveReader extends BaseArchiveReader {
  readonly format = 'asar';

  private constructor(
    name: string,
    entries: readonly ArchiveEntry[],
    private readonly source: ByteSource,
    private readonly dataOffset: number,
    private readonly nodes: ReadonlyMap<string, AsarNode>,
  ) {
    super(name, entries);
  }

  static async open(name: string, source: ByteSource): Promise<AsarArchiveReader> {
    const prefix = await source.readRange(0, 16);
    if (prefix.byteLength < 16) throw new Error('not an asar archive (truncated)');
    const view = new DataView(prefix.buffer, prefix.byteOffset, prefix.byteLength);

    const indexSize = u32le(view, 4);
    const headerSize = u32le(view, 8);
    const jsonSize = u32le(view, 12);
    if (indexSize <= 0 || jsonSize <= 0) {
      throw new Error('not an asar archive (invalid header)');
    }

    const jsonBytes = await source.readRange(16, jsonSize);
    const json = new TextDecoder('utf-8').decode(jsonBytes);
    const root = JSON.parse(json) as AsarNode;

    const dataOffset = 8 + headerSize;
    const entries: ArchiveEntry[] = [];
    const nodes = new Map<string, AsarNode>();

    const walk = (node: AsarNode, prefixPath: string): void => {
      if (!node.files) return;
      for (const [childName, child] of Object.entries(node.files)) {
        const path = prefixPath ? `${prefixPath}/${childName}` : childName;
        if (child.files) {
          entries.push({ path: `${path}/`, size: 0, isDirectory: true });
          walk(child, path);
        } else {
          entries.push({
            path,
            size: child.size ?? 0,
            isDirectory: false,
          });
          nodes.set(path, child);
        }
      }
    };
    walk(root, '');

    return new AsarArchiveReader(name, entries, source, dataOffset, nodes);
  }

  protected async extract(entry: ArchiveEntry): Promise<Uint8Array> {
    const node = this.nodes.get(entry.path);
    if (!node) throw new Error(`asar entry missing: "${entry.path}"`);
    if (node.unpacked) {
      throw new Error(
        `"${entry.path}" lives in app.asar.unpacked and must be extracted separately`,
      );
    }
    const offset = Number(node.offset ?? 0);
    const size = node.size ?? 0;
    return this.source.readRange(this.dataOffset + offset, size);
  }
}
