#!/usr/bin/env node
import { verifyOfficialRelease } from '../dist/release-check.js';

const [host, salt, hash, ...extra] = process.argv.slice(2);
if (!host || !salt || !hash || extra.length) {
  console.error('Usage: node packages/plugin-system/scripts/check-release.mjs <host> <salt> <hash>');
  process.exitCode = 2;
} else {
  try {
    const packages = await verifyOfficialRelease({ host, salt, hash });
    console.log(JSON.stringify({ ok: true, packages }, null, 2));
  } catch (error) {
    console.error(error instanceof Error ? error.message : error);
    process.exitCode = 1;
  }
}
