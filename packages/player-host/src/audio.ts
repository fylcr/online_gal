import type { AudioChannel } from '@online-gal/engine-api';

interface PlayingSound {
  readonly source: AudioBufferSourceNode;
  readonly gain: GainNode;
  readonly channel: AudioChannel;
  buffer: AudioBuffer;
  pausedAt: number;
  startedAt: number;
  /** True while it has been started and not yet stopped. */
  active: boolean;
}

export interface AudioLike {
  readonly duration: number;
}

export interface PlayOptions {
  readonly loop?: boolean;
  readonly volume?: number;
  readonly channel?: AudioChannel;
  /** Offset into the buffer, in seconds. */
  readonly offset?: number;
}

/**
 * Channel-aware audio engine built on Web Audio.
 *
 * KAG and ONS both address audio by channel id (`0` for BGM, `1` for SE, and
 * so on), so playback is keyed by a string channel with a per-channel master
 * gain — the same model the native engines expose, which keeps the runtime
 * code identical in shape.
 */
export class AudioEngine {
  private readonly context: AudioContext;
  private readonly masters = new Map<AudioChannel, GainNode>();
  private readonly playing = new Map<string, PlayingSound>();
  private readonly decodeCache = new Map<string, AudioBuffer>();
  private volumes: Record<AudioChannel, number> = {
    bgm: 0.8,
    se: 0.9,
    voice: 1,
    system: 0.7,
  };
  private muted = false;
  private suspended = false;

  constructor(context?: AudioContext) {
    this.context =
      context ??
      new (globalThis.AudioContext ??
        (globalThis as { webkitAudioContext?: typeof AudioContext })
          .webkitAudioContext!)();
    for (const channel of ['bgm', 'se', 'voice', 'system'] as const) {
      const gain = this.context.createGain();
      gain.gain.value = this.volumes[channel];
      gain.connect(this.context.destination);
      this.masters.set(channel, gain);
    }
  }

  get sampleRate(): number {
    return this.context.sampleRate;
  }

  get state(): AudioContextState {
    return this.context.state;
  }

  /** Browsers require a user gesture before audio may start. */
  async unlock(): Promise<void> {
    if (this.context.state === 'suspended' && !this.suspended) {
      await this.context.resume();
    }
  }

  setVolume(channel: AudioChannel, value: number): void {
    const clamped = Math.min(1, Math.max(0, value));
    this.volumes[channel] = clamped;
    this.masters.get(channel)!.gain.value = this.muted ? 0 : clamped;
  }

  getVolume(channel: AudioChannel): number {
    return this.volumes[channel];
  }

  setMuted(muted: boolean): void {
    this.muted = muted;
    for (const channel of this.masters.keys()) {
      this.masters.get(channel)!.gain.value = muted ? 0 : this.volumes[channel];
    }
  }

  isMuted(): boolean {
    return this.muted;
  }

  /** Decodes audio bytes, memoising by cache key (usually the VFS path). */
  async decode(bytes: Uint8Array, cacheKey?: string): Promise<AudioBuffer> {
    if (cacheKey) {
      const cached = this.decodeCache.get(cacheKey);
      if (cached) return cached;
    }
    // `decodeAudioData` detaches the buffer, so always hand it a copy.
    const copy = bytes.slice().buffer;
    const buffer = await this.context.decodeAudioData(copy);
    if (cacheKey) this.decodeCache.set(cacheKey, buffer);
    return buffer;
  }

  clearDecodeCache(): void {
    this.decodeCache.clear();
  }

  /** Starts playback on `channel`, replacing whatever was playing there. */
  play(channel: string, buffer: AudioBuffer, options: PlayOptions = {}): void {
    this.stop(channel);
    const channelId = options.channel ?? 'bgm';
    const master = this.masters.get(channelId) ?? this.masters.get('bgm')!;
    const gain = this.context.createGain();
    gain.gain.value = options.volume ?? 1;
    gain.connect(master);

    const source = this.context.createBufferSource();
    source.buffer = buffer;
    source.loop = options.loop ?? false;
    source.connect(gain);

    const offset = Math.max(0, options.offset ?? 0);
    source.start(0, offset);
    this.playing.set(channel, {
      source,
      gain,
      channel: channelId,
      buffer,
      pausedAt: offset,
      startedAt: this.context.currentTime,
      active: true,
    });
    source.onended = () => {
      const current = this.playing.get(channel);
      if (current && current.source === source) {
        // A looping source never actually ends; a non-looping one does.
        if (!source.loop) this.playing.delete(channel);
      }
    };
  }

  isPlaying(channel: string): boolean {
    return this.playing.get(channel)?.active ?? false;
  }

  /** Current playback position in seconds. */
  position(channel: string): number {
    const sound = this.playing.get(channel);
    if (!sound || !sound.active) return sound?.pausedAt ?? 0;
    if (sound.buffer.duration <= 0) return 0;
    const elapsed = this.context.currentTime - sound.startedAt;
    const raw = sound.pausedAt + elapsed;
    return sound.source.loop ? raw % sound.buffer.duration : raw;
  }

  fade(channel: string, toVolume: number, durationMs: number): void {
    const sound = this.playing.get(channel);
    if (!sound) return;
    const now = this.context.currentTime;
    const target = Math.min(1, Math.max(0, toVolume));
    sound.gain.gain.cancelScheduledValues(now);
    sound.gain.gain.setValueAtTime(sound.gain.gain.value, now);
    sound.gain.gain.linearRampToValueAtTime(target, now + durationMs / 1000);
  }

  /** Stops a channel. `fadeMs > 0` ramps out instead of cutting. */
  stop(channel: string, fadeMs = 0): void {
    const sound = this.playing.get(channel);
    if (!sound) return;
    this.playing.delete(channel);
    if (fadeMs > 0) {
      const now = this.context.currentTime;
      sound.pausedAt = this.position(channel);
      sound.gain.gain.cancelScheduledValues(now);
      sound.gain.gain.setValueAtTime(sound.gain.gain.value, now);
      sound.gain.gain.linearRampToValueAtTime(0, now + fadeMs / 1000);
      sound.active = false;
      try {
        sound.source.stop(now + fadeMs / 1000);
      } catch {
        // Already stopped.
      }
      return;
    }
    sound.active = false;
    try {
      sound.source.stop();
    } catch {
      // Already stopped.
    }
  }

  stopAll(): void {
    for (const channel of [...this.playing.keys()]) this.stop(channel);
  }

  /** Suspends the whole graph, preserving positions. */
  pause(): void {
    if (this.suspended) return;
    for (const [channel, sound] of this.playing) {
      sound.pausedAt = this.position(channel);
      sound.active = false;
      try {
        sound.source.stop();
      } catch {
        // Ignore.
      }
    }
    this.suspended = true;
    void this.context.suspend();
  }

  /** Resumes everything that was playing before `pause()`. */
  async resume(): Promise<void> {
    if (!this.suspended) return;
    this.suspended = false;
    await this.context.resume();
    for (const [channel, sound] of [...this.playing]) {
      this.play(channel, sound.buffer, {
        channel: sound.channel,
        loop: sound.source.loop,
        volume: sound.gain.gain.value,
        offset: sound.pausedAt,
      });
    }
  }

  dispose(): void {
    this.stopAll();
    this.decodeCache.clear();
    void this.context.close();
  }
}
