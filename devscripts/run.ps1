$BUILD_DIR = "build"
if (-Not (Test-Path -Path $BUILD_DIR -PathType Container)) {
    New-Item -ItemType Directory -Path $BUILD_DIR | Out-Null
}
Set-Location -Path $BUILD_DIR

cmake ..
if ($?) {
    ninja
    if ($?) {
        ./bragii
    }
}
cd ..
