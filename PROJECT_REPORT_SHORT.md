# PokemonLang Project Report 

## Project Summary
PokemonLang is a custom programming language inspired by Pokemon-themed keywords.
The project includes:
- Lexical analysis with Flex
- Syntax and semantic analysis with Bison
- Intermediate code generation (`intermediate_code.ir`)
- Runtime execution support through C transpilation
- A localhost web interface for writing and running code

## Completed Work
- Implemented keyword/token rules and parser grammar
- Added semantic checks (type and declaration validation)
- Added explainability trace mode (token, reduce, semantic, IR)
- Added indentation checking and sample programs
- Added web IDE features (Run, Trace, sample loader)

## Highlighted Exceptions (Important)
1. `flex -o` option failed in this environment.
   - Exception: Installed Flex did not support the expected output option usage.
   - Fix: Used `flex <file>` and default `lex.yy.c` generation.

2. Linker error: multiple definition of `main`.
   - Exception: `lex.yy.c` from standalone lexer was linked with parser output.
   - Fix: Re-generated `lex.yy.c` from `pokelang_parser.l` before building compiler.

3. Web sample If/Else load error.
   - Exception: Variable name `hp` conflicted with language token (`hp` division operator).
   - Fix: Renamed sample variable to `healthPoints`.

4. Port conflict on `3000`.
   - Exception: Default local port already in use.
   - Fix: Shifted web server default to `3001`.

## Current Status
Project is functional for compile, semantic validation, tracing, and local web execution.
Core rubric requirements are implemented and demo-ready.