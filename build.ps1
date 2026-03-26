# PokemonLang Compiler Build Script
# Builds both the standalone lexer and the full compiler (lexer + parser)

Write-Host "`n=== PokemonLang Compiler Build System ===" -ForegroundColor Cyan
Write-Host "Building lexer and parser...`n" -ForegroundColor Cyan

# Clean previous builds
Write-Host "[1/5] Cleaning previous builds..." -ForegroundColor Yellow
Remove-Item lex.yy.c, pokelang.tab.c, pokelang.tab.h, pokelang_lexer.exe, pokelang_compiler.exe -ErrorAction SilentlyContinue

# Build standalone lexer (tokenizer only)
Write-Host "[2/5] Building standalone lexer..." -ForegroundColor Yellow
flex pokelang.l
if ($LASTEXITCODE -eq 0) {
    gcc lex.yy.c -o pokelang_lexer.exe
    if ($LASTEXITCODE -eq 0) {
        Write-Host "  [OK] Standalone lexer built: pokelang_lexer.exe" -ForegroundColor Green
    } else {
        Write-Host "  [ERR] Failed to compile standalone lexer" -ForegroundColor Red
        exit 1
    }
} else {
    Write-Host "  [ERR] Failed to generate lexer" -ForegroundColor Red
    exit 1
}

# Build parser
Write-Host "[3/5] Generating parser from Bison..." -ForegroundColor Yellow
win_bison -d pokelang.y
if ($LASTEXITCODE -eq 0) {
    Write-Host "  [OK] Parser generated: pokelang.tab.c, pokelang.tab.h" -ForegroundColor Green
} else {
    Write-Host "  [ERR] Failed to generate parser" -ForegroundColor Red
    exit 1
}

# Build lexer for parser
Write-Host "[4/5] Generating lexer for parser integration..." -ForegroundColor Yellow
flex pokelang_parser.l
if ($LASTEXITCODE -eq 0) {
    Write-Host "  [OK] Integrated lexer generated" -ForegroundColor Green
} else {
    Write-Host "  [ERR] Failed to generate integrated lexer" -ForegroundColor Red
    exit 1
}

# Compile full compiler
Write-Host "[5/5] Compiling full compiler (lexer + parser)..." -ForegroundColor Yellow
gcc pokelang.tab.c lex.yy.c -o pokelang_compiler.exe -lm
if ($LASTEXITCODE -eq 0) {
    Write-Host "  [OK] Full compiler built: pokelang_compiler.exe" -ForegroundColor Green
} else {
    Write-Host "  [ERR] Failed to compile full compiler" -ForegroundColor Red
    exit 1
}

Write-Host "`n=== Build Complete ===" -ForegroundColor Green
Write-Host "`nYou now have:" -ForegroundColor Cyan
Write-Host "  1. pokelang_lexer.exe     - Standalone lexical analyzer (tokenizer)" -ForegroundColor White
Write-Host "  2. pokelang_compiler.exe  - Full compiler (lexer + parser)" -ForegroundColor White

Write-Host "`nUsage:" -ForegroundColor Cyan
Write-Host "  Tokenize only:  .\pokelang_lexer.exe input.poke tokens.txt" -ForegroundColor White
Write-Host "  Parse & verify: .\pokelang_compiler.exe input.poke" -ForegroundColor White

Write-Host "`nExample:" -ForegroundColor Cyan
Write-Host "  .\pokelang_compiler.exe simple_test.poke" -ForegroundColor White
