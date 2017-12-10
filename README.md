[Style Guide](https://google.github.io/styleguide/cppguide.html)

[Exercises](https://www.smu.edu/Guildhall/Admissions/Portfolio-Requirements/Programming)

# Resource Manager
## Core Requirements
- [x] Support any amount of nodes
- [x] Read file and parse into nodes with dependencies
- [x] Display current view of graph
- [ ] Display usability of nodes
- [ ] Allow node deletion
- [x] Display menu and allow quit
- [ ] Zero memory leaks

## Hidden Requirements
- [x] Cycle detection

## Extras
- [x] Breadth first traversal
- [x] Preorder depth first traversal
- [x] Postorder depth first traversal
- [ ] Allow inserting of new nodes and links
- [ ] Save graph on exit

## Issues
- [ ] Adding a node through the CLI does not show up upon further iteration, but memory is allocated.
- [ ] Delete node -> add node -> list nodes: results in a memory error.
- [ ] Usability does not do a deep traversal. If a resource's dependency is unusable, that resource is now unusable.

## Next Steps
- [ ] Use smart pointers where appropriate.
- [ ] Pass references instead of huge containers.
- [ ] It may be preferable to use a _list_ instead of a _vector_ as a primary data structure.
