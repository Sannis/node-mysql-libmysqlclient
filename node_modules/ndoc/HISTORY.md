3.1.0 / 2013-06-24
------------------

* Removed all async code, API is also sync now
* Improved error reporting for invalid syntax (#72)
* Different syntax fixes
* Removed mincer, files are rendered directly


3.0.1 / 2013-06-19
------------------

* Fixed signatures values (#70)
* Changed generated reference links names to full format
* Updated dependencies


3.0.0 / 2013-06-18
------------------

* Completely rewritten syntax parser (jison -> pegjs)
* Improved error reporting
* Multiple fixes


2.0.5 / 2013-02-16
------------------

* Fixed bug, that caused problems with new `argparse`
* Updated dependencies
* Updated linting rules


2.0.4 / 2012-08-09
------------------

* Fixed main index file path (when ndoc used as library)
* Updated dependencies versions


2.0.3 / 2012-07-29
------------------

* Fixed name/prefix for nested event items. Thanks to @Sannis.


2.0.2 / 2012-07-19
------------------

* Add `--alias <from>:<to>` option. Closes #58.
* [API] cli.findFiles now support functions.


2.0.1 / 2012-07-08
------------------

* Enhance `.ndocrc` reader to support comments.


2.0.0 / 2012-07-02
------------------

* `Internal` methods are now skipped by default.
* Stop on collisions, instead of warning
* Bundled mode (all doc in single file), layout/styles cleanup.
* Highlight on build time (no more hangs on big docs load)
* Ability to add github ribbon.
* Multiple folders/files support.
* Parcers/Generators can be attached as external modules
* Reorganized options (see new help).
* Options can be loaded from file now.
* Grand refactoring.
* Multiple bugfixes (see ussue tracker).


1.0.7 / 2012-02-09
------------------

* Improve error reporting.
* Partial code cleanup.


1.0.6 / 2012-02-05
------------------

* Fixed links to sources for windows (forced '\' to '/'). Closes #37.
  Thanks to Sascha Drews (perfusorius).


1.0.5 / 2012-01-04
------------------

* Updated `Marked` version. Now supports ``` blocks.
  Thanks to Christopher Jeffrey (chjj).


1.0.4 / 2011-12-15
------------------

* Styled new tags (read-only, internal, chainable). Closes #11.
* Fixed signatures with complex params. Closes #30.


1.0.3 / 2011-12-14
------------------

* Added events support,
* Added nested arguments support in signatures (for callbacks & iterators)
* Added back icons for global methods/properties


1.0.2 / 2011-12-13
------------------

* Fixed highlight of last menu item after scroll, closes #17
* Fixed boilerplate layout
* Fixed missed slash in links to source code, closes #19


1.0.1 / 2011-12-07
------------------

* Fixed default "view source" link for github


1.0.0 / 2011-12-07
------------------

* Total refactoring + feature-complete now
* Skin rewrite
* `Prototype` as test


0.1.0 / 2011-11-24
------------------

* First public release
