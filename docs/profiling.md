# Profiling Ray-Invaders (Linux)

This guide shows how to capture CPU profiles to find hotspots and verify performance improvements.

## Build Release

Use the optimized build for realistic profiling:

- `make release`

## Profile with perf (recommended)

1. Record:
   - `make profile`
   - This launches the game under `perf`. Play for a bit; press Ctrl+C to stop.
2. Report:
   - `make perf-report`
   - Navigate the interactive report to inspect top functions and call graphs.

Tips:
- Ensure `dwarf` call-graph support is installed (`linux-tools` matching your kernel).
- You can pass extra args to `perf` by running it directly if needed.

## Profile with Callgrind (alternative)

1. Record:
   - `make callgrind`
   - This is much slower; use briefly and quit after reproducing the scene.
2. Analyze:
   - Use `kcachegrind` or `qcachegrind` to open the produced `callgrind.out.*` file.

## What to look for

- CPU time in collision/update loops (Base, Player, EnemyManager)
- Per-frame allocations or vector growth
- Hot draw paths or excessive state changes

Commit notes:
- Profiling artifacts are ignored by `.gitignore` (callgrind/perf outputs).
