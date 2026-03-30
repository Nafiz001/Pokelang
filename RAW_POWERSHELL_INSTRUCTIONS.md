# PokemonLang Raw PowerShell Instructions (No build.ps1, No run_program.ps1)

This guide shows exactly how to run the project with direct PowerShell commands only.

## 1. Open terminal in project root

Run:

    Set-Location D:\VS\PokeLang

## 2. Check required tools

Run:

    gcc --version
    flex --version
    win_bison --version
    node --version

If all commands print versions, environment is ready.

## 3. Build compiler manually (raw commands)

3.1 Clean old generated files (optional but recommended):

    Remove-Item .\lex.yy.c, .\pokelang.tab.c, .\pokelang.tab.h, .\pokelang_compiler.exe, .\pokelang_lexer.exe -ErrorAction SilentlyContinue

3.2 Build standalone lexer:

    flex .\pokelang.l
    gcc .\lex.yy.c -o .\pokelang_lexer.exe

3.3 Build parser + integrated lexer compiler:

    win_bison -d .\pokelang.y
    flex .\pokelang_parser.l
    gcc .\pokelang.tab.c .\lex.yy.c -o .\pokelang_compiler.exe -lm

Note for this environment:

1. Your installed `C:\Program Files (x86)\GnuWin32\bin\flex.exe` fails with `-o` output option in this workspace.
2. Use plain `flex <file>` so it generates default `lex.yy.c`.
3. For `pokelang_compiler.exe`, `lex.yy.c` MUST come from `pokelang_parser.l` (not `pokelang.l`).

## 4. Run syntax + semantic checks

Run with existing sample files:

    .\pokelang_compiler.exe .\simple_test.poke
    .\pokelang_compiler.exe .\comprehensive_test.poke

Run with your own file:

    .\pokelang_compiler.exe .\my_test.poke

Expected behavior:

1. Success output when syntax and semantics are valid.
2. Syntax errors with line numbers for grammar problems.
3. Semantic errors/warnings for type and declaration issues.
4. Intermediate representation output file:

    .\intermediate_code.ir

## 5. Tokenize source (lexer-only output)

Build standalone lexer first (section 3.2), then run:

    .\pokelang_lexer.exe .\simple_test.poke tokens.txt
    Get-Content .\tokens.txt

## 6. Manual runtime execution (raw commands, no run_program.ps1)

This section manually transpiles PokemonLang to C, compiles, then executes.

6.1 Choose input file:

    $InputFile = ".\simple_test.poke"

6.2 Load source and remove comments:

    $content = Get-Content $InputFile -Raw
    $content = [regex]::Replace($content, '\$\*.*?\*\$', '', [System.Text.RegularExpressions.RegexOptions]::Singleline)
    $content = [regex]::Replace($content, '\$\$.*$', '', [System.Text.RegularExpressions.RegexOptions]::Multiline)

6.3 Convert switch/case forms first:

    $content = [regex]::Replace($content, '\bTypeBadge\s+([^\r\n]+?)\s+dot\b', 'case $1:')
    $content = [regex]::Replace($content, '\bDefaultBadge\s+dot\b', 'default:')

