# Resource Manager
### Core Requirements
- [x] Support any amount of nodes
- [x] Read file and parse into nodes with dependencies
- [x] Display menu and allow quit
- [x] Display current view of graph
- [ ] Display usability of nodes (issue: needs to do deep traversal check)
- [x] Allow node deletion
- [ ] Check for memory leaks

### Hidden Requirements
- [x] Cycle detection

### Extras
- [x] Breadth first traversal
- [x] Preorder depth first traversal
- [x] Postorder depth first traversal
- [x] Allow inserting of new nodes and links
- [ ] Fix traversals as they are broken by incomplete change to `forward_list`
- [ ] Use smart pointers where appropriate
- [ ] Pass containers by reference
- [ ] Save graph on exit

[Style Guide](https://google.github.io/styleguide/cppguide.html)
