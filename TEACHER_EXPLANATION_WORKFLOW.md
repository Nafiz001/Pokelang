# PokemonLang: Full Workflow + Rubric Implementation Explanation

This file is designed for presentation and viva. It explains:

1. End-to-end compiler workflow
2. What happens to a sample `.poke` file at each stage
3. Where each rubric item is implemented
4. How to answer common teacher questions

---

## 1) Big Picture: What this project is

PokemonLang is a custom language compiler project with three practical execution paths:

1. **Lexical analysis** (Flex)
2. **Syntax + semantic analysis** (Bison + semantic actions)
3. **Runtime behavior demonstration** (PokemonLang -> C transpile -> GCC executable)

So, the project supports both:

1. **Compiler correctness checks** (syntax/semantic)
2. **Real execution behavior demo** (runtime output)

---

## 2) Important files and what each one does

1. `pokelang.l`
   - Standalone lexer
   - Converts source text into token stream output
   - Has its own `main` for lexer-only mode

2. `pokelang_parser.l`
   - Lexer integrated with parser
   - Returns tokens directly to Bison parser

3. `pokelang.y`
   - Bison grammar + semantic logic + IR generation
   - Parses structure and executes semantic checks

4. `run_program.ps1`
   - Runtime demonstration path
   - Converts PokemonLang keywords to C syntax
   - Compiles generated C with GCC and runs executable

5. `web/server.js`
   - Localhost backend for browser editor
   - Receives code and executes pipeline

6. `web/public/*`
   - Frontend editor UI (write code, click run, see output)

7. `rubrics.md`
   - Teacher rubric requirements list

---

## 3) End-to-end workflow using a sample file

Use: `simple_test.poke`

### Step A: Source file input

You write PokemonLang syntax such as:

1. `Move EmptyBall StartBattle fob fcb OpenBall`
2. `Pikachu health evo 100 dot`
3. `Brock ... Misty ... Jessie ...`

This is custom language text (not C).

### Step B: Lexical analysis (tokenization)

Handled by Flex rules (`pokelang_parser.l` for parser mode).

What happens:

1. Keywords are recognized and mapped to token IDs
   - Example: `Pikachu` -> `INT`
   - `Brock` -> `IF`
   - `evo` -> `ASSIGN`
   - `dot` -> `SEMICOLON`
2. Literals are parsed
   - integers, floats, strings, chars
3. Invalid characters trigger lexical errors with line number

Output at this stage (conceptually): stream of tokens.

### Step C: Syntax analysis (grammar validation)

Handled by Bison grammar in `pokelang.y`.

What happens:

1. Parser checks if token sequence matches grammar rules
2. Confirms proper constructs:
   - function declaration
   - variable declarations
   - expression forms
   - if/else chains
   - loops, switch-case
3. If structure is wrong, parser reports syntax error with line number

### Step D: Semantic analysis (meaning-level checks)

Also in `pokelang.y` via semantic actions and helper C code.

What happens:

1. Symbol table tracks variables
2. Function table tracks function signatures
3. Checks performed:
   - undeclared identifier use
   - redeclaration
   - assignment type compatibility
   - implicit conversion warnings
   - function argument count validation
   - return type correctness
4. Math and expression semantics are evaluated for correctness checks

Result:

1. Semantic errors stop successful compilation
2. Semantic warnings are shown but compilation can still pass

### Step E: Intermediate representation generation

During semantic pass, IR lines are written to:

1. `intermediate_code.ir`

This demonstrates an advanced compiler feature (rubric bonus).

### Step F: Runtime behavior demonstration path

For live output behavior, `run_program.ps1` does:

1. Read PokemonLang source
2. Remove comments
3. Replace PokemonLang keywords/operators with C equivalents
4. Inject helper runtime function(s) (e.g., `primechu`)
5. Write `generated_program.c`
6. Compile with `gcc ... -lm`
7. Run `generated_program.exe`

This proves loops/conditions/functions actually execute and produce output.

### 3.1 Exact walkthrough: for-loop counting 1 to 10 (with file + line references)

Use sample file:

1. `samples/sample_for_count_1_to_10.poke`

Important lines in sample:

1. Line 2: `Pikachu i evo 0 dot` (declare counter)
2. Line 4: `AshTrain fob i evo 1 dot i <= 10 dot i evo i atk 1 fcb OpenBall` (for-loop)
3. Line 7: `Brock fob i == 11 fcb OpenBall` (post-loop check)
4. Line 8: success print
5. Line 13: `back 0 dot`

