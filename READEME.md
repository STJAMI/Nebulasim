# NebulaSim

NebulaSim is a simple **C-based interactive space object simulation** that models particles moving, merging, and changing brightness on a 2D grid. The project uses core C concepts: arrays, structs, pointers, file I/O, loops, and conditional logic.

## Features

- Interactive simulation with step-by-step control
- Batch mode to save frames to `steps/stepXXXX.txt`
- Replay mode to view saved frames
- Simple merge/collision rules and brightness based on energy
- Configurable grid size, particle count, and steps

## Build & Run

1. Create folder structure:

NebulaSim/
├── src/
│ ├── main.c
│ ├── nebula.c
│ └── nebula.h
├── steps/ # create this manually
├── Makefile
└── README.md

2. Create `steps/` directory:

- Linux/Mac: `mkdir -p steps`
- Windows (cmd): `mkdir steps`

3. Build:

make

4. Run:

./NebulaSim

## Files

- `src/main.c` — main program and menu
- `src/nebula.c` & `src/nebula.h` — simulation engine
- `steps/` — saved frames (auto-saved by program)
- `Makefile` — build helper

## Notes

- Test in a terminal (Linux/Mac Terminal, Windows PowerShell/CMD).
- Use `batch save` to produce many frames, then use `Replay saved simulation frames` to view them.
