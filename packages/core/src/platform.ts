import { RuntimeRegistry, type GameSource, type RuntimeProvider } from '@online-gal/engine-api';
import { FileListSource, fromDataTransfer, type ImportedFile } from '@online-gal/vfs';
import { kirikiriWasmProvider } from '@online-gal/engine-core-kirikiroid2-web';
import { kirikiriProvider } from '@online-gal/engine-interpreter-kirikiri-kag3';
import { onscripterWasmProvider } from '@online-gal/engine-core-onscripter-yuri';
import { onscripterProvider } from '@online-gal/engine-interpreter-onscripter';
import { artemisProvider } from '@online-gal/engine-core-artemis-compat';
import { siglusProvider } from '@online-gal/engine-shell-siglus';
import { tyranoProvider } from '@online-gal/engine-core-tyranobuilder';
import { webProvider } from '@online-gal/engine-core-web-html5';
import { GameLibrary, type ImportRequest, type ImportResult } from './library.js';
import { SaveStore, SettingsStore } from './storage.js';
import { DEFAULT_SETTINGS, type AppSettings } from './types.js';

/**
 * Built-in runtime providers, in registration (tie-break) order.
 *
 * Order follows the goal's integration sequence: the engines that execute come
 * first, then the recognised-only ones, then the web runtimes that claim the
 * broadest set of directories — so a generic `index.html` rule can never
 * outrank a specific engine marker.
 */
export const BUILTIN_PROVIDERS: readonly RuntimeProvider[] = [
  kirikiriWasmProvider,
  // The real engine sits ahead of the interpreter because both recognise the same
  // games and a real build cannot drift the way a reimplementation can.
  kirikiriProvider,
  // The WebAssembly engine sits ahead of the TypeScript interpreter because both recognise
  // the same games: the choice is made by probe score and then by priority, but listing the
  // real engine first keeps it ahead in any tie. An actual build of OnscripterYuri cannot
  // drift from the original's behaviour the way a reimplementation can, so it should win
  // wherever it can run; the interpreter remains as the fallback for games it cannot open.
  onscripterWasmProvider,
  onscripterProvider,
  siglusProvider,
  artemisProvider,
  tyranoProvider,
  webProvider,
];

export interface PlatformOptions {
  /** Extra providers to register ahead of the built-ins. */
  readonly providers?: readonly RuntimeProvider[];
  readonly dbName?: string;
}

/**
 * Convenience composition root: one object exposing the registry, the library,
 * the save store and the settings store with a sensible default wiring. The UI
 * uses this; tests can construct the pieces directly.
 */
export class Platform {
  readonly registry: RuntimeRegistry;
  readonly library: GameLibrary;
  readonly saves: SaveStore;
  readonly settings: SettingsStore;
  private readonly probeErrors: { runtimeId: string; error: unknown }[] = [];

  constructor(options: PlatformOptions = {}) {
    this.registry = new RuntimeRegistry({
      onProbeError: (runtimeId, error) => {
        this.probeErrors.push({ runtimeId, error });
      },
    });
    this.registry.registerAll(options.providers ?? []);
    this.registry.registerAll(BUILTIN_PROVIDERS);

    this.library = new GameLibrary({
      registry: this.registry,
      ...(options.dbName ? { dbName: options.dbName } : {}),
    });
    this.saves = new SaveStore(options.dbName);
    this.settings = new SettingsStore(options.dbName);
  }

  /** Provider-level probe failures, surfaced in the diagnostics panel. */
  get probeFailures(): readonly { runtimeId: string; error: unknown }[] {
    return this.probeErrors;
  }

  async loadSettings(): Promise<AppSettings> {
    return this.settings.load();
  }

  /** Imports a directory the user picked through the File System Access API. */
  async importDirectory(handle: FileSystemDirectoryHandle): Promise<ImportResult> {
    return this.library.import({
      name: handle.name,
      source: { kind: 'directory', handle },
    });
  }

  /** Imports a `FileList` from `<input type="file" webkitdirectory>`. */
  async importFileList(files: FileList | readonly File[]): Promise<ImportResult> {
    const list = Array.from(files);
    if (list.length === 0) throw new Error('no files were selected');
    const name = deriveFolderName(list);
    const imported = FileListSource.fromFileList(name, list);
    const entries: ImportedFile[] = imported.paths().map((path) => ({
      path,
      file: list.find((file) => {
        const relative =
          (file as File & { webkitRelativePath?: string }).webkitRelativePath || file.name;
        return relative.endsWith(path);
      }) as File,
    }));
    return this.library.import({
      name,
      source: { kind: 'files', fileCount: entries.length },
      files: entries.filter((entry) => entry.file instanceof Blob),
    });
  }

  /** Imports files dropped onto the window. */
  async importDataTransfer(items: DataTransferItemList): Promise<ImportResult> {
    const files = await fromDataTransfer(items);
    if (files.length === 0) throw new Error('the drop contained no readable files');
    const name = deriveDroppedName(files);
    return this.library.import({
      name,
      source: { kind: 'files', fileCount: files.length },
      files,
    });
  }

  /** Imports from the dev server's configured game root, for development. */
  async importLocalPath(path: string, name?: string): Promise<ImportResult> {
    return this.library.import({
      name: name ?? path.replace(/\/+$/, '').split('/').pop() ?? 'local game',
      source: { kind: 'local', localPath: path },
    });
  }

  /** Re-imports every record, useful after adding a provider. */
  async reindex(): Promise<number> {
    let updated = 0;
    for (const record of await this.library.list()) {
      if (await this.library.redetect(record.id)) updated += 1;
    }
    return updated;
  }
}

function deriveFolderName(files: readonly File[]): string {
  for (const file of files) {
    const relative = (file as File & { webkitRelativePath?: string }).webkitRelativePath;
    if (relative && relative.includes('/')) return relative.split('/')[0]!;
  }
  // A selection with no `webkitRelativePath` is not a folder: it is one or more loose
  // files, which is how an archive arrives through the file input. Naming that entry
  // after the archive beats calling every such import "imported game".
  const single = files.length === 1 ? files[0] : undefined;
  if (single) {
    const relative = (single as File & { webkitRelativePath?: string }).webkitRelativePath;
    if (!relative) return single.name.replace(/\.[^.]+$/, '') || single.name;
  }
  return 'imported game';
}

function deriveDroppedName(files: readonly ImportedFile[]): string {
  const roots = new Set<string>();
  for (const file of files) {
    const segment = file.path.split('/')[0];
    if (segment) roots.add(segment);
  }
  // A dropped folder yields one common root; a loose file yields its own name.
  if (roots.size === 1) return [...roots][0]!;
  return 'imported game';
}

/** Default settings re-exported for the UI's initial render. */
export { DEFAULT_SETTINGS };

/** Constructs a `GameSource` for a raw set of imported files. */
export function sourceFromImportedFiles(
  name: string,
  files: readonly ImportedFile[],
): GameSource {
  return new FileListSource(name, files);
}

export type { ImportRequest, ImportResult };
