param(
    [Parameter(Mandatory = $true)]
    [string]$InputFile,

    [string]$OutputC = "generated_program.c",

    [string]$OutputExe = "generated_program.exe",

    [switch]$Run
)

if (-not (Test-Path $InputFile)) {
    Write-Error "Input file not found: $InputFile"
    exit 1
}

$content = Get-Content $InputFile -Raw

# Remove multi-line comments: $* ... *$
$content = [regex]::Replace($content, '\$\*.*?\*\$', '', [System.Text.RegularExpressions.RegexOptions]::Singleline)

# Remove single-line comments: $$ ... endline
$content = [regex]::Replace($content, '\$\$.*$', '', [System.Text.RegularExpressions.RegexOptions]::Multiline)

# Handle switch-case syntax before generic delimiter replacement.
$content = [regex]::Replace($content, '\bTypeBadge\s+([^\r\n]+?)\s+dot\b', 'case $1:')
$content = [regex]::Replace($content, '\bDefaultBadge\s+dot\b', 'default:')

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

# Convert declaration of main from void to int for standard C behavior.
$content = [regex]::Replace($content, '\bvoid\s+main\s*\(', 'int main(')

$runtimeHeader = @'
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

'@

$finalCode = $runtimeHeader + $content
Set-Content -Path $OutputC -Value $finalCode -Encoding UTF8

Write-Host "Generated C file: $OutputC"

gcc $OutputC -o $OutputExe -lm
if ($LASTEXITCODE -ne 0) {
    Write-Error "Failed to compile generated C code."
    exit 1
}

Write-Host "Built executable: $OutputExe"

if ($Run) {
    Write-Host "Running: $OutputExe"
    Write-Host "----- Program Output Start -----"
    & ".\$OutputExe"
    $programExitCode = $LASTEXITCODE
    Write-Host ""
    Write-Host "----- Program Output End (exit code: $programExitCode) -----"
}
