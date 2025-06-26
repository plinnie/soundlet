Push-Location
& "${env:ProgramFiles}\Microsoft Visual Studio\2022\Community\Common7\Tools\Launch-VsDevShell.ps1" -Arch amd64
Pop-Location
cmake -B Builds -G "Visual Studio 17 2022"
msbuild Builds\Soundlet.sln /p:Configuration="Release" /p:Platform="x64"
