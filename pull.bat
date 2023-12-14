echo off
cls
set /p repo=Please enter the name of the repo to pull from:
git pull %repo%
pause