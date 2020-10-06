# Fel language
Fel is greatly inspired by a mix of both javascript and lua but also a dash of C++ and python.

# Comments
Fel uses c-style comments with the distinction that multi line comments can be nested (though there are problems rendering it).

```javascript
// single line comment

/* multi line
   /* this comment is commented out */
   comment */
```

# Functions

```javascript
let foo = fun(){};
fun bar(){return 42;}
let baz = (a, b, c) => a+b+c;
```

There is no difference how functions are defined. All capture their scope and this referes to the calling object.

```javascript
foo();
bar(42);
baz(1, 2, 3);
```

# Strings

```javascript
"there are double quotes"
'and single quotes'
'and escapes \n\r\t\'\" and \\'
"all \'escape\' characters work in both strings"
```

# variables

```javascript
var this_is_a_variable; // like javascript let
let this_is_a_const_ref; // like javascript const
const this_is_a_constant; // like c++ const
```

# objects

```javascript
var obj = {
    p1: 3,
    p2: 42,
    variable: variable,
    another_variable, // same as above
    f1() = {},
    f2: () => {}, // comma optional here
};
```

# destructuring

```javascript
let {a, b, c} = some_function();

// f is a function that takes a object with a property called name, and sends it to the name parameter
// second parameter is a another destructuring that that has a default value if the property is missing and a default property of empty object
let f = ({name}, {println=print}={}) => { println(name); };

let [first, ...rest] = some_array;
let {remove_this, ...clean} = some_object;
let {...shallow_copy} = some_other_object;
```
