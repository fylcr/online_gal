/**
 * Plugin rules shared by the plugin system: the manifest format and gate,
 * capabilities and grants, budgets, the service graph between plugins and
 * teardown order.
 *
 * The plugin system itself (`@online-gal/plugin-system`) is a Service Worker
 * that installs plugins and serves the active manager as the site; it uses
 * `parseManifest` from here. `PluginHost` runs plugin code that is loaded into
 * a page (engines, library providers) under these rules.
 *
 * Not decided yet and therefore not here: the hook vocabulary and an isolated
 * execution environment.
 */
export * from './errors.js';
export * from './manifest.js';
export * from './capabilities.js';
export * from './budget.js';
export * from './host-api.js';
export * from './registry.js';
