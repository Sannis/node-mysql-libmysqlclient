Syntax
======

Comments
--------

Documentation comments start with `/**` and end with `**/`. Each new line starts with `*`. 

    /** ...
     *  ...
     **/

Tags (optional)
----

The opening line of a comment is reserved for tags. Tags are optional. Tags are separated by a comma followed by optional whitespace(s) (`, `). They can be either a tag name or a key / value pair separated by a colon and optional whitespace(s) (`: `):

    tagName ': ' tagValue [', ' tagName ': ' tagValue]...

Currently supported tags are:

  * `section: <name>`           - description belongs to specified section `name`
  * `alias of: <name>`          - entity is another name for entity `name`
  * `related to: <name>`        - description is related to entity `name`
  * `read-only`                 - entity is read-only
  * `internal`                  - entity is meant to be private
  * `chainable`                 - method can be chained, i.e. the return value is the same object to which method belongs. E.g. $(...).on(...).on(...)
  * `deprecated`                - entity is considered deprecated. Deprecation tag in addition have the following flavors:
    * `deprecated: <from>`        - deprecated starting from version `from`
    * `deprecated: <from>..<out>` - deprecated starting from version `from` and will be removed by version `out`

E.g.:

    /** deprecated: 1.0..2.0, section: DOM, alias of: Element#descendantOf, chainable
     *  Element#childOf(@element, className) -> Element
     *
     *  ...
     **/



EBNF
----

The lines _directly following_ tags are reserved for the EBNF description of the documented object. Typically, there's only one EBNF per documented object:

    /** 
     *  Element#down(@element[, cssSelector][, index]) -> Element | null
     *
     *  ...
     **/

However, methods, functions, etc. may have several signatures, hence may require more than one description line, in which case description lines directly follow each other:

    /** 
     *  Element#writeAttribute(@element, attribute[, value = true]) -> Element
     *  Element#writeAttribute(@element, attributes) -> Element
     *
     *  ...
     **/
     
### Arguments

For all methods, functions, etc. parentheses around the arguments are required even if no arguments are present.
The syntax for arguments is as follows:

#### required arguments

    /** 
     *  Event.stop(event) -> Event
     *
     *  ...
     **/
     
#### optional arguments

Optional arguments are surrounded by squared brackets (`[]`).

    /** 
     *  String#evalJSON([sanitize]) -> Object | Array
     *
     *  ...
     **/

A default value may be indicated using the equal sign (`=`).
     
    /** 
     *  String#evalJSON([sanitize = false]) -> Object | Array
     *
     *  ...
     **/
     

Note that the argument separating comas belong _inside_ the brackets.

    /** 
     *  Event.findElement(event[, cssSelector]) -> Element | null
     *
     *  ...
     **/     
     
Arguments can be described below the EBNF description using the following syntax:
    
    - argumentName (acceptedType | otherAcceptedType | ...): description.
     
### Supported EBNF types

**BIG FAT WARNING**: EBNF descriptions **must be separated by an empty comment line** from the rest of description:

    /** 
     *  String#evalJSON([sanitize = false]) -> Object | Array
     *
     *  Here goes markdown for String#evalJSON description.
     **/
     

    /** 
     *  Event.findElement(event[, cssSelector]) -> Element | null
     *  - event (Event): a native Event instance
     *  - cssSelector (String): a optional CSS selector which uses
     *  the same syntax found in regular CSS.
     *
     *  Regular method markdown goes here.
     **/

Descriptions are parsed as markdown markup, with github-style extension for code blocks 
     
#### Namespace

    /** 
     *  Ajax
     *
     *  ...
     **/
     
    /** 
     *  Prototype.Browser
     *
     *  ...
     **/
     
#### Classes

Classes require a `class` prefix:

    /** 
     *  class Ajax.Base
     *
     *  ...
     **/

Sub-classes can indicate their parent just like in the Ruby syntax:

    /** 
     *  class Ajax.Request < Ajax.Base
     *
     *  ...
     **/

Where `Ajax.Base` is the parent class and `Ajax.Request` the subclass.

Included mixins are indicated like so:

    /** 
     *  class CustomHash
     *  includes Enumerable, Comparable
     *
     *  ...
     **/

#### Mixins

Mixins are indicated by a `mixin` prefix:

    /** 
     *  mixin Enumerable
     *
     *  ...
     **/

#### Constructors

Constructors require the `new` prefix and their arguments.

    /** 
     *  new Element(tagName[, attributes])
     *
     *  ...
     **/
          
    /** 
     *  new Foobar()
     *
     *  ...
     **/
     
#### Class Methods

Class methods are identified by a dot (`.`).

    /** 
     *  Array.from([iterable]) -> Array
     *
     *  ...
     **/

#### Instance Methods

Instance methods are identified by the hash symbol (`#`).

    /** 
     *  Array#first() -> Array element
     *
     *  ...
     **/
     
#### Utilities

Utilities are global functions starting with a dollar-sign (`$`).

    /** 
     *  $w(string) -> Array
     *
     *  ...
     **/
     
#### Methodized Methods

Methodized methods are methods which are both available as a class method and an instance method, in which case the first argument becomes the instance object itself. For example, all of `Element`'s instance methods are methodized and thus also available as class methods of `Element`. Methodized methods are indicated by prefixing their first argument with the `@` symbol.

    /** 
     *  Element#hide(@element) -> Element
     *
     *  ...
     **/
     
#### Class Properties

Class properties are identified by a dot (`.`).

    /** 
     *  Ajax.Responders.responders -> Array
     *
     *  ...
     **/
     
#### Instance Properties

Instance properties are identified by the hash symbol (`#`).

    /** 
     *  Ajax.Response#responseText -> String
     *
     *  ...
     **/
     
#### Constants

Constant must have their value specified using the equal sign (`=`).

    /** 
     *  Prototype.JSONFilter = /^\/\*-secure-([\s\S]*)\*\/\s*$/
     *
     *  ...
     **/
     
#### Events

Events are identified by `@`:

    /** 
     *  Features@head(request, socket, head)
     *
     *  ...
     **/

Sugar
----

### Sections

Sections are grouped parts of documentation. They don't add to element hierarchy, but just help to organize documentation.

    /** 
     *  == Section Name ==
     *
     *  Describe this section here.
     **/

### Short links

Short links help to quickly refer some element. Supported are two flavors:

  * `[[Method.Name]]`         - renders to `<a href=HREF>Method.Name</a>`
  * `[[Method.Name TEXT]]`    - renders to `<a href=HREF>TEXT</a>`

Difference from PDoc
--------------------

1. Descriptions should be ALWAYS separated by empty line from the upper string (signature, section, tags...).
2. `deprecated` can have options.
3. Additional tags - read-only, internal, chainable.
4. Events support.
