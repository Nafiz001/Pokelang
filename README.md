# PokemonLang Compiler

Current project state is focused on a clean run path for rubric demonstration.

## Main commands

```powershell
.\build.ps1
.\pokelang_compiler.exe .\simple_test.poke
.\run_program.ps1 -InputFile .\simple_test.poke -Run
```

## Localhost web studio

```powershell
node .\web\server.js
```

Then open:

```text
http://127.0.0.1:3001
```

## Primary guide

Use `RUN_CHECK_PRESENT_GUIDE.md` for:

1. How to run and verify
2. How to give input and get output
3. Teacher explanation script
4. Live demo plan

## Important files

1. `pokelang.l`
2. `pokelang_parser.l`
3. `pokelang.y`
4. `build.ps1`
5. `run_program.ps1`
6. `simple_test.poke`
7. `comprehensive_test.poke`
8. `hello_world_test.poke`
