rm -Rf build/android/
cp -R android/ build/
cp -R res/. build/android/app/src/main/assets
cd build/android/
gradle assembleDebug
