param([switch]$DryRun)
# Undo reorganize_effects.ps1
# Usage:
#   powershell -ExecutionPolicy Bypass -File .\undo_reorganize_effects.ps1 -DryRun   (preview only, nothing is moved)
#   powershell -ExecutionPolicy Bypass -File .\undo_reorganize_effects.ps1            (really move)
$ErrorActionPreference = 'Stop'
$root = $PSScriptRoot
# each entry = whole folder: @(from, to)
$moves = @(
    @('Boss\Mage\Beam', 'Beam'),
    @('Boss\Mage\Tornado', 'Tornado'),
    @('Boss\Mage\Salamander', 'Salamander'),
    @('Boss\Mage\Sword', 'Sword'),
    @('Boss\Golem', 'Golem'),
    @('Enemy\EnemySpawner', 'EnemySpawner'),
    @('Enemy\Energy', 'Energy'),
    @('Enemy\Explosion', 'Explosion'),
    @('Common\Hit', 'Hit'),
    @('Common\Smoke', 'Smoke')
)

# 1. check everything first. If anything is wrong, stop before moving anything.
$errors = 0
foreach ($m in $moves) {
    $src = Join-Path $root $m[0]
    $dst = Join-Path $root $m[1]
    if (-not (Test-Path -LiteralPath $src -PathType Container)) {
        Write-Host ("MISSING FOLDER : " + $m[0]); $errors++
    }
    if (Test-Path -LiteralPath $dst) {
        Write-Host ("ALREADY EXISTS : " + $m[1]); $errors++
    }
}
if ($errors -gt 0) {
    Write-Host "Stopped. $errors problem(s) found. Nothing was moved."
    exit 1
}

# 2. move whole folders (contents, file names and folder names stay the same)
foreach ($m in $moves) {
    $src = Join-Path $root $m[0]
    $dst = Join-Path $root $m[1]
    if ($DryRun) {
        $n = (Get-ChildItem -LiteralPath $src -Recurse -File | Measure-Object).Count
        Write-Host ("[DryRun] " + $m[0] + "  ->  " + $m[1] + "   (" + $n + " files)")
        continue
    }
    $parent = Split-Path $dst -Parent
    if (-not (Test-Path -LiteralPath $parent)) {
        New-Item -ItemType Directory -Path $parent | Out-Null
    }
    Move-Item -LiteralPath $src -Destination $dst
    Write-Host ("moved : " + $m[0] + "  ->  " + $m[1])
}

# 3. remove parent folders that became empty
if (-not $DryRun) {
    $dirs = @()
    foreach ($m in $moves) {
        $d = Split-Path $m[0] -Parent
        while ($d) {
            $dirs += $d
            $d = Split-Path $d -Parent
        }
    }
    $dirs = $dirs | Sort-Object -Unique | Sort-Object { $_.Length } -Descending
    foreach ($d in $dirs) {
        $p = Join-Path $root $d
        if ((Test-Path -LiteralPath $p) -and -not (Get-ChildItem -LiteralPath $p -Force)) {
            Remove-Item -LiteralPath $p
            Write-Host ("removed empty folder : " + $d)
        }
    }
    Write-Host "Done."
}
