#!/usr/bin/env python3
"""Publish UI data atomically; the running mod refreshes it within 0.5 seconds."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import tempfile
import time

root = Path(__file__).resolve().parents[1]
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--config-dir', type=Path, default=Path.home() / 'Library/Application Support/Steam/steamapps/common/Geometry Dash/Geometry Dash.app/Contents/geode/config/souplyy.gd-ui')
parser.add_argument('--watch', action='store_true', help='Automatically publish resources/ui.json whenever it changes')
args = parser.parse_args()

def publish():
    content = (root / 'resources/ui.json').read_text()
    data = json.loads(content)
    data["icon_revision"] = hashlib.sha256((root / "resources/icons/settings.svg").read_bytes()).hexdigest()
    content = json.dumps(data, indent=2) + "\n"
    if not (10 <= data['height'] <= 48 and 10 <= data['button_size'] <= data['height'] and 8 <= data['icon_size'] <= data['button_size']):
        raise ValueError('Invalid toolbar dimensions')
    args.config_dir.mkdir(parents=True, exist_ok=True)
    # Publish the original SVG icon before its revision in the UI config.
    with tempfile.NamedTemporaryFile(mode='wb', dir=args.config_dir, delete=False) as image:
        image.write((root / 'resources/icons/settings.svg').read_bytes())
        staged_image = Path(image.name)
    try:
        os.replace(staged_image, args.config_dir / 'settings.svg')
    finally:
        staged_image.unlink(missing_ok=True)
    with tempfile.NamedTemporaryFile(mode='w', dir=args.config_dir, delete=False) as output:
        output.write(content)
        staged = Path(output.name)
    try:
        os.replace(staged, args.config_dir / 'ui.json')
    finally:
        staged.unlink(missing_ok=True)
    print('Published UI. The running top bar will refresh automatically.', flush=True)

if args.watch:
    previous = None
    print('Watching resources/ui.json for live updates.', flush=True)
    while True:
        current = ((root / 'resources/ui.json').read_bytes(), (root / 'resources/icons/settings.svg').read_bytes())
        if current != previous:
            try:
                publish()
                previous = current
            except (ValueError, OSError, KeyError) as error:
                print(f'Not published: {error}', flush=True)
                previous = current
        time.sleep(0.5)
else:
    publish()
