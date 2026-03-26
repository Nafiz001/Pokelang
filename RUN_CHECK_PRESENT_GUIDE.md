# PokemonLang Final Guide (Run, Verify, Input/Output, Teacher Demo)

## 1) What this project now includes

1. Lexical analysis with Flex.
2. Syntax + semantic analysis with Bison.
3. Runtime execution path (PokemonLang -> C -> executable).
4. Intermediate code generation (`intermediate_code.ir`) during compile checks.

## 2) Project files you should use

1. `pokelang.l` - standalone lexer rules.
2. `pokelang_parser.l` - lexer integrated with parser.
3. `pokelang.y` - parser + semantic checks + IR generation.
4. `build.ps1` - builds lexer and compiler executables.
5. `run_program.ps1` - transpiles and executes PokemonLang programs.
6. `simple_test.poke`, `hello_world_test.poke`, `comprehensive_test.poke` - sample inputs.
7. `rubrics.md` - rubric checklist.
8. `RUBRIC_IMPLEMENTATION_REPORT.md` - implemented rubric status.

## 3) Prerequisites

1. Flex
2. Bison (`win_bison`)
3. GCC (`gcc`)
4. PowerShell

## 4) Build and verify (Compiler path)

Run in project root:

```powershell
.\build.ps1
```

Expected output:

1. `pokelang_lexer.exe` created
2. `pokelang_compiler.exe` created

Then verify syntax + semantic checks:

```powershell
.\pokelang_compiler.exe .\simple_test.poke
.\pokelang_compiler.exe .\comprehensive_test.poke
```

Expected result:

1. Compilation success message
2. Semantic warnings may appear for implicit conversions (expected and useful)
3. `intermediate_code.ir` generated

## 5) Run with real execution behavior (Runtime path)

This is the strongest demo for loops, conditionals, function returns, and actual output.

```powershell
.\run_program.ps1 -InputFile .\hello_world_test.poke -Run
.\run_program.ps1 -InputFile .\simple_test.poke -Run
.\run_program.ps1 -InputFile .\comprehensive_test.poke -Run
```

What happens:

1. PokemonLang file is transpiled to `generated_program.c`
2. C code is compiled to `generated_program.exe`
3. Executable runs and prints program output

## 6) How to give input and get output

### A) Provide input program

1. Create or edit a `.poke` file (example: `my_test.poke`).
2. Write PokemonLang code using your language keywords.

### B) Get compiler/semantic output

```powershell
.\pokelang_compiler.exe .\my_test.poke
```

You get:

1. Syntax errors (if grammar breaks)
2. Semantic errors (undeclared variable, invalid assignment, return mismatch)
3. Semantic warnings (implicit conversion, etc.)

### C) Get runtime output

```powershell
.\run_program.ps1 -InputFile .\my_test.poke -Run
```

You get:

1. Real printed output from `pikapika`
2. Final behavior of conditions/loops/functions

## 7) How to check if it works or not (quick checklist)

1. Build succeeds with no fatal errors (`.\build.ps1`).
2. Compiler accepts known-valid file (`simple_test.poke`).
3. Compiler rejects intentionally broken file with proper line-based errors.
4. Runtime script executes sample files and prints expected messages.
5. IR file is generated during compile checks.

## 8) Teacher explanation script (what to say)

Use this exact flow during presentation:

1. "This project has three phases: lexical analysis, syntax/semantic analysis, and runtime execution."
2. "First, I run `build.ps1` to generate lexer and parser executables."
3. "Then I run `pokelang_compiler.exe` on a `.poke` file to prove grammar + semantic checks."
4. "Semantic layer checks declarations, assignment type compatibility, function calls, and return types."
5. "For real behavior validation, I run `run_program.ps1 -Run` which transpiles to C and executes."
6. "This proves loops, if/else, functions, and output behavior actually work end-to-end."
7. "As an advanced feature, compiler also emits intermediate code (`intermediate_code.ir`)."

## 9) 3-minute live demo plan

1. Build:

```powershell
.\build.ps1
```

2. Compile check:

```powershell
.\pokelang_compiler.exe .\simple_test.poke
```

3. Runtime execution:

```powershell
.\run_program.ps1 -InputFile .\simple_test.poke -Run
```

4. Full feature run:

```powershell
.\run_program.ps1 -InputFile .\comprehensive_test.poke -Run
```

5. Show generated IR file:

```powershell
Get-Content .\intermediate_code.ir | Select-Object -First 20
```

## 10) If something fails

1. Re-run build: `.\build.ps1`
2. Confirm tool install: `gcc --version`, `flex --version`, `win_bison --version`
3. Check `.poke` grammar tokens (`dot`, `fob`, `fcb`, `OpenBall`, `CloseBall`)
4. Fix first reported error line, then run again