#### Command map (which command is doing what)

1. `win_bison -d .\pokelang.y`
   - Generates parser C/H files from grammar (`pokelang.tab.c`, `pokelang.tab.h`).

2. `flex .\pokelang_parser.l`
   - Generates integrated lexer (`lex.yy.c`) that returns tokens to parser.

3. `gcc .\pokelang.tab.c .\lex.yy.c -o .\pokelang_compiler.exe -lm`
   - Builds compiler executable for syntax + semantic + IR + trace flow.

4. `pokelang_compiler.exe .\samples\sample_for_count_1_to_10.poke --trace`
   - Runs explainability pipeline:
     token stream -> grammar reductions -> semantic actions -> IR lines.

5. `run_program.ps1`
   - Output execution path (runtime behavior demonstration).

#### Stage 1: Lexer tokenization (where it happens)

In `pokelang_parser.l`:

1. Line 62: `AshTrain` -> `FOR`
2. Line 108: `evo` -> `ASSIGN`
3. Line 103: `atk` -> `PLUS`
4. Line 114: `dot` -> `SEMICOLON`
5. Line 129: integer literals -> `INTEGER_LITERAL`
6. Line 167: names like `i` -> `IDENTIFIER`
7. Line 16 and line 21: token trace function + macro (`[TRACE][TOKEN]`)

So the loop header at line 4 is tokenized into this conceptual pattern:

`FOR LPAREN IDENTIFIER ASSIGN INTEGER_LITERAL SEMICOLON IDENTIFIER LESS_EQUAL INTEGER_LITERAL SEMICOLON IDENTIFIER ASSIGN IDENTIFIER PLUS INTEGER_LITERAL RPAREN LBRACE ... RBRACE`

#### Stage 2: Parser grammar reduction for for-loop

In `pokelang.y`:

1. Line 769 starts `iteration_statement`
2. Line 785-790 handles `FOR (init; condition; update) statement`
3. Line 787 emits reduction trace:
   `iteration_statement -> FOR (init;cond;update) statement`

Also related grammar:

1. Line 841 starts `assignment_expression`
2. Line 844 traces simple expression reductions
3. Line 847-855 handles assignment reductions (`i evo ...`)

#### Stage 3: Semantic actions for counter updates

In `pokelang.y`:

1. Line 208 (`declare_symbol`) creates symbol table entry for `i`
2. Line 261 (`assign_symbol`) applies updates like `i = 1`, `i = i + 1`
3. Line 228 and line 290 emit semantic traces (`[TRACE][SEM] ...`)

This is why trace output shows:

1. `declare symbol name=i type=Pikachu array=0`
2. `assign i <= ...`

#### Stage 4: IR lines emitted during this sample

In `pokelang.y`:

1. Line 121 `emit_ir(...)` writes IR lines
2. Line 230 emits declaration IR (`DECL ...`)
3. Line 292 emits assignment IR (`i = ...`)
4. Line 444 emits print IR (`PRINT ...`)

Generated file:

1. `intermediate_code.ir`

#### Stage 5: Explainability mode entry point

In `pokelang.y`:

1. Line 1145: usage supports `[--trace]`
2. Line 1150: detects `--trace` / `-t`
3. Line 1171-1172: prints trace stage banner

Run command:

1. `pokelang_compiler.exe .\samples\sample_for_count_1_to_10.poke --trace`

#### Stage 6: Runtime execution path for true loop behavior

Compiler trace mode is great for explainability, but true loop behavior is shown in runtime path:

In `run_program.ps1`:

1. Line 47 maps `AshTrain` -> `for`
2. Line 108/103/114 equivalents in transpiler map:
   - line 77 `evo` -> `=`
   - line 72 `atk` -> `+`
   - line 82 `dot` -> `;`
3. Line 118 compiles generated C using `gcc`
4. Line 126+ runs executable and prints output block

Run command:

1. `run_program.ps1`

Expected output includes:

1. `Counted 1 to 10 successfully`

#### Viva note (important)

1. `--trace` mode in parser shows semantic/reduction flow (analysis path).
2. Runtime script demonstrates actual execution path (behavior path).
3. Show both to answer "how do you know it really executes?" confidently.

---

