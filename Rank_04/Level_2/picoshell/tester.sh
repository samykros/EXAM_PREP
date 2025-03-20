#!/bin/bash

# 1️⃣ Compilar el código
gcc -Wall -Wextra -Werror -o picoshell picoshell.c
if [ $? -ne 0 ]; then
    echo "❌ Error al compilar 'picoshell.c'."
    exit 1
fi

echo "✅ Compilación exitosa."

# 2️⃣ Tests: Cada línea tiene -> "comando esperado"
declare -A tests=(
    ["echo hello"]="hello"
    ["echo test | cat | sed 's/t/T/g'"]="TesT"
    ["echo squalala | cat | sed 's/a/b/g'"]="squblblb"
    ["ls | grep picoshell"]=""
    ["echo hi | grep bye"]=""
    ["cat /etc/passwd | wc -l"]="$(cat /etc/passwd | wc -l)"
    ["/bin/false"]=""
    ["/bin/false | echo ok"]="ok"
)

# 3️⃣ Ejecutar cada test
passed=0
failed=0

for cmd in "${!tests[@]}"
do
    expected="${tests[$cmd]}"
    result=$(eval ./picoshell $cmd 2>&1)

    if [ "$result" == "$expected" ]; then
        echo "✅ Test PASADO: '$cmd'"
        ((passed++))
    else
        echo "❌ Test FALLADO: '$cmd'"
        echo "   ➤ Esperado: '$expected'"
        echo "   ➤ Obtenido: '$result'"
        ((failed++))
    fi
done

# 4️⃣ Resumen final
echo "----------------------------"
echo "🟩 Tests pasados: $passed"
echo "🟥 Tests fallados: $failed"
echo "----------------------------"

# Estado final de éxito o fallo
if [ "$failed" -eq 0 ]; then
    echo "🎯 TODOS LOS TESTS PASADOS 🎯"
    exit 0
else
    echo "❗️ Hay errores, revisa los fallos."
    exit 1
fi
