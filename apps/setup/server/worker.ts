import { handleSetup as cloudflare } from '../../../functions/__cf/[[path]].ts';
import { onRequest as setup, type SetupEnv } from './setup.ts';

interface Env extends SetupEnv {
  ASSETS: { fetch(request: Request): Promise<Response> };
}

export default {
  async fetch(request: Request, env: Env): Promise<Response> {
    const { pathname } = new URL(request.url);
    let response: Response;
    try {
      if (pathname === '/__cf' || pathname.startsWith('/__cf/')) response = await cloudflare(request);
      else if (pathname === '/__setup' || pathname.startsWith('/__setup/')) response = await setup({ request, env });
      else response = await env.ASSETS.fetch(request);
    } catch {
      response = new Response('Upstream unavailable', { status: 502 });
    }
    // Pages applies _headers to static assets only. Keep the same policy on Worker responses.
    const protectedResponse = new Response(response.body, response);
    protectedResponse.headers.set('cache-control', 'no-store');
    protectedResponse.headers.set('x-content-type-options', 'nosniff');
    protectedResponse.headers.set('referrer-policy', 'no-referrer');
    protectedResponse.headers.set('x-frame-options', 'DENY');
    protectedResponse.headers.append('content-security-policy', "frame-ancestors 'none'");
    return protectedResponse;
  },
};
