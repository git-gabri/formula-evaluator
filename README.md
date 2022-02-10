# Formula evaluator

This piece of software resembles more of an rpn calculator.

Compile it using `cmake`.

To use it launch the executable and encapsulate the expression in double quotes like this: `./formula_eval "2 2 +"`

## Available commands
The "# consumed" is the number of elements consumed from the stack.  
The "# pushed" is the number of elements pushed on the stack.  
The elements on the stack are referred as follows:
```
Bottom          Top
  |              |
  V              V
 btm [.....] z y x    <- Newly pushed elements end up here
```
Number constants are verified with this regex:
```
^[+-]?[0-9]+(?:\.[0-9]+)?(?:[eE][+-]?[0-9]+)?$
```
| String command | # consumed | # pushed | Comment |
| --- | --- | --- | --- |
| number | 0 | 1 | Pushes number constant on the stack |
| `dup` | 1 | 2 | Duplicates `x` |
| `pop` | 1 | 0 | Removes `x` |
| `swp` | 2 | 2 | Swaps `x` and `x` |
| `+` | 2 | 1 | Performs `y + x` |
| `-` | 2 | 1 | Performs `y - x` |
| `*` | 2 | 1 | Performs `y * x` |
| `/` | 2 | 1 | Performs `y / x` |
| `root2`, `sqrt` | 1 | 1 | Square root of `x` |
| `root3`, `cbrt` | 1 | 1 | Cubic root of `x` |
| `root4` | 1 | 1 | 4-th root of `x` |
| `root` | 2 | 1 | `y`-th root of `x` |
| `exp` | 1 | 1 | e ^ `x` |
| `exp2` | 1 | 1 | 2 ^ `x` |
| `exp10` | 1 | 1 | 10 ^ `x` |
| `pow` | 2 | 1 | `y` ^ `x` |
| `ln` | 1 | 1 | Natural logarithm of `x`, ln(`x`) |
| `log2` | 1 | 1 | Logarithm in base 2 of `x` |
| `log10` | 1 | 1 | Logarithm in base 10 of `x` |
| `log` | 2 | 1 | Logarithm in base `y` of `x` |
| `sin` | 1 | 1 | sin(`x`), sine of `x` |
| `cos` | 1 | 1 | cos(`x`), cosine of `x` |
| `tan` | 1 | 1 | tan(`x`), tangent of `x` |
| `asin` | 1 | 1 | arcsin(`x`), inverse sine of `x` |
| `acos` | 1 | 1 | arccos(`x`), inverse cosine of `x` |
| `atan` | 1 | 1 | arctan(`x`), inverse tangent of `x` |
| `sinh` | 1 | 1 | sinh(`x`), hyperbolic sine of `x` |
| `cosh` | 1 | 1 | cosh(`x`), hyperbolic cosine of `x` |
| `tanh` | 1 | 1 | tanh(`x`), hyperbolic tangent of `x` |
| `asinh` | 1 | 1 | arcsinh(`x`), inverse h. sine of `x` |
| `acosh` | 1 | 1 | arccosh(`x`), inverse h. cosine of `x` |
| `atanh` | 0 | 1 | arctanh(`x`), inverse h. tangent of `x` |