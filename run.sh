#!/bin/bash

echo "==> Compilando..."
make

if [ $? -ne 0 ]; then
    echo "Error de compilacion."
    exit 1
fi

echo ""
echo "==> Ejecutando..."
./bin/log_analyzer