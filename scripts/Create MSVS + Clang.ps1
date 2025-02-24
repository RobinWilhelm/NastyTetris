
$BUILD_PATH = "../build/VS2022/Clang/"

if (!(Test-Path -Path $BUILD_PATH))
{
	New-Item $BUILD_PATH -ItemType Directory
}

cmake -B $BUILD_PATH  -G "Visual Studio 17 2022" -T ClangCl 
Read-Host -Prompt "Press Enter to exit"