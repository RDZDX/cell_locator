"C:\Program Files\MRE_SDK\tools\DllPackage.exe" "D:\MyGitHub\cell_locator\cell_locator.vcproj"
if %errorlevel% == 0 (
 echo postbuild OK.
  copy cell_locator.vpp ..\..\..\MoDIS_VC9\WIN32FS\DRIVE_E\cell_locator.vpp /y
exit 0
)else (
echo postbuild error
  echo error code: %errorlevel%
  exit 1
)

