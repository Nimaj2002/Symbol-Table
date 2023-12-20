# Symbol-Table

A simple Symbol Table implementation based on the Compilers Principles, Techniques, &amp; Tools book

how to use(after compiling using make command):

```bash
./magician sourcefile.magic
```

test files availible under the folder testinput

### Project descriptions
Determining the types of identifiers in the static domain for block-structured programs:
(there is a Block-structured programs consist of optional declarations and statements containing identifiers. The goal is to  translate and print a program where declarations are removed, and each statement includes its identifier along with the type of that identifier.)
1. Obtaining a block-structured program from the user:
- It is assumed that the entire program is considered as the main block, starting with "begin" and ending with
the "end" statement, and it does not include any declarations or statements.
- Each block in the program begins with "{" and ends with "} ".
- Blocks can be followed by each other or nested within each other (or a combination of both).
- Each block may or may not have declarations or statements.
- Allowed types for identifier declarations are char, int, float, and bool.
- The structure of identifiers is a combination of letters, numbers, and "_", with the first character not being a digit, and its maximum length
is 7 characters.
- Declarations and statements are not case-sensitive.
- Whitespace (blank, tab, and newline) or comments can be used between declarations and statements. Comments can be block (between */ and /*) or
line-based (after // until the end of the line).
2. Translating and printing the new program:
- In each row of the symbol chain tables, the type of the identifier along with its type is placed.
- The structure of symbol chain tables can be implemented as a concatenated table.
(Ready-made constructs in programming languages can be used in this regard.)
- Declarations, all white spaces, and comments are removed in the output.
- In the statements section, after each identifier, a colon ":" is placed, followed by the type of the identifier in the output.
- Keywords "begin," "end," and identifier types are written in lowercase in the output.
- In the output, after "begin," "{," "}," and ";" a blank is added.

° For this project, you are only allowed to use the method mentioned in the design of compilers, and
the use of other methods or ready-made tools is not allowed (meaning designing a grammar, adding
semantic actions to the grammar, removing left recursions from the grammar if any, designing and
implementing a predictive recursive parser, and creating concatenated symbol chain tables).