$srcDir = "$PSScriptRoot\..\src"
$buildDir = "$PSScriptRoot\..\build"
$distDir = "$buildDir\dist"

if ($args -contains "-c") {
	rm -r $buildDir # Remove-Item -Recurse -Force $buildDir
}

cmake -G Ninja -S $srcDir -B $buildDir
ninja -C $buildDir

mkdir $distDir -ErrorAction SilentlyContinue
mv $buildDir\*.mll $distDir