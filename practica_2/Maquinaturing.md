# Práctica 2 — Simulador de una Máquina de Turing

**Asignatura:** Complejidad Computacional · **Curso:** 4.º · **Año académico:** 2026/27
**Autor:** Álvaro Pérez Ramos — `alu0101574042@ull.edu.es`
**Escuela Superior de Ingeniería y Tecnología · Universidad de La Laguna**

---

## 1. Tipo de Máquina de Turing implementada

> **MT multicinta determinista**, donde la ejecución termina únicamente por ausencia de transiciones
> (no hay un límite de pasos: si la MT no termina, el simulador tampoco).

De las tres variaciones que el enunciado obliga a elegir:

| Variación                                   | Elección                          |
| -------------------------------------------- | ---------------------------------- |
| Escritura y movimiento                       | **Simultáneos** (una transición hace las dos cosas a la vez) |
| Movimientos permitidos                       | **L, R y S** (incluye "sin mover") |
| Dirección de la cinta                        | **Infinita en ambas direcciones** (MT tradicional) |

Una cadena se acepta si, al detenerse (ninguna transición aplicable), el estado alcanzado
pertenece a `F`. El contenido final de la cinta 1 se muestra siempre, se acepte o no.

---

## 2. Compilación y ejecución

### Requisitos

| Herramienta | Versión mínima                      |
| ----------- | ------------------------------------ |
| CMake       | 3.10                                 |
| Compilador  | C++17 (g++ 9 / clang 10 o superior)  |

### Compilación

```bash
cmake -S . -B build
cmake --build build
```

El ejecutable se genera en `build/bin/maquina_turing`.

### Ejecución

```
./build/bin/maquina_turing -config <f> [-in <f>]
```

| Opción          | Obligatoria | Descripción                                                             |
| --------------- | ----------- | ------------------------------------------------------------------------ |
| `-config <f>`   | Sí          | Fichero de texto con la configuración de la MT.                          |
| `-in <f>`       | No          | Fichero con las cadenas a comprobar. Si se omite, se leen por teclado.   |
| `-h`, `--help`  | No          | Muestra la ayuda y termina (se resuelve antes que cualquier otra comprobación). |

A diferencia de P01, no hay `-trace` ni `-out`: el enunciado de esta práctica no los pide. La
salida, para cada cadena, es siempre la misma: `ACEPTADA`/`RECHAZADA` y el contenido de la cinta 1
con el cabezal entre corchetes (p. ej. `ab[c]d`).

```bash
./build/bin/maquina_turing -config test/Ejemplo1_MT.txt
./build/bin/maquina_turing -config test/Ejemplo2_MT.txt -in cadenas.txt
```

---

## 3. Formato del fichero de configuración

```
# Las líneas en blanco y el texto que sigue a '#' se ignoran
q0 q1 q2        # conjunto Q
0 1             # conjunto Sigma
0 1 .           # conjunto Gamma (un carácter por símbolo)
q0              # estado inicial
.               # símbolo blanco (debe pertenecer a Gamma, y NO a Sigma)
q2              # conjunto F
1               # número de cintas (N)
q0 0 q1 0 R     # transición con N=1: origen lee destino escribe movimiento
```

Con `N` cintas, cada transición agrupa los campos **por tipo**, no por cinta:

```
origen  lee_1 .. lee_N  destino  escribe_1 .. escribe_N  mov_1 .. mov_N
```

Ejemplo con `N=2` (la cinta 2 recibe una copia de lo que hay en la cinta 1):

```
q0 a . q0 a a R R
```

Convenios:

- El símbolo blanco se declara en su propia línea, **después** de `Σ` y `Γ`: debe pertenecer a `Γ`
  (la cinta necesita poder representarlo) y **no** puede pertenecer a `Σ` (no es un símbolo de
  entrada válido).
- No existe convenio de ε: no hay transiciones vacías, cada transición consulta siempre un símbolo
  concreto en cada cinta.
- Los movimientos válidos son `L`, `R` y `S`.
- Inicialmente, la cinta 1 contiene la cadena de entrada con el cabezal en su primer símbolo; el
  resto de las cintas (si `N>1`) están completamente en blanco, con el cabezal en la posición 0.

---

## 4. Diseño orientado a objetos

### 4.1 Diagrama de clases

```mermaid
classDiagram
    direction LR

    class Symbol {
        -char character_
        +GetCharacter() char
        +ToString() string
    }

    class State {
        -string name_
        +GetName() string
    }

    class Alphabet {
        -set~Symbol~ symbols_
        +InsertSymbol(Symbol) bool
        +Contains(Symbol) bool
        +Size() size_t
        +IsEmpty() bool
    }

    class Chain {
        -string text_
        +Chain(string, Alphabet)
        +GetText() string
        +Length() size_t
    }

    class Movement {
        <<enumeration>>
        kLeft
        kRight
        kStay
    }

    class TapeAction {
        <<struct>>
        +Symbol read_symbol
        +Symbol write_symbol
        +Movement movement
    }

    class Tape {
        -Symbol blank_symbol_
        -map~long long, Symbol~ cells_
        -long long head_position_
        +Read() Symbol
        +Write(Symbol) void
        +Move(Movement) void
        +ToString() string
    }

    class TransitionKey {
        <<struct>>
        +State origin_state
        +vector~Symbol~ read_symbols
    }

    class Transition {
        -State origin_state_
        -State destination_state_
        -vector~TapeAction~ tape_actions_
        +GetKey() TransitionKey
        +ToString() string
    }

    class TransitionFunction {
        -map~TransitionKey, Transition~ transitions_by_key_
        +Insert(Transition) bool
        +GetTransition(State, vector~Symbol~) optional~Transition~
        +GetTransitionsFrom(State) vector~Transition~
        +Size() size_t
    }

    class TuringMachine {
        -set~State~ states_
        -vector~State~ state_declaration_order_
        -Alphabet input_alphabet_
        -Alphabet tape_alphabet_
        -State initial_state_
        -Symbol blank_symbol_
        -set~State~ final_states_
        -int tape_count_
        -TransitionFunction transition_function_
        +IsFinalState(State) bool
        +ComputeReachableStates() set~State~
    }

    class TuringConfiguration {
        -State current_state_
        -vector~Tape~ tapes_
        +ReadSymbols() vector~Symbol~
        +ApplyTransition(Transition) void
    }

    class TuringRunResult {
        <<struct>>
        +bool is_accepted
        +string tape_contents
    }

    class Simulator {
        -TuringMachine machine_
        +Run(Chain) TuringRunResult
    }

    class TuringMachineParser {
        -string configuration_file_path_
        -vector~SourceLine~ significant_lines_
        +Parse(ostream) TuringMachine
    }

    class TuringMachineValidator {
        +Validate(TuringMachine, ostream)$ void
    }

    class CommandLineOptions {
        -bool is_help_requested_
        -string configuration_file_path_
        -string input_file_path_
        +Parse(int, char**)$ CommandLineOptions
        +BuildHelpText(string)$ string
    }

    Alphabet o-- Symbol
    Chain ..> Alphabet
    Tape o-- Symbol
    Transition *-- State
    Transition *-- TapeAction
    TapeAction *-- Symbol
    TapeAction *-- Movement
    Transition ..> TransitionKey : genera
    TransitionFunction o-- Transition
    TuringMachine *-- TransitionFunction
    TuringMachine *-- Alphabet
    TuringConfiguration *-- State
    TuringConfiguration *-- Tape
    Simulator --> TuringMachine : simula
    Simulator ..> TuringConfiguration : ejecuta
    Simulator ..> Chain : procesa
    Simulator ..> TuringRunResult : devuelve
    TuringMachineParser ..> TuringMachine : construye
    TuringMachineValidator ..> TuringMachine : comprueba
```

### 4.2 Jerarquía de excepciones

```mermaid
classDiagram
    direction TB

    class exception { <<std>> }
    class Error {
        -string message_
        +what() char*
    }
    class ConfigurationError {
        -int line_number_
        +GetLineNumber() int
        +SetLineNumber(int) void
    }

    exception <|-- Error
    Error <|-- CommandLineError
    Error <|-- FileError
    Error <|-- ChainError
    Error <|-- ConfigurationError

    ConfigurationError <|-- MissingSectionError
    ConfigurationError <|-- InvalidStateError
    ConfigurationError <|-- InvalidSymbolError
    ConfigurationError <|-- InvalidTransitionError
    ConfigurationError <|-- DuplicatedElementError
    ConfigurationError <|-- NonDeterministicTransitionError
```

Sin `SimulationError`: al no haber backtracking ni límite de pasos, no hay nada parecido al
desbordamiento de pila o al límite de exploración de P01 que proteger.

### 4.3 Flujo de ejecución

```mermaid
sequenceDiagram
    participant Usuario
    participant main
    participant CommandLineOptions
    participant TuringMachineParser
    participant TuringMachineValidator
    participant Simulator
    participant TuringConfiguration

    Usuario->>main: -config f -in g
    main->>CommandLineOptions: Parse(argc, argv)
    CommandLineOptions-->>main: opciones validadas
    main->>TuringMachineParser: Parse(ostream)
    TuringMachineParser-->>main: TuringMachine
    main->>TuringMachineValidator: Validate(machine, ostream)
    TuringMachineValidator-->>main: avisos por el ostream
    main->>Simulator: Simulator(machine)
    loop por cada cadena de g (o del teclado)
        main->>Simulator: Run(chain)
        Simulator->>TuringConfiguration: ReadSymbols() / ApplyTransition() hasta que no haya transición
        Simulator-->>main: TuringRunResult (veredicto + cinta 1)
    end
    main-->>Usuario: ACEPTADA/RECHAZADA y el contenido de la cinta 1, por cada cadena
```

### 4.4 Responsabilidad de cada clase

| Clase                   | Responsabilidad                                                                       |
| ------------------------ | ---------------------------------------------------------------------------------------- |
| `Symbol`                | Símbolo de un alfabeto. Sin convenio de ε: no existe en una MT.                          |
| `State`                 | Estado, identificado por su nombre.                                                      |
| `Alphabet`              | Conjunto finito de símbolos; se usa para `Σ` y para `Γ` (un único `Γ` para todas las cintas). |
| `Chain`                 | Cadena de entrada validada contra `Σ`.                                                    |
| `Movement`              | Enumerado `{L, R, S}`.                                                                    |
| `TapeAction`            | Lo que una transición hace sobre **una** cinta: símbolo leído, símbolo a escribir, movimiento. |
| `Tape`                  | Cinta infinita bidireccional; mapa disperso posición→símbolo, el resto se asume blanco.   |
| `Transition`            | `(origen, destino, [TapeAction])`, una acción por cinta.                                  |
| `TransitionFunction`    | La función `δ` completa; determinista: una única `Transition` por clave.                  |
| `TuringMachine`         | La séptupla `(Q, Σ, Γ, s, b, F, δ)` + número de cintas. Inmutable, no valida nada.         |
| `TuringConfiguration`   | Estado + cintas en un instante. **Mutable** (sin backtracking no hace falta inmutabilidad). |
| `TuringRunResult`       | Veredicto + contenido final de la cinta 1, ya formateado.                                 |
| `Simulator`             | Bucle de ejecución: aplica la única transición aplicable hasta que no quede ninguna.       |
| `TuringMachineParser`   | Lee el fichero y valida todo lo que aborta la carga (mismo patrón que `AutomatonParser` de P01). |
| `TuringMachineValidator`| Avisos que necesitan la MT completa: sin transiciones, estado inalcanzable, ningún final alcanzable. |
| `CommandLineOptions`    | Analiza y valida `-config`/`-in`/`-h`.                                                     |

### 4.5 Decisiones de diseño

1. **`TapeAction` agrupa lectura, escritura y movimiento de una misma cinta en una sola estructura**,
   en vez de tres vectores paralelos (`vector<Symbol> lee`, `vector<Symbol> escribe`,
   `vector<Movement> mueve`). Con vectores paralelos nada garantiza que los tres tengan la misma
   longitud ni que la posición *i* de cada uno se refiera a la misma cinta; con `TapeAction`, esa
   invariante la impone la propia estructura, no la disciplina de quien escribe el código.
2. **`TransitionFunction` indexa una única `Transition` por clave**, a diferencia de
   `vector<Transition>` en P01: la MT es determinista, así que dos transiciones con la misma clave
   `(estado, símbolos leídos)` y distinto resto no son una alternativa más (como en un AP no
   determinista) — son una contradicción. De ahí `NonDeterministicTransitionError`, que no existía
   en P01: una transición repetida *idéntica* se avisa e ignora, pero una que comparte clave con
   distinto destino/escritura/movimiento aborta la carga.
3. **`Tape` se representa como `map<long long, Symbol>` disperso**, no como un `vector` con
   desplazamiento. Así la cinta bidireccional no necesita ningún caso especial para el "extremo
   izquierdo": el índice admite valores negativos con la misma naturalidad que positivos, y solo
   ocupan memoria las celdas realmente escritas (escribir el blanco borra la celda).
4. **`TuringConfiguration` es mutable**, a diferencia de `InstantaneousDescription` (P01), que era
   inmutable a propósito. La inmutabilidad de P01 existía para que cada rama de una búsqueda con
   retroceso tuviera su propia copia sin interferir con las demás; aquí no hay retroceso ni ramas
   que coexistan, así que no hay nada que proteger copiando — aplicar la transición "in place" es
   más simple y no pierde nada.
5. **`TuringMachineValidator` no comprueba `Σ ∩ Γ ≠ ∅`**, a diferencia de `AutomatonValidator`
   (P01). En un AP esa intersección era casi siempre un descuido (`Σ` y `Γ` juegan papeles
   independientes); en una MT, `Σ ⊆ Γ` es lo esperable -la cinta tiene que poder representar la
   propia entrada-, así que ese aviso dispararía en cualquier MT bien diseñada y no aportaría nada.

---

## 5. Algoritmo de ejecución

Al ser determinista, no hace falta backtracking: en cada paso hay como mucho una transición
aplicable.

```
Run(cadena):
    cinta_1 = cadena (cabezal en el primer símbolo); cintas 2..N en blanco
    estado = q0
    mientras exista delta(estado, simbolos_leidos_en_cada_cinta):
        aplicar la transición (escribir + mover en cada cinta; cambiar de estado)
    devolver (estado pertenece a F, contenido de la cinta 1)
```

Sin límite de pasos: si la MT entra en un bucle que nunca para, el programa tampoco (tal y como
exige el enunciado: "la finalización de la ejecución vendrá determinada únicamente por la ausencia
de transiciones").

---

## 6. Gestión de errores

### Línea de comandos (`CommandLineError`)

| Situación                           | Tratamiento        |
| ------------------------------------- | -------------------- |
| Falta `-config`                     | Error + ayuda       |
| Opción repetida                     | Error + ayuda       |
| Opción sin valor                    | Error + ayuda       |
| Opción desconocida                  | Error + ayuda       |
| Fichero de `-config` o `-in` inaccesible | Error           |
| `-h`, `--help`                      | Muestra la ayuda; no es un error, se resuelve antes que el resto |

### Fichero de configuración

| Situación                                               | Excepción                          |
| --------------------------------------------------------- | ------------------------------------ |
| Fichero vacío o inaccesible                              | `FileError`                        |
| El fichero termina antes de una sección obligatoria      | `MissingSectionError`              |
| `Q` vacío                                                | `MissingSectionError`              |
| Más de un estado inicial, o línea del blanco con más de un token | `MissingSectionError`         |
| Estado repetido en `Q` o `F`                             | `DuplicatedElementError`           |
| Símbolo repetido en `Σ` o `Γ`                            | `DuplicatedElementError`           |
| Símbolo de más de un carácter (en `Σ`, `Γ`, o el blanco) | `InvalidSymbolError`               |
| El blanco no pertenece a `Γ`                             | `InvalidSymbolError`               |
| El blanco pertenece a `Σ`                                | `InvalidSymbolError`               |
| `q0 ∉ Q`, o algún estado de `F ∉ Q`                      | `InvalidStateError`                |
| Número de cintas no es un entero positivo                | `ConfigurationError`               |
| Transición con un número de campos distinto del esperado (`2+3·N`) | `InvalidTransitionError` |
| Estado de origen o destino no declarado                  | `InvalidTransitionError`           |
| Símbolo leído o escrito que no pertenece a `Γ`           | `InvalidTransitionError`           |
| Movimiento ajeno a `{L, R, S}`                           | `InvalidTransitionError`           |
| Dos transiciones con la misma clave y distinto resto (MT no determinista) | `NonDeterministicTransitionError` |

### Avisos (no abortan)

| Situación                        | Motivo                                   |
| ----------------------------------- | ------------------------------------------- |
| Transición duplicada (idéntica)   | Se ignora la repetición                    |
| MT sin ninguna transición         | Se detiene inmediatamente en `q0`          |
| Estado inalcanzable desde `q0`    | Estado inútil                              |
| Ningún estado de `F` alcanzable   | Ninguna cadena se aceptará nunca           |

### Cadenas de entrada

| Situación             | Tratamiento                                     |
| ------------------------ | -------------------------------------------------- |
| Símbolo ajeno a `Σ`   | `ChainError`; se descarta esa cadena y se continúa |

---

## 7. Batería de pruebas y Máquinas de Turing diseñadas

### Ejemplos de formato (proporcionados en el enunciado)

No son los problemas a resolver, solo ilustran la sintaxis del fichero:

| Fichero                 | Qué hace                                                  | Cintas |
| ----------------------- | ----------------------------------------------------------- | ------ |
| `test/Ejemplo1_MT.txt`  | Reconoce cadenas binarias con un número impar de ceros     | 1      |
| `test/Ejemplo2_MT.txt`  | Duplica un número en unario (`1ⁿ` → `1²ⁿ`)                 | 1      |

### Problema 1: `L = {aⁿbᵐ : m ≥ n, n > 0}` — `test/Problema1_anbm.txt` (2 cintas)

C1 contiene la entrada; C2 acumula una marca (`a`) por cada `a` leída en C1.

| Estado | Función |
| ------ | ------- |
| `q0`   | Inicial. Solo tiene transición para `a`: una cadena que empiece por `b`, o la vacía, no tiene transición y se rechaza de raíz (`n = 0`). |
| `q1`   | Modo `a`: marca cada `a` en C2; pasa a `q2` en cuanto lee la primera `b`. |
| `q2`   | Modo `b`: cada `b` consume una marca de C2; cuando C2 se agota sigue consumiendo `b` sin comparar (`m ≥ n` ya garantizado). Una `a` aquí no tiene transición: orden incorrecto, se rechaza. |
| `q3`   | Final. |

![Grafo de la MT del problema 1](docs/grafo_problema1.png)

### Problema 2: contar `a`/`b` en unario — pendiente

Diseño previsto con 3 cintas (C1 entrada/resultado, C2 marcas de `a`, C3 marcas de `b`); fichero y
grafo aún por añadir (`test/Problema2_conteo.txt`, `docs/grafo_problema2.png`).

-------------------- | ------------------------------------------------------------------ | ------ |
| `Ejemplo1_MT.txt`  | Reconoce cadenas binarias con un número impar de ceros            | 1      |
| `Ejemplo2_MT.txt`  | Duplica un número en unario (`1ⁿ` → `1²ⁿ`)                        | 1      |
| `Ejemplo3_MT.txt`  | Reconoce las cadenas aⁿbᵐ                                         | 1      |

---

## 8. Estructura del proyecto

```
practica_2/
├── CMakeLists.txt
├── MaquinaTuring.md
├── include/
│   ├── alphabet.h
│   ├── chain.h
│   ├── command_line_options.h
│   ├── errors.h
│   ├── movement.h
│   ├── simulator.h
│   ├── state.h
│   ├── symbol.h
│   ├── tape.h
│   ├── tape_action.h
│   ├── transition.h
│   ├── transition_function.h
│   ├── turing_configuration.h
│   ├── turing_machine.h
│   ├── turing_machine_parser.h
│   ├── turing_machine_validator.h
│   └── turing_run_result.h
├── src/
│   ├── alphabet.cc
│   ├── chain.cc
│   ├── command_line_options.cc
│   ├── main.cc
│   ├── simulator.cc
│   ├── tape.cc
│   ├── transition.cc
│   ├── transition_function.cc
│   ├── turing_configuration.cc
│   ├── turing_machine.cc
│   ├── turing_machine_parser.cc
│   └── turing_machine_validator.cc
├── test/
│   ├── Ejemplo1_MT.txt
│   ├── Ejemplo2_MT.txt
│   └── Problema1_anbm.txt
└── docs/
    ├── CC_2627_Practica2.pdf
    ├── grafo_problema1.dot
    ├── grafo_problema1.png
    └── grafo_problema1.svg
```

---

## 9. Referencias

1. Material de la asignatura Complejidad Computacional, Universidad de La Laguna (Moodle).
2. [*Organizing a C++ Project*](https://www.studyplan.dev/cmake/organizing-a-cpp-project) — STUDYPLAN.dev.
3. [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html).