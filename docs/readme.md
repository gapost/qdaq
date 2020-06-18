# Build QDaq documentation

The documentation is built with doxygen with the configuration file Doxyfile.

doxygen should be run from within the docs folder. The generated html files go into docs/html

The docs/index.html is just a redirection to docs/html/index.html in order for github "pages" to work.

The *.dox files contain documentation text.

# Documentation and git

The documentation files in docs/html are part of the master branch.

Thus when rebuilding the documentation there is a large number of changed files that messes up the git history.

To avoid that:

- While working on the documentation and constantly regenerating the doxy output, temporarily change the output folder to some location outside the qdaq tree
- When you finish your changes to the documentation source files (in *.dox, *.cpp, *.h), first commit these changes with an indicative comment
- Then rebuild the doxygen output and commit all files in docs/html to a single commit with comment like "Rebuild documentation"


