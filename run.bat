@echo off
REM Compila y ejecuta el analizador de logs en Windows

if not exist bin_win mkdir bin_win

echo ==> Compilando...
gcc main_windows.c log_processor.c -o bin_win\log_analyzer.exe

if %errorlevel% neq 0 (
    echo Error de compilacion.
    exit /b 1
)

echo.
echo ==> Ejecutando...
bin_win\log_analyzer.exe