# Nova v1 language reference

## Values and output
Supported values are numbers, quoted strings, booleans (`true`, `false`) and `nil`.
`say expression` and `print expression` output the value followed by a newline.
Strings support `\\n`, `\\t`, and escaped characters.

## Variables
```nova
let score = 10
score = score + 5
say score
```
Declare using `let`; assignment updates an existing variable. Blocks introduce local scopes.

## Operators
Arithmetic: `+`, `-`, `*`, `/`, `%`; comparisons: `< <= > >=`; equality: `== !=`; logic: `and or !`. Parentheses group expressions. Plus concatenates if either operand is a string. Comparisons currently require numbers. Division by zero is an error.

## Conditions and loops
```nova
if (score >= 15) { say "Great" } else { say "Keep going" }
let i = 0
while (i < 3) { say i; i = i + 1 }
```
Conditions require parentheses and braces around blocks. Loops re-evaluate their condition before each iteration.

## Comments
Lines may begin with `#` or `//`.

## CLI
`nova file.nova`, `nova --help`, `nova --version`; running with no file starts a line-oriented prompt.

## Limitations
Functions, collections, imports, package management, async execution, bytecode and native compilation are not included in v1.
