// npm install @resvg/resvg-js in your development environment.
// Render the exact SVG at 512 px; do not trace or simplify the original paths.
const fs = require('node:fs');
const path = require('node:path');
const crypto = require('node:crypto');
const { Resvg } = require('@resvg/resvg-js');
const root = path.resolve(__dirname, '..');
const svg = fs.readFileSync(path.join(root, 'resources/icons/settings.svg'), 'utf8');
const png = new Resvg(svg.replace('currentColor', '#ffffff'), {
  fitTo: { mode: 'width', value: 512 }
}).render().asPng();
fs.writeFileSync(path.join(root, 'resources/icons/settings.png'), png);
const file = path.join(root, 'resources/ui.json');
const config = JSON.parse(fs.readFileSync(file));
config.icon_revision = crypto.createHash('sha256').update(png).digest('hex');
fs.writeFileSync(file, JSON.stringify(config, null, 2) + '\n');