## 4) Detailed walkthrough of one sample line

Example line in PokemonLang:

`Pikachu health evo 100 dot`

How it flows:

1. Lexer sees `Pikachu` -> token `INT`
2. Lexer sees `health` -> token `IDENTIFIER`
3. Lexer sees `evo` -> token `ASSIGN`
4. Lexer sees `100` -> token `INTEGER_LITERAL`
5. Lexer sees `dot` -> token `SEMICOLON`

Parser reduces this to variable declaration with initializer.

Semantic action:

1. Declares symbol `health` with type int
2. Validates assignment compatibility
3. Stores initialized value in symbol table
4. Emits IR declaration/assignment line

Runtime path (if used):

1. Transpiles to C equivalent: `int health = 100;`
2. C compiler compiles it
3. Executable uses it in actual control flow

---

## 5) Rubric mapping: where and how each is implemented

### 5.1 Lexical Analysis using Flex

Implemented in:

1. `pokelang.l`
2. `pokelang_parser.l`

How:

1. Regex patterns define tokens for keywords, operators, literals
2. Whitespace/comments ignored
3. Invalid token handler prints errors with line number

### 5.2 Token Definitions

Implemented in:

1. Token declarations in `pokelang.y`
2. Matching return rules in `pokelang_parser.l`

How:

1. Every language keyword/operator maps to a named token
2. Lexer and parser token sets are aligned

### 5.3 Handling Invalid Tokens

Implemented in:

1. Catch-all rule `.` in lexer files

How:

1. Reports invalid character and line number
2. Prevents silent lexical failures

### 5.4 Integration with Bison

Implemented in:

1. `pokelang_parser.l` includes parser header
2. `pokelang.y` consumes tokens from lexer

How:

1. Lexer returns token IDs + semantic values via `yylval`
2. Bison grammar uses them for parsing and semantic actions

### 5.5 Syntax Analysis (well-defined grammar)

Implemented in:

1. Grammar sections in `pokelang.y`

How:

1. Rules for program, declarations, statements, expressions, control flow
2. Precedence/associativity to resolve expression ambiguity

### 5.6 Syntax Error Handling

Implemented in:

1. `yyerror()` in `pokelang.y`

How:

1. Line-aware error messages
2. Syntax error counter and result summary

### 5.7 Type Checking, Implicit Conversion, Variable Declaration

Implemented in:

1. Semantic helper functions in `pokelang.y`

How:

1. Symbol table stores type and initialization state
2. `can_assign(...)` validates type compatibility
3. Implicit numeric conversions produce warnings
4. Undeclared and redeclared names produce errors

### 5.8 Correctness of execution behavior

Implemented in two layers:

1. Semantic-evaluation checks in `pokelang.y`
2. Real runtime execution via `run_program.ps1` transpile+run

How:

1. Compiler validates structure and meaning
2. Runtime path executes generated C for observable behavior

### 5.9 Expression Evaluation

Implemented in:

1. Expression grammar + evaluation helpers in `pokelang.y`

How:

1. Arithmetic/logical/relational operations supported
2. Division/modulo edge checks (e.g., divide by zero) handled

### 5.10 Conditional Statements (if/else)

Implemented in:

1. `selection_statement` grammar in `pokelang.y`

How:

1. Parses `Brock`, `Misty`, `Jessie` constructs
2. Condition semantics validated
3. Runtime path demonstrates actual branching output

### 5.11 Loops

Implemented in:

1. `iteration_statement` grammar in `pokelang.y`

How:

1. Supports `AshTrain`, `WildBattle`, `RetryBattle`
2. Runtime path shows real repeated execution

### 5.12 Functions

Implemented in:

1. Function grammar + function table in `pokelang.y`

How:

1. Registers function names, return types, arg counts
2. Validates call argument count and return type behavior

### 5.13 Unique/Advanced Feature

Implemented in:

1. IR generation to `intermediate_code.ir`

How:

1. Declaration, assignment, print/function-related operations emit intermediate lines
2. Demonstrates compiler-design extension beyond basic parsing

### 5.14 Simple Indentation Rule (exceptional language feature)

Implemented in:

1. `pokelang_parser.l` (line-start indentation tracking and checks)
2. `pokelang.y` (compile result reporting via indentation error counter)

Rule:

1. Inside `OpenBall ... CloseBall`, every non-`CloseBall` line must be indented by at least 4 spaces per block depth.