6.4 Apply keyword/operator replacements:

    $replacements = [ordered]@{
        '\bProfessorOak\b' = '#include'
        '\bPokeDefine\b' = '#define'
        '\bMove\b' = ''
        '\bStartBattle\b' = 'main'
        '\bPikachu\b' = 'int'
        '\bCharizard\b' = 'float'
        '\bMewtwo\b' = 'double'
        '\bSnorlax\b' = 'long'
        '\bEevee\b' = 'char'
        '\bTogepi\b' = 'int'
        '\bEmptyBall\b' = 'void'
        '\bBrock\b' = 'if'
        '\bMisty\b' = 'else if'
        '\bJessie\b' = 'else'
        '\bAshTrain\b' = 'for'
        '\bWildBattle\b' = 'while'
        '\bRetryBattle\b' = 'do'
        '\bHandleGym\b' = 'switch'
        '\bRunAway\b' = 'break'
        '\bKeepFighting\b' = 'continue'
        '\bback\b' = 'return'
        '\bpikapika\b' = 'printf'
        '\bpokecatch\b' = 'scanf'
        '\bpowerup\b' = 'pow'
        '\brootchu\b' = 'sqrt'
        '\bfloorchu\b' = 'floor'
        '\bceilchu\b' = 'ceil'
        '\bheal\b' = 'fabs'
        '\bsinchu\b' = 'sin'
        '\bcoschu\b' = 'cos'
        '\btanchu\b' = 'tan'
        '\basinchu\b' = 'asin'
        '\bacoschu\b' = 'acos'
        '\batanchu\b' = 'atan'
        '\bprimechu\b' = 'primechu'
        '\batk\b' = '+'
        '\bdmg\b' = '-'
        '\bcrit\b' = '&&'
        '\bteam\b' = '||'
        '\bconfuse\b' = '!'
        '\bevo\b' = '='
        '\bmod\b' = '%'
        '\bx\b' = '*'
        '\bhp\b' = '/'
        '\bdot\b' = ';'
        '\bfob\b' = '('
        '\bfcb\b' = ')'
        '\bOpenBall\b' = '{'
        '\bCloseBall\b' = '}'
    }

    foreach ($pattern in $replacements.Keys) {
        $content = [regex]::Replace($content, $pattern, $replacements[$pattern])
    }

    $content = [regex]::Replace($content, '\bvoid\s+main\s*\(', 'int main(')

6.5 Add runtime header and write C output:

    $runtimeHeader = @"
    #include <stdio.h>
    #include <stdlib.h>
    #include <math.h>

    int primechu(double n) {
        int i;
        int v = (int)n;
        if (v < 2) return 0;
        for (i = 2; i * i <= v; ++i) {
            if (v % i == 0) return 0;
        }
        return 1;
    }

    "@

    Set-Content -Path .\generated_program.c -Value ($runtimeHeader + $content) -Encoding UTF8

6.6 Compile generated C and run:

    gcc .\generated_program.c -o .\generated_program.exe -lm
    .\generated_program.exe

## 7. Run the localhost site

Run server:

    node .\web\server.js

Open browser:

    http://127.0.0.1:3001

Important note:

1. The site backend currently calls run_program.ps1 internally.
2. If you want the site to also avoid run_program.ps1 internally, backend code in web/server.js must be changed to use the manual transpile pipeline directly.

## 8. Quick class/demo command sequence

Use this exact sequence:

    Set-Location D:\VS\PokeLang
    win_bison -d .\pokelang.y
    flex .\pokelang_parser.l
    gcc .\pokelang.tab.c .\lex.yy.c -o .\pokelang_compiler.exe -lm
    .\pokelang_compiler.exe .\simple_test.poke
    .\pokelang_compiler.exe .\comprehensive_test.poke

Optional runtime demo with manual transpile block: run section 6.

## 9. Common linker fix: multiple definition of `main`

If you see:

`multiple definition of 'main' ... lex.yy.c ... pokelang.tab.c`

Reason:

1. You generated `lex.yy.c` from `pokelang.l` and then linked it with `pokelang.tab.c`.
2. `pokelang.l` is standalone lexer and contains its own `main()`.

Fix commands (exact):

    Remove-Item .\lex.yy.c, .\pokelang_compiler.exe -ErrorAction SilentlyContinue
    win_bison -d .\pokelang.y
    flex .\pokelang_parser.l
    gcc .\pokelang.tab.c .\lex.yy.c -o .\pokelang_compiler.exe -lm

Standalone lexer build is separate:

    flex .\pokelang.l
    gcc .\lex.yy.c -o .\pokelang_lexer.exe
