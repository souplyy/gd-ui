#!/usr/bin/env python3
"""Convert an official Lucide SVG to native stroke data (pip install svgpathtools)."""
import argparse
import json
import math
from pathlib import Path
from svgpathtools import svg2paths

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('svg', type=Path)
parser.add_argument('output', type=Path)
args = parser.parse_args()
paths, _ = svg2paths(str(args.svg))
lines = []
for path in paths:
    for subpath in path.continuous_subpaths():
        points = []
        for segment in subpath:
            steps = max(2, math.ceil(segment.length() * 3))
            for i in range(steps):
                point = segment.point(i / steps)
                points.append([round(point.real, 4), round(point.imag, 4)])
        endpoint = subpath[-1].end
        points.append([round(endpoint.real, 4), round(endpoint.imag, 4)])
        lines.append(points)
args.output.write_text(json.dumps(lines, separators=(',', ':')) + '\n')
