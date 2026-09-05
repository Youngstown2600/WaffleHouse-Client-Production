param(
    [switch]$Uninstall
)

$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent $MyInvocation.MyCommand.Path

if (-not $Uninstall -and $PSBoundParameters.Count -eq 0 -and [Environment]::UserInteractive) {
    Write-Host ""
    Write-Host "What do you want to do?"
    Write-Host "  1) Build / refresh the portable Windows package"
    Write-Host "  2) Uninstall / remove the package produced by this builder"
    $choice = Read-Host "Selection [1]"
    if ($choice -eq "2") { $Uninstall = $true }
    elseif ($choice -ne "" -and $choice -ne "1") { throw "Invalid selection." }
}

if ($Uninstall) {
    $PackageDir = Join-Path $Root "dist\windows"
    if (Test-Path $PackageDir) {
        Remove-Item -Recurse -Force $PackageDir
        Write-Host "Removed portable WaffleHouse-Client package: $PackageDir"
    } else {
        Write-Host "No portable WaffleHouse-Client package was found at: $PackageDir"
    }
    Write-Host "Per-user WaffleHouse configuration was preserved."
    exit 0
}
$Candidates = @(
    "C:\msys64\msys2_shell.cmd",
    "C:\tools\msys64\msys2_shell.cmd"
)
$MsysShell = $Candidates | Where-Object { Test-Path $_ } | Select-Object -First 1
if (-not $MsysShell) {
    throw "MSYS2 was not found. Install MSYS2 (UCRT64 recommended), then rerun this builder."
}
Write-Host "WaffleHouse-Client 5.1r4 - Windows 10/11 GUI + CLI"
Write-Host "Using MSYS2 UCRT64: $MsysShell"
$Escaped = $Root.Replace("'", "'\"'\"'")
$Command = "ROOT=`$(cygpath -u '$Escaped'); cd `"`$ROOT`"; exec ./scripts/build-windows-msys2.sh"
& $MsysShell -defterm -no-start -ucrt64 -shell bash -c $Command
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
