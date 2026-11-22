/**
 * Browser `#lib/ui/theme`. The console is plain text, so every role is the identity; the palette
 * and symbols keep upstream's values for anything that reads them.
 */
type Style = (text: string) => string;
const plain: Style = (text) => text;

export function supportsColor(): boolean {
  return false;
}

export const theme: Record<
  | 'brand'
  | 'success'
  | 'error'
  | 'warning'
  | 'info'
  | 'muted'
  | 'code'
  | 'jsonKey'
  | 'jsonString'
  | 'jsonNumber'
  | 'jsonBoolean'
  | 'bold'
  | 'italic',
  Style
> = {
  brand: plain,
  success: plain,
  error: plain,
  warning: plain,
  info: plain,
  muted: plain,
  code: plain,
  jsonKey: plain,
  jsonString: plain,
  jsonNumber: plain,
  jsonBoolean: plain,
  bold: plain,
  italic: plain,
};

export const colorPalette = {
  brand: '#BF6600',
  success: '#3A8E52',
  error: '#D45350',
  warning: '#BF6600',
  info: '#3D82BB',
  muted: '#7F7F7F',
  code: '#3D82BB',
  jsonKey: '#3D82BB',
  jsonString: '#3A8E52',
  jsonNumber: '#BF6600',
  jsonBoolean: '#926CC1',
} as const;

export const symbols = { success: '✓', error: '✗', warning: '⚠', info: 'ℹ', arrow: '→' } as const;
