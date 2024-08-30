$srcDir = "$PSScriptRoot\..\src"
$buildDir = "$PSScriptRoot\..\build"
$distDir = "C:\Users\gavin\devkitBase\plug-ins\plug-ins" # "$buildDir\dist"

if ($args -contains "-c") {
	rm -r $buildDir # Remove-Item -Recurse -Force $buildDir
	rm -r $distDir # Remove-Item -Recurse -Force $distDir
}

cmake -G Ninja -S $srcDir -B $buildDir
ninja -C $buildDir

mkdir $distDir -ErrorAction SilentlyContinue
mv $buildDir\*.mll $distDir