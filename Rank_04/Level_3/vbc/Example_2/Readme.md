Árbol de Sintaxis Abstracta (AST). 🌳


📌¿Qué hace parse_factor()?

Si ve (, llama a parse_expression() para leer lo que hay dentro.
Si es un número (0-9), lo convierte a un nodo hoja del AST.
💡Importante: Avanza el puntero expr ((*expr)++) para moverse en la cadena.

📌¿Qué hace parse_term()?

	Llama a parse_factor() primero, porque los números/paréntesis van antes de *.
	Mientras haya *, sigue multiplicando (2*3*4 se convierte en ((2*3)*4)).

📌¿Qué hace parse_expression()?

	Llama a parse_term() primero, porque las multiplicaciones tienen más prioridad.
	Mientras haya +, sigue sumando (2+3+4 se convierte en ((2+3)+4)).


📌Función				📌¿Qué hace?
parse_factor()			Maneja números y paréntesis ((2+3))
parse_term()			Maneja * (multiplicación tiene más prioridad que +)
parse_expression()		Maneja + (suma, menor prioridad que *)
¿Por qué funciona?		Cada función descompone la expresión en partes más pequeñas recursivamente.


💡 Ejemplo con "2+3*4"
////////////////////////////////////////////////////////////////////
//  parse_expression("2+3*4")                                     //
//  ↓                                                             //
//  parse_term("2") -> Nodo 2                                     //
//  ↓                                                             //
//  Vemos '+', creamos nodo '+' con hijos (2, parse_term("3*4"))  //
//  ↓                                                             //
//  parse_term("3*4") devuelve (3*4)                              //
//  ↓                                                             //
//  AST generado: (2 + (3 * 4))                                   //
////////////////////////////////////////////////////////////////////


💡 Ejemplo super ejemplificado con "2*(3+4)"
////////////////////////////////////////////////////////////////////////
//  parse_expression("2*(3+4)")                                       //
//  ↓                                                                 //
//  parse_term("2*(3+4)")                                             //
//  ↓                                                                 //
//  parse_factor("2") -> Nodo 2                                       //
//  ↓                                                                 //
//  Vemos '*', creamos nodo '*' con hijos (2, parse_factor("(3+4)"))  //
//  ↓                                                                 //
//  parse_factor("(3+4)") -> Llama a parse_expression("3+4")          //
//  ↓                                                                 //
//  parse_expression("3+4") -> Crea nodo '+' con hijos (3, 4)         //
//  ↓                                                                 //
//  Retorna nodo '*' con hijos (2, (3+4))                             //
////////////////////////////////////////////////////////////////////////

      *
    /   \
   2     +
        /   \
       3     4

🎭 Paso 1: parse_expression()

t_node *parse_expression(char **expr)
{
    t_node *node = parse_term(expr);  // 🔹 Primero llamamos a parse_term()
    
    while (**expr == '+') // 🔹 Buscamos sumas, pero la primera vez no hay '+'
    {
        char op = *(*expr)++;
        node = create_operator_node(op, node, parse_term(expr));
    }

    return node;
}

📌 ¿Qué pasa aquí?

    Llama a parse_term(expr) para manejar * antes que +.
    No entra en el while de + porque el primer carácter es "2".

🎭 Paso 2: parse_term()

t_node *parse_term(char **expr)
{
    t_node *node = parse_factor(expr); // 🔹 Primero parseamos un "factor"

    while (**expr == '*')  // 🔹 Buscamos multiplicaciones, sí hay '*'
    {
        char op = *(*expr)++;  // 🔹 Guardamos '*'
        node = create_operator_node(op, node, parse_factor(expr)); // 🔹 Procesamos el lado derecho de '*'
    }

    return node;
}

📌 ¿Qué pasa aquí?

    Llama a parse_factor(expr).
    Encuentra "2", así que devuelve un nodo con el número 2.
    Luego encuentra "*", así que crea un nodo * y necesita el segundo operando (3+4).
    Llama a parse_factor(expr) otra vez para el lado derecho del *.

🎭 Paso 3: parse_factor()

t_node *parse_factor(char **expr)
{
    if (**expr == '(') // 🔹 Hay un paréntesis
    {
        (*expr)++;  // 🔹 Saltamos '('
        t_node *node = parse_expression(expr); // 🔹 Parseamos la expresión dentro de los paréntesis
        (*expr)++;  // 🔹 Saltamos ')'
        return node;
    }
    return create_number_node(*(*expr)++ - '0'); // 🔹 Si no hay '(', es un número
}

📌 ¿Qué pasa aquí?

    Encuentra "(", así que llama a parse_expression() para procesar "3+4".

🎭 Paso 4: parse_expression() dentro del paréntesis "3+4"

t_node *parse_expression(char **expr)
{
    t_node *node = parse_term(expr);  // 🔹 Primero manejamos multiplicaciones (pero no hay)

    while (**expr == '+')  // 🔹 ¡Sí hay '+'!
    {
        char op = *(*expr)++;
        node = create_operator_node(op, node, parse_term(expr));
    }

    return node;
}

📌 ¿Qué pasa aquí?

    Llama a parse_term(expr), que llama a parse_factor(expr).
    parse_factor() ve "3", así que devuelve un nodo con 3.
    Regresamos a parse_expression() y encontramos "+".
    Guardamos + y llamamos a parse_term(expr) para 4.
    parse_factor(expr) devuelve un nodo con 4.
    Se construye el nodo + con 3 y 4.
    Se retorna este nodo + al parse_factor() original (parse_factor("(3+4)")).

🎭 Paso 5: Volvemos a parse_term()

📌 ¿Qué tenemos hasta ahora?

      *
    /   \
   2     +
        /   \
       3     4

    parse_factor(expr) nos ha devuelto el nodo + con 3 y 4.
    parse_term(expr) ahora tiene un nodo * con 2 y + (3,4).
    No hay más *, así que parse_term() termina.

🎭 Paso 6: Volvemos a parse_expression()

📌 ¿Qué tenemos ahora?

      *
    /   \
   2     +
        /   \
       3     4

    parse_term(expr) nos ha devuelto el nodo *.
    No hay + después de "* (3+4)", así que parse_expression() termina.
    Retornamos el nodo raíz *.

🎯 while (**expr == '+')

📌 Que pasa si tenemos + en la posición inicial? Subject dice lo siguiente:
Numbers (single-digit only, e.g., 2, 3).
📌 Cuando sí encontramos +, ya estamos dentro del parsing de los paréntesis ((3+4)), así que el while se activa en la recursión correcta.

💡 Visualizando los while

    Primer parse_expression() no ve +, así que llama a parse_term().
    Dentro del paréntesis "3+4", parse_expression() ve + y lo procesa.
    Una vez procesado (3+4), volvemos a la raíz con el nodo *.

🏆 Resumen Final

📌 parse_expression() solo maneja + cuando lo encuentra en la posición correcta.
📌 Si no hay +, simplemente deja que parse_term() haga su trabajo.
📌 parse_term() se encarga de *, respetando la precedencia.
📌 parse_factor() maneja los paréntesis y los números.

✅ Cada llamada a función se apila y resuelve en orden correcto.
✅ El árbol se genera paso a paso sin perder la estructura de la expresión.





La funcion evaluate, pero primero, como se ve nuestra lista ahora?


📌 Cuando parse_expression() y parse_term() van procesando la cadena, los nodos se crean en este orden:

1️⃣ 2 (nodo número) → create_number_node(2)
2️⃣ 3 (nodo número) → create_number_node(3)
3️⃣ 4 (nodo número) → create_number_node(4)
4️⃣ + (nodo operador) → create_operator_node('+', nodo_3, nodo_4)
5️⃣ * (nodo operador) → create_operator_node('*', nodo_2, nodo_+)

📌 Lista de nodos en orden de creación (como si fuera un array en memoria):

[ 2, 3, 4, +, * ]

Cómo evaluate() usa la lista (Orden de Evaluación)

📌 Ahora vemos cómo evaluate() recorre el árbol en orden de ejecución.
📌 Este orden es diferente al orden de creación de nodos.

1️⃣ Empieza en la raíz (*) → No es un número, así que evalúa los hijos.
2️⃣ Llama a evaluate(left), que es evaluate(2).

    2 es un número, se devuelve 2 inmediatamente.
    3️⃣ Llama a evaluate(right), que es evaluate(+).
    4️⃣ Dentro de evaluate(+), evalúa left (3).
    3 es un número, se devuelve 3 inmediatamente.
    5️⃣ Dentro de evaluate(+), evalúa right (4).
    4 es un número, se devuelve 4 inmediatamente.
    6️⃣ Se calcula 3 + 4 = 7.
    7️⃣ Vuelve al nodo raíz *, ahora tiene 2 * 7 = 14.

🔥 1️⃣ ¿Por qué * es la raíz?

📌 Cuando construimos el AST (parse_expression() y parse_term()), respetamos la jerarquía de operaciones:

1️⃣ Encontramos 2 → Creamos el nodo 2.
2️⃣ Encontramos * → 2 se convierte en left, y seguimos parseando right.
3️⃣ Encontramos (, entramos en parse_expression() para 3+4.
4️⃣ Creamos + → 3 es left, 4 es right.
5️⃣ Volvemos a *, y + se convierte en right del *.

🔥🔥Como funciona evauate:

evaluate(*)
├── evaluate(2) → 2
└── evaluate(+)
    ├── evaluate(3) → 3
    └── evaluate(4) → 4
    (3 + 4) → 7
(* evalúa: 2 * 7) → 14




Si te lias con punteros

t_node *parse_expression(char **expr);
// Tiene ese doble ** porque va a lo siguiente:
t_node *ast = parse_expression(&expr);
// *ast es un puntero y &expr 

1️⃣ t_node *ast → ast es un puntero a un nodo, es decir, el resultado del AST que vamos a construir.
2️⃣ parse_expression(&expr) → Llamamos a parse_expression() y le pasamos &expr.
3️⃣ &expr → Obtenemos la dirección de expr, lo que permite que parse_expression() modifique expr directamente.