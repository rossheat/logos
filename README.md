# Logos
Logos is an interpreter capable of parsing and evaluating arbitrarily complex propositional logic statements.

## Building
To build:
```bash
make
```

This will create the `logos` executable.
To run the test suite:
```bash
make test
```

## Usage
Start the REPL:
```bash
./logos
```
Exit with 'exit' or 'quit'.

### Basic operations
1. Set variables:
```
>> SET P true
>> SET Q false
```
2. Enable AST output (displays the Abstract Syntax Tree of expressions):
```
>> SET OUTPUT_AST true
```
3. Evaluate expressions:
```
>> P & Q
Result: false
```

### Example
```
>> SET P true
>> SET Q false
>> SET R true
>> SET S true
>> SET OUTPUT_AST true
>> ((P -> Q) & (R -> S)) <-> (~P | ~R)
AST:
Infix[
  Operator: <->
  Left:
    Infix[
      Operator: &
      Left:
        Infix[
          Operator: ->
          Left:
            Identifier(P)
          Right:
            Identifier(Q)
        ]
      Right:
        Infix[
          Operator: ->
          Left:
            Identifier(R)
          Right:
            Identifier(S)
        ]
    ]
  Right:
    Infix[
      Operator: |
      Left:
        Prefix[
          Operator: ~
          Right:
            Identifier(P)
        ]
      Right:
        Prefix[
          Operator: ~
          Right:
            Identifier(R)
        ]
    ]
]
Result: true
```

### Operators in order of precedence
- `~` (NOT)
- `&` (AND)
- `|` (OR), `^` (XOR)
- `->` (IMPLIES)
- `<->` (IFF/Bi-implication)

## Thanks to Vaughan Pratt 
Logos uses Pratt parsing (also known as "Top Down Operator Precedence Parsing"), first described by Vaughan Pratt in his 1973 paper "Top Down Operator Precedence". This method is more adept at dealing with expressions than regular recursive descent parsing. 

## License
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.