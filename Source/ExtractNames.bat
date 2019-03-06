@echo off
echo Extracting file names...
cd ..
cd ..

dir "%CD%\Prints" > Names.cvs /b /o
move Names.cvs "%CD%\Source"

echo Complete!

