$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent $MyInvocation.MyCommand.Path
$Candidates = @(
    "C:\msys64\msys2_shell.cmd",
    "C:\tools\msys64\msys2_shell.cmd"
)
$MsysShell = $Candidates | Where-Object { Test-Path $_ } | Select-Object -First 1
if (-not $MsysShell) {
    throw "MSYS2 was not found. Install MSYS2 (UCRT64 recommended), then rerun this builder."
}
Write-Host "WaffleHouse-Client 5.1 - Windows 10/11 GUI + CLI"
Write-Host "Using MSYS2 UCRT64: $MsysShell"
$Escaped = $Root.Replace("'", "'\"'\"'")
$Command = "ROOT=`$(cygpath -u '$Escaped'); cd `"`$ROOT`"; exec ./scripts/build-windows-msys2.sh"
& $MsysShell -defterm -no-start -ucrt64 -shell bash -c $Command
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
