# SkipList Implementation Requirements

1. **Template Implementation**:
    - Must support any data type `T`
    - Customizable comparison function (default: `std::less<T>`)

2. **Core Functionality**:
    - Insertion with automatic sorting
    - Deletion by iterator or value
    - Efficient search operations
    - Duplicate prevention
    - Clear all elements

3. **Memory Management**:
    - Proper ownership of dynamically allocated nodes
    - Deep copying for copy operations
    - Move semantics support

4. **Iterator Support**:
    - Bidirectional iterators (forward/backward traversal)
    - Const-correct iterators
    - Exception safety for invalid operations

5. **Operational Characteristics**:
    - O(log n) average time complexity for search/insert/delete
    - Configurable:
        - Maximum skip list height (default: 32)
        - Probability factor (default: 0.5)

6. **Utility Operations**:
    - Size tracking
    - Empty check
    - Container comparison (==, !=)

7. **Exception Safety**:
    - Basic exception guarantee for most operations
    - Strong exception guarantee for:
        - `insert()`
        - `erase()`
        - `clear()`

8. **Thread Safety**:
    - Not thread-safe (client must manage synchronization)