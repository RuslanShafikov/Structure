# SkipList Test Documentation

## Default Constructor
- **Purpose**: Verify default initialization
- **Tests**:
    - List is empty after construction
    - Size is 0
    - Begin and end iterators are equal

## Insert and Size
- **Purpose**: Test element insertion
- **Tests**:
    - New element insertion returns success
    - Size increases after insertion
    - Duplicate insertion fails
    - Size remains unchanged for duplicates

## Find and Contains
- **Purpose**: Test element lookup
- **Tests**:
    - Existing elements are found
    - Non-existent elements return end iterator
    - Contains returns correct boolean

## Erase
- **Purpose**: Test element removal
- **Tests**:
    - Existing elements are removed
    - Size decreases after removal
    - Non-existent elements return 0 removals
    - Iterator remains valid after erase

## Forward Iteration
- **Purpose**: Test sequential access
- **Tests**:
    - Elements are traversed in sorted order
    - Works with arbitrary insertion order
    - Matches sorted vector output

## Reverse Iteration
- **Purpose**: Test reverse traversal
- **Tests**:
    - Correct element order from end to begin
    - Throws exception when decrementing beyond begin

## Copy Constructor
- **Purpose**: Test copy semantics
- **Tests**:
    - Copied list matches original size
    - Elements are identical
    - Modifications don't affect original

## Move Operations
- **Purpose**: Test move semantics
- **Tests**:
    - Source list is empty after move
    - Destination contains all elements
    - Works for constructor and assignment

## Clear
- **Purpose**: Test reset functionality
- **Tests**:
    - Size becomes 0 after clear
    - List is empty
    - Iterators become equal

## Comparison Operators
- **Purpose**: Test equality checks
- **Tests**:
    - Identical lists are equal
    - Different lists are not equal
    - Empty vs non-empty comparison

## Custom Comparator
- **Purpose**: Test custom ordering
- **Tests**:
    - Case-insensitive string comparison
    - Correct element lookup
    - Case-insensitive contains

## Error Handling
- **Purpose**: Test edge case behavior
- **Tests**:
    - End iterator dereference throws
    - Begin iterator decrement throws
    - End iterator increment throws

## Subscript Operator Access
- **Purpose**: Test index-based access
- **Tests**:
    - Elements accessible by index
    - Elements modifiable via subscript
    - Correct ordering (0 = smallest)

## Const Subscript Operator
- **Purpose**: Test read-only access
- **Tests**:
    - Const access works
    - Correct values returned
    - Works on const references

## At Method Access
- **Purpose**: Test bounds-checked access
- **Tests**:
    - Correct elements returned
    - Elements modifiable via at()
    - Bounds checking enforced

## Const At Method
- **Purpose**: Test read-only bounds-checked access
- **Tests**:
    - Const at() works
    - Correct values returned
    - Works on const references

## Out of Range Access
- **Purpose**: Test bounds validation
- **Tests**:
    - Negative indices throw
    - Oversized indices throw
    - Edge cases handled

## Empty List Access
- **Purpose**: Test empty collection behavior
- **Tests**:
    - Any access throws out_of_range
    - Consistent error behavior

## Sequential Access Consistency
- **Purpose**: Test large dataset integrity
- **Tests**:
    - Sequential insertion preserves order
    - All elements accessible by index
    - Correct values at each position

## Reverse Order Insertion
- **Purpose**: Test different insertion pattern
- **Tests**:
    - Reverse insertion produces sorted order
    - Elements accessible in ascending order
    - Consistent with forward insertion

## Access After Modification
- **Purpose**: Test mutation integrity
- **Tests**:
    - Subscript modifications persist
    - at() modifications persist
    - Changes visible in subsequent accesses

## String Type Access
- **Purpose**: Test non-integer types
- **Tests**:
    - String elements work with accessors
    - String modification via subscript
    - String modification via at()

## Large Dataset Access
- **Purpose**: Test scalability
- **Tests**:
    - 10,000 elements handled correctly
    - Random access returns correct values
    - Consistent behavior at scale