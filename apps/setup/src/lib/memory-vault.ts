import type { VaultRecord, VaultStore } from '@online-gal/plugin-system/account';

export class MemoryVaultStore implements VaultStore {
  private key: CryptoKey | undefined;
  private readonly records = new Map<string, VaultRecord>();

  async getKey() { return this.key; }
  async addKey(key: CryptoKey) { return this.key ??= key; }
  async get(provider: string) { return this.records.get(provider); }
  async put(record: VaultRecord) { this.records.set(record.provider, record); }
  async delete(provider: string) { this.records.delete(provider); }
}
