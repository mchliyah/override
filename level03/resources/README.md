
Conceptual resources for Level03

This folder contains links and references about the concepts used in `level03` (reverse engineering jump-tables and numeric constraint analysis). Use the commands in the original walkthrough when testing in a 32-bit VM — no helper scripts are required.

Recommended reading:

- Jump tables and switch-case reverse engineering: https://en.wikipedia.org/wiki/Jump_table
- Basic reverse engineering techniques: https://en.wikibooks.org/wiki/X86_Disassembly
- Assembly numeric analysis / arithmetic: https://en.wikipedia.org/wiki/Assembly_language

Example (run inside a 32-bit VM):

```bash
# use a valid password from the walkthrough range (e.g. 322424827)
printf "322424827\n" | ./level03
```