How it works:

1. Lexer tracks line-start indentation (`current_indent`) and brace depth.
2. Before returning each token, lexer validates indentation for that line.
3. Violations print `Indentation error at line ...` and increment indentation error count.
4. Compiler result fails if indentation error count is non-zero.

---

## 6) Command workflow for demonstration (teacher-friendly)

### 6.1 Build manually (raw)

    Set-Location D:\VS\PokeLang
    Remove-Item .\lex.yy.c, .\pokelang.tab.c, .\pokelang.tab.h, .\pokelang_compiler.exe, .\pokelang_lexer.exe -ErrorAction SilentlyContinue
    flex .\pokelang.l
    gcc .\lex.yy.c -o .\pokelang_lexer.exe
    win_bison -d .\pokelang.y
    flex .\pokelang_parser.l
    gcc .\pokelang.tab.c .\lex.yy.c -o .\pokelang_compiler.exe -lm

### 6.2 Run compiler checks

    .\pokelang_compiler.exe .\simple_test.poke
    .\pokelang_compiler.exe .\comprehensive_test.poke

### 6.2.1 Run Explainability Step Trace (for viva)

   .\pokelang_compiler.exe .\simple_test.poke --trace

Trace order shown in output:

1. `[TRACE][TOKEN]` token stream from lexer
2. `[TRACE][REDUCE]` grammar reductions from parser
3. `[TRACE][SEM]` semantic actions (declare/assign/call/context)
4. `[TRACE][IR]` intermediate representation lines as they are emitted

### 6.3 Show IR output

    Get-Content .\intermediate_code.ir | Select-Object -First 20

### 6.4 Run real runtime behavior

    .\run_program.ps1 -InputFile .\simple_test.poke -Run

### 6.5 Show localhost UI

    node .\web\server.js

Open browser:

    http://127.0.0.1:3001

---

## 7) Viva questions and strong answers

### Q1: Why do you have two lexer files?

Answer:

1. `pokelang.l` is standalone for lexical-only output/testing.
2. `pokelang_parser.l` is parser-integrated and returns tokens to Bison.
3. This separation avoids mixing standalone `main()` with parser-linked build.

### Q2: How is semantic analysis different from syntax analysis in your project?

Answer:

1. Syntax checks grammar form (structure correctness).
2. Semantic checks meaning (types, declaration validity, function call correctness, return type compatibility).

### Q3: Where is type checking implemented?

Answer:

1. In semantic helper functions inside `pokelang.y`.
2. Assignment and return operations call compatibility checks.
3. Implicit conversions are allowed for numeric types but reported as warnings.

### Q4: How do you prove loops and conditionals actually execute?

Answer:

1. Compiler path proves syntax+semantic validity.
2. Runtime transpile path (`run_program.ps1`) compiles equivalent C and executes it.
3. Output differences across branches/iterations prove actual runtime behavior.

### Q5: What is your advanced feature?

Answer:

1. Intermediate code generation (`intermediate_code.ir`).
2. It logs core operations and demonstrates a compiler pipeline stage beyond parsing.

### Q6: Why do some runs show warnings but still pass?

Answer:

1. Warnings represent legal but potentially unsafe behavior (e.g., implicit conversion).
2. Errors represent invalid semantics and fail compilation.

### Q7: What caused the `multiple definition of main` issue and how was it fixed?

Answer:

1. It happens when `lex.yy.c` came from standalone `pokelang.l` and is linked with parser output.
2. Fix: regenerate `lex.yy.c` from `pokelang_parser.l` before building `pokelang_compiler.exe`.

---

## 8) Short explanation script (30-45 seconds)

"PokemonLang compiler has lexer, parser, and semantic checker stages. Flex tokenizes custom keywords, Bison validates grammar, and semantic actions enforce declaration, type, and function rules. During compile checks it also emits intermediate code. For runtime demonstration, we transpile PokemonLang to C, compile with GCC, and execute to prove loops, conditionals, and functions work end-to-end. This directly maps to rubric requirements for lexical analysis, syntax analysis, semantic correctness, execution behavior, and advanced features."

---

## 9) Final takeaway

If teacher asks "Is this only parser output?", answer:

1. No. It includes parsing + semantic analysis + runtime demonstration + intermediate code generation.
2. So both theoretical compiler stages and practical execution behavior are covered.
