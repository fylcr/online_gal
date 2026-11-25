<script lang="ts">
  import { formatPlayTime, relativeTime, type GameRecord } from '../../lib/platform';

  let { game, blocked, onDetails, onMenu }: {
    game: GameRecord;
    blocked: string | null;
    onDetails: (event: MouseEvent) => void;
    onMenu: (event: MouseEvent) => void;
  } = $props();
  const name = $derived(game.title || game.name);
</script>

{#snippet cover()}
  <figure class="relative aspect-[5/7] overflow-hidden bg-base-300">
    {#if game.cover}
      <img src={game.cover} alt="" loading="lazy" class="absolute inset-0 size-full object-cover" />
    {:else}
      <span aria-hidden="true" class="text-6xl font-bold text-base-content/25">{Array.from(name)[0]}</span>
    {/if}
    {#if game.detection.runtimeName}
      <span class="absolute top-2 left-2 rounded-full bg-base-100/90 px-2 py-0.5 text-xs font-medium">
        {game.detection.runtimeName}
      </span>
    {/if}
    {#if blocked}
      <div class="absolute inset-0 grid place-items-center bg-base-100/60 p-2">
        <span class="rounded-xl bg-base-content px-3 py-1 text-center text-xs text-base-100">{blocked}</span>
      </div>
    {:else}
      <div class="absolute inset-x-0 bottom-0 flex justify-end bg-gradient-to-t from-black/60 to-transparent px-2 pt-6 pb-2 text-xs text-white">
        {game.playTimeMs > 0 ? formatPlayTime(game.playTimeMs) : '未玩过'}
      </div>
    {/if}
  </figure>
  <div class="px-3 pt-2">
    <h2 class="line-clamp-2 h-10 text-sm leading-5 font-medium">{name}</h2>
    <p class="text-xs text-base-content/65">{relativeTime(game.lastPlayedAt)}</p>
  </div>
{/snippet}

<!-- Right-click supplements the visible details button for keyboard and touch users. -->
<li oncontextmenu={onMenu} class="card min-w-0 overflow-hidden rounded-2xl bg-base-200">
  {#if blocked}
    <div>{@render cover()}</div>
  {:else}
    <a href={`/play?id=${encodeURIComponent(game.id)}`} aria-label={`开始 ${name}`} class="block hover:text-primary focus-visible:outline-offset-[-2px]">
      {@render cover()}
    </a>
  {/if}
  <button type="button" class="btn btn-ghost min-h-11 justify-start self-start px-3 text-xs" aria-label={`游戏详情 ${name}`} onclick={onDetails}>
    游戏详情
  </button>
</li>
