$WORKDIR = Get-Location

if (Test-Path -Path "build" -PathType Container) {
    Write-Host "`e[33mYou seem to have an existing build directory..."
    Write-Host "We recommend removing the build directory and doing a clean build if this script fails`e[0m"
}

if (-Not (Test-Path -Path "$HOME\vcpkg" -PathType Container)) {
    Write-Host "Installing VCPKG..."
    Set-Location -Path $HOME
    git clone https://github.com/microsoft/vcpkg.git vcpkg
    Set-Location -Path vcpkg
    ./bootstrap-vcpkg.bat
}
$env:VCPKG_ROOT = "$HOME\vcpkg"
$env:PATH = "$env:VCPKG_ROOT;$env:PATH"

cd $WORKDIR
cmake --preset=default
