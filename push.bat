echo off
cls
git add .
set /p msg=Please enter your commit message:
git commit -m "%msg%"
set /p repo=Please enter the repository name:
git push %repo%
pause