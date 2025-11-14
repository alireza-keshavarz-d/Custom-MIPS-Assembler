# MIPS Assembler
**A two-pass C++ assembler for a simplified MIPS-like CPU**

Part of the *Computer Architecture* course at **Hormozgan University** - Fall 2025

---

## Features

- **Two-pass assembly** with symbol resolution
- Supports **R-type** and **I-type** instructions
- Handles **labels**, **sections** (`.text`, `.data`), and **`.word` directives**
- Generates **separate instruction and data memory** outputs
- Outputs **32-bit binary words in text format** (binary string per line)

---

## Supported Instructions

| Type | Mnemonics |
|------|-----------|
| **R-type** | `add`, `sub`, `and`, `or`, `mult`, `sll`, `srl`, `not` |
| **I-type** | `lw`, `sw`, `beq` |

> Register set: `a0-a7`, `r0-r7`, `s0-s7`, `t0-t7` (32 total)

---

## Build Instructions

### Prerequisites
- **C++20** compiler (GCC, Clang, MSVC)
- **CMake** ≥ 3.12

### Build Steps
```bash
git clone https://github.com/alumen2101/Custom-MIPS-Assembler.git
cd Custom-MIPS-Assembler
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

The executable will be in the `build/` directory:
- Linux/macOS: `assembler`
- Windows: `assembler.exe`

---

## Usage
```bash
./assembler <input.asm> <inst_template.vhd> <inst_output.vhd> <data_template.vhd> <data_output.vhd>
```

### Example
```asm
.text
main:   
    lw   a0, 0(r0)
    lw   a1, 4(r0)

    add  t0, a0, a1

    sw   t0, 8(r0)

halt:   
    beq  r0, r0, halt

.data
    .word 7
    .word 5
    .word 0
```

will result in (not templated)

IM
```
00000000000000000000000000100011   # lw a0, 0(r0)
00000000000000000001000000100011   # lw a1, 4(r0)
00000000000100001000011000000000   # add t0, a0, a1
10101100000011000000000000001000   # sw t0, 8(r0)
00010000000000000000100000000000   # beq r0,r0,halt
```

DM
```
00000000000000000000000000000111   # 7
00000000000000000000000000000101   # 5
00000000000000000000000000000000   # placeholder (overwritten later)
```

---

## Output Format

Each line = one **32-bit word** in **binary string format**
- Instruction memory: `.text` section
- Data memory: `.data` section
- Big-endian encoding

---

## GitHub Releases

TBA ...

---

## Project Structure

```
.
├── src/             # Source files (.cpp, .h)
├── CMakeLists.txt   # Cross-platform build
├── .github/         # GitHub Actions CI/CD
├── templates/       # tempaltes for DM and IM vhdl files
└── README.md
```

---

## Contributing

Contributions are welcome!
1. Fork the repo
2. Create a feature branch
3. Submit a pull request

---

## License

MIT License – See [LICENSE](LICENSE) for details.

---

**Hormozgan University – Computer Architecture – Fall 2025**  
*Project by Keshavarz*