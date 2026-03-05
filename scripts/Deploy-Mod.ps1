<#
.SYNOPSIS
    Deploy Virtua Cop 2 Accessibility Mod to the game folder.

.DESCRIPTION
    Copies the built WINMM.dll proxy to the game directory.
    Backs up the original WINMM.dll if present.
    Reminds you to also copy Tolk.dll and nvdaControllerClient32.dll.

.PARAMETER GameDir
    Path to the VirtuaCop2 game folder (containing VC2.EXE and PPJ2DD.EXE).
    Example: "C:\Games\VirtuaCop2\VirtuaCop2"

.PARAMETER BuildConfig
    "Release" (default) or "Debug"

.EXAMPLE
    .\Deploy-Mod.ps1 -GameDir "C:\Games\VirtuaCop2\VirtuaCop2"
    .\Deploy-Mod.ps1 -GameDir "C:\Games\VirtuaCop2\VirtuaCop2" -BuildConfig Debug
#>
param(
    [Parameter(Mandatory=$true)]
    [string]$GameDir,

    [string]$BuildConfig = "Release"
)

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$ProjectDir = Split-Path -Parent $ScriptDir
$BuildDir = Join-Path $ProjectDir "Win32\$BuildConfig"
$ProxyDll = Join-Path $BuildDir "WINMM.dll"
$TargetDll = Join-Path $GameDir "WINMM.dll"
$BackupDll = Join-Path $GameDir "WINMM_orig.dll"

Write-Host ""
Write-Host "=== VC2 Accessibility Mod Deploy ===" -ForegroundColor Cyan
Write-Host "Game dir:    $GameDir"
Write-Host "Build:       $BuildConfig ($BuildDir)"
Write-Host ""

# Check build output exists
if (-not (Test-Path $ProxyDll)) {
    Write-Host "ERROR: Build output not found." -ForegroundColor Red
    Write-Host "Expected: $ProxyDll"
    Write-Host "Make sure you built the project in Visual Studio first (Build > Build Solution)."
    exit 1
}

# Check game directory exists
if (-not (Test-Path $GameDir)) {
    Write-Host "ERROR: Game directory not found: $GameDir" -ForegroundColor Red
    exit 1
}

# Check for PPJ2DD.EXE to confirm this is the right folder
if (-not (Test-Path (Join-Path $GameDir "PPJ2DD.EXE"))) {
    Write-Host "WARNING: PPJ2DD.EXE not found in game directory." -ForegroundColor Yellow
    Write-Host "Make sure you're pointing to the VirtuaCop2 subfolder (not the outer folder)."
    $confirm = Read-Host "Continue anyway? (y/n)"
    if ($confirm -ne "y") { exit 1 }
}

# Backup original WINMM.dll if present and not already backed up
if ((Test-Path $TargetDll) -and -not (Test-Path $BackupDll)) {
    Write-Host "Backing up original WINMM.dll -> WINMM_orig.dll..." -ForegroundColor Yellow
    Copy-Item $TargetDll $BackupDll
    Write-Host "Backup created." -ForegroundColor Green
} elseif (Test-Path $BackupDll) {
    Write-Host "Backup WINMM_orig.dll already exists. Skipping backup." -ForegroundColor Gray
}

# Deploy proxy DLL
Write-Host "Deploying mod WINMM.dll..."
Copy-Item $ProxyDll $TargetDll -Force
Write-Host "WINMM.dll deployed." -ForegroundColor Green

# Check for Tolk DLLs
$tolkDll = Join-Path $GameDir "Tolk.dll"
$nvdaDll = Join-Path $GameDir "nvdaControllerClient32.dll"

Write-Host ""
Write-Host "=== Checking for Tolk DLLs ===" -ForegroundColor Cyan

if (-not (Test-Path $tolkDll)) {
    Write-Host "MISSING: Tolk.dll not found in game folder." -ForegroundColor Red
    Write-Host "Download from: https://github.com/ndarilek/tolk/releases"
    Write-Host "Copy Tolk.dll (32-bit) to: $GameDir"
} else {
    Write-Host "OK: Tolk.dll found." -ForegroundColor Green
}

if (-not (Test-Path $nvdaDll)) {
    Write-Host "MISSING: nvdaControllerClient32.dll not found in game folder." -ForegroundColor Red
    Write-Host "This is required for NVDA support."
    Write-Host "It comes bundled with Tolk. Copy it to: $GameDir"
} else {
    Write-Host "OK: nvdaControllerClient32.dll found." -ForegroundColor Green
}

Write-Host ""
Write-Host "=== Deploy Complete ===" -ForegroundColor Cyan
Write-Host "Launch VC2.EXE to start the game."
Write-Host "Your screen reader should announce 'Virtua Cop 2 Accessibility Mod loaded' on startup."
Write-Host ""
Write-Host "Hotkeys during gameplay:"
Write-Host "  F1  - Full status (mode, stage, difficulty, lives, score)"
Write-Host "  F2  - Repeat last announcement"
Write-Host "  F3  - Score"
Write-Host "  F4  - Stage and difficulty"
Write-Host "  F5  - Lives"
Write-Host "  F12 - Toggle debug log (VC2_AccessMod_Debug.log)"
Write-Host ""
Write-Host "NOTE: Memory addresses for lives/score are placeholder zeros until"
Write-Host "you complete Cheat Engine analysis. See docs/game-api.md."" 
