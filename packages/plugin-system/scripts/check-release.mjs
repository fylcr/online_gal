#!/usr/bin/env node
import { verifyOfficialRelease, verifyOfficialReleaseAt } from '../dist/release-check.js';

const [host, salt, hash, flag, deploymentOrigin, ...extra] = process.argv.slice(2);
const deployment = /^https:\/\/[a-f0-9]{8}\.[a-z0-9][a-z0-9-]*\.pages\.dev$/;
if (!host || !salt || !hash || extra.length || (flag !== undefined &&
    (flag !== '--deployment-origin' || !deployment.test(deploymentOrigin ?? '')))) {
  console.error('Usage: node packages/plugin-system/scripts/check-release.mjs <host> <salt> <hash> [--deployment-origin <https://id.project.pages.dev>]');
  process.exitCode = 2;
} else {
  try {
    const candidate = { host, salt, hash };
    const packages = deploymentOrigin
      ? await verifyOfficialReleaseAt(candidate, deploymentOrigin)
      : await verifyOfficialRelease(candidate);
    console.log(JSON.stringify({ ok: true, packages }, null, 2));
  } catch (error) {
    console.error(error instanceof Error ? error.message : error);
    process.exitCode = 1;
  }
}
