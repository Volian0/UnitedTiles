rmdir build\android /s /q
xcopy /E /I /Y android build\android
xcopy /E /I /Y res build\android\app\src\main\assets
cd build\android
call gradle assembleDebug
cd ../..