@echo off
set GP_JDBC_JARFILE=postgresql-8.1-407.jdbc3.jar

echo Setting up PATH for Greenplum drivers
set GPHOME_CONNECTIVITY=%~dp0
set PATH=%PATH%;%GPHOME_CONNECTIVITY%bin;%GPHOME_CONNECTIVITY%lib
set CLASSPATH=%GPHOME_CONNECTIVITY%drivers\jdbc\%GP_JDBC_JARFILE%;%CLASSPATH%

echo.
echo CONNECTIVITY environment variables configured successfully.
echo.
