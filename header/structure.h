#ifndef SKIP_LIST_H
#define SKIP_LIST_H

#include <vector>
#include <random>
#include <stdexcept>
#include <iterator>
#include <type_traits>
#include <memory>
#include <algorithm>
/**
 * @file skip_list.h
 * @brief Implementation of a Skip List data structure
 *
 * This file contains the implementation of a template-based Skip List container
 * with full iterator support, exception safety, and standard container interfaces.
 */

template <typename T, typename Compare = std::less<T>>
class SkipList {
public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = T&;
    using const_reference = const T&;
    using pointer = T*;
    using const_pointer = const T*;

    class Iterator;
    class ConstIterator;

    // Constructors
    explicit SkipList(double probability = 0.5, int max_level = 32);
    SkipList(const SkipList& other);
    SkipList(SkipList&& other) noexcept;
    ~SkipList();

    // Assignment operators
    SkipList& operator=(const SkipList& other);
    SkipList& operator=(SkipList&& other) noexcept;

    // Iterators
    Iterator begin() noexcept;
    Iterator end() noexcept;
    ConstIterator begin() const noexcept;
    ConstIterator end() const noexcept;
    ConstIterator cbegin() const noexcept;
    ConstIterator cend() const noexcept;

    // Modifiers
    std::pair<Iterator, bool> insert(const T& value);
    Iterator erase(Iterator pos);
    size_type erase(const T& value);
    void clear();

    // Lookup
    Iterator find(const T& value);
    ConstIterator find(const T& value) const;
    bool contains(const T& value) const;
    bool empty() const noexcept;
    size_type size() const noexcept;

    // Comparison operators
    bool operator==(const SkipList& other) const;
    bool operator!=(const SkipList& other) const;

private:
    struct Node;
    using NodePtr = Node*;

    /**
     * @struct Node
     * @brief Represents a node in the SkipList
     *
     * @tparam T Type of element stored in the node
     * @param value The stored data element
     * @param forward Vector of pointers to next nodes at each level
     * @param prev Pointer to the previous node in the base level
     */
    struct Node {
        T value;
        std::vector<NodePtr> forward;
        NodePtr prev;

        /**
         * @brief Constructs a new Node
         * @param val Value to store in the node
         * @param level Number of levels for this node
         */
        explicit Node(const T& val, int level);
    };

    // SkipList configuration parameters
    double probability_;   ///< Probability for node promotion
    int max_level_;        ///< Maximum allowed levels in the list
    int current_max_level_;///< Current highest level in use
    size_type size_;       ///< Number of elements in the list

    // Head and tail pointers
    NodePtr head_;         ///< Pointer to head node (dummy node)
    NodePtr tail_;         ///< Pointer to last node in base level

    // Comparator for ordering elements
    Compare comp_;         ///< Comparison function object

    // Random number generation for level determination
    mutable std::mt19937 gen_;     ///< Mersenne Twister random engine
    mutable std::uniform_real_distribution<double> dist_; ///< Uniform distribution [0.0, 1.0)

    // Helper methods
    int random_level() const;
    NodePtr find_node(const T& value) const;
    void link_forward(NodePtr node, int level, NodePtr next_node);
    void insert_node(NodePtr node, const std::vector<NodePtr>& predecessors);
};

/**
 * @class Iterator
 * @brief Bidirectional iterator for SkipList
 */
template <typename T, typename Compare>
class SkipList<T, Compare>::Iterator {
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;

    Iterator() noexcept : node_(nullptr) {}
    explicit Iterator(NodePtr node) noexcept : node_(node) {}

    reference operator*() const;
    pointer operator->() const;

    Iterator& operator++();
    Iterator operator++(int);
    Iterator& operator--();
    Iterator operator--(int);

    bool operator==(const Iterator& other) const noexcept;
    bool operator!=(const Iterator& other) const noexcept;

private:
    NodePtr node_;  ///< Current node pointer
    friend class SkipList;
};

/**
 * @class ConstIterator
 * @brief Constant bidirectional iterator for SkipList
 */
template <typename T, typename Compare>
class SkipList<T, Compare>::ConstIterator {
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = const T;
    using difference_type = std::ptrdiff_t;
    using pointer = const T*;
    using reference = const T&;

    ConstIterator() noexcept : node_(nullptr) {}
    explicit ConstIterator(NodePtr node) noexcept : node_(node) {}
    ConstIterator(const Iterator& it) noexcept : node_(it.node_) {}

    reference operator*() const;
    pointer operator->() const;

    ConstIterator& operator++();
    ConstIterator operator++(int);
    ConstIterator& operator--();
    ConstIterator operator--()(int);

    bool operator==(const ConstIterator& other) const noexcept;
    bool operator!=(const ConstIterator& other) const noexcept;

private:
    NodePtr node_;  ///< Current node pointer
    friend class SkipList;
};

// ==================== METHOD IMPLEMENTATIONS ====================

template <typename T, typename Compare>
SkipList<T, Compare>::Node::Node(const T& val, int level)
    : value(val), forward(level + 1, nullptr), prev(nullptr) {}

/**
 * @brief Constructs SkipList with given probability and max level
 * @param probability Probability for node promotion (0.0-1.0)
 * @param max_level Maximum allowed levels in the skip list
 */
template <typename T, typename Compare>
SkipList<T, Compare>::SkipList(double probability, int max_level)
    : probability_(probability),
      max_level_(max_level),
      current_max_level_(0),
      size_(0),
      head_(new Node(T(), max_level)),
      tail_(nullptr),
      comp_(),
      gen_(std::random_device{}()),
      dist_(0.0, 1.0) {}

/**
 * @brief Copy constructor
 * @param other SkipList to copy from
 */
template <typename T, typename Compare>
SkipList<T, Compare>::SkipList(const SkipList& other)
    : probability_(other.probability_),
      max_level_(other.max_level_),
      current_max_level_(0),
      size_(0),
      head_(new Node(T(), max_level_)),
      tail_(nullptr),
      comp_(other.comp_),
      gen_(std::random_device{}()),
      dist_(0.0, 1.0) {
    for (const auto& val : other) {
        insert(val);
    }
}

/**
 * @brief Move constructor
 * @param other SkipList to move from
 */
template <typename T, typename Compare>
SkipList<T, Compare>::SkipList(SkipList&& other) noexcept
    : probability_(other.probability_),
      max_level_(other.max_level_),
      current_max_level_(other.current_max_level_),
      size_(other.size_),
      head_(other.head_),
      tail_(other.tail_),
      comp_(std::move(other.comp_)),
      gen_(std::move(other.gen_)),
      dist_(std::move(other.dist_)) {
    other.head_ = new Node(T(), max_level_);
    other.current_max_level_ = 0;
    other.size_ = 0;
    other.tail_ = nullptr;
}

/**
 * @brief Destructor - cleans up all allocated nodes
 */
template <typename T, typename Compare>
SkipList<T, Compare>::~SkipList() {
    clear();
    delete head_;
}

/**
 * @brief Copy assignment operator
 * @param other SkipList to copy from
 * @return Reference to this object
 */
template <typename T, typename Compare>
SkipList<T, Compare>& SkipList<T, Compare>::operator=(const SkipList& other) {
    if (this != &other) {
        SkipList temp(other);
        std::swap(*this, temp);
    }
    return *this;
}

/**
 * @brief Move assignment operator
 * @param other SkipList to move from
 * @return Reference to this object
 */
template <typename T, typename Compare>
SkipList<T, Compare>& SkipList<T, Compare>::operator=(SkipList&& other) noexcept {
    if (this != &other) {
        clear();
        delete head_;

        probability_ = other.probability_;
        max_level_ = other.max_level_;
        current_max_level_ = other.current_max_level_;
        size_ = other.size_;
        head_ = other.head_;
        tail_ = other.tail_;
        comp_ = std::move(other.comp_);
        gen_ = std::move(other.gen_);
        dist_ = std::move(other.dist_);

        // Reset other's state
        other.head_ = new Node(T(), max_level_);
        other.current_max_level_ = 0;
        other.size_ = 0;
        other.tail_ = nullptr;
    }
    return *this;
}

/**
 * @brief Generates random level for new node
 * @return Random level based on probability distribution
 */
template <typename T, typename Compare>
int SkipList<T, Compare>::random_level() const {
    int level = 0;
    while (dist_(gen_) < probability_ && level < max_level_) {
        ++level;
    }
    return level;
}

/**
 * @brief Finds node containing given value
 * @param value Value to search for
 * @return Pointer to node if found, nullptr otherwise
 */
template <typename T, typename Compare>
typename SkipList<T, Compare>::NodePtr SkipList<T, Compare>::find_node(const T& value) const {
    NodePtr current = head_;
    for (int i = current_max_level_; i >= 0; --i) {
        while (current->forward[i] != nullptr && comp_(current->forward[i]->value, value)) {
            current = current->forward[i];
        }
    }
    current = current->forward[0];
    if (current != nullptr && !comp_(current->value, value) && !comp_(value, current->value)) {
        return current;
    }
    return nullptr;
}

/**
 * @brief Links node to next node at specified level
 * @param node Node to link
 * @param level Level to perform linking
 * @param next_node Next node in sequence
 */
template <typename T, typename Compare>
void SkipList<T, Compare>::link_forward(NodePtr node, int level, NodePtr next_node) {
    node->forward[level] = next_node;
    if (next_node != nullptr) {
        next_node->prev = node;
    }
}

/**
 * @brief Inserts node using predecessor information
 * @param node Node to insert
 * @param predecessors Vector of predecessor nodes at each level
 */
template <typename T, typename Compare>
void SkipList<T, Compare>::insert_node(NodePtr node, const std::vector<NodePtr>& predecessors) {
    int level = node->forward.size() - 1;
    for (int i = 0; i <= level; ++i) {
        link_forward(predecessors[i], i, node);
        node->forward[i] = predecessors[i]->forward[i];
        predecessors[i]->forward[i] = node;
    }
    if (predecessors[0] == head_) {
        node->prev = nullptr;
    } else {
        node->prev = predecessors[0];
    }
    if (node->forward[0] != nullptr) {
        node->forward[0]->prev = node;
    } else {
        tail_ = node;
    }
}

/**
 * @brief Inserts value into SkipList
 * @param value Value to insert
 * @return Pair with iterator to inserted element and bool indicating success
 */
template <typename T, typename Compare>
std::pair<typename SkipList<T, Compare>::Iterator, bool> SkipList<T, Compare>::insert(const T& value) {
    std::vector<NodePtr> predecessors(max_level_ + 1, head_);
    NodePtr current = head_;

    // Traverse from highest level down to base level
    for (int i = current_max_level_; i >= 0; --i) {
        while (current->forward[i] != nullptr && comp_(current->forward[i]->value, value)) {
            current = current->forward[i];
        }
        predecessors[i] = current;
    }
    current = current->forward[0];

    // Value already exists
    if (current != nullptr && !comp_(current->value, value) && !comp_(value, current->value)) {
        return {Iterator(current), false};
    }

    int new_level = random_level();
    if (new_level > current_max_level_) {
        for (int i = current_max_level_ + 1; i <= new_level; ++i) {
            predecessors[i] = head_;
        }
        current_max_level_ = new_level;
    }

    NodePtr new_node = new Node(value, new_level);
    insert_node(new_node, predecessors);
    ++size_;
    return {Iterator(new_node), true};
}

/**
 * @brief Erases element at given position
 * @param pos Iterator to element to erase
 * @return Iterator following last erased element
 */
template <typename T, typename Compare>
typename SkipList<T, Compare>::Iterator SkipList<T, Compare>::erase(Iterator pos) {
    if (pos == end()) {
        return end();
    }

    NodePtr node = pos.node_;
    Iterator next_it = pos;
    ++next_it;

    // Remove references to node at all levels
    for (size_t i = 0; i < node->forward.size(); ++i) {
        NodePtr prev_node = (i == 0) ? node->prev : head_;
        while (prev_node != nullptr && prev_node->forward.size() <= i) {
            if (prev_node == head_) break;
            prev_node = prev_node->prev;
        }
        if (prev_node != nullptr) {
            prev_node->forward[i] = node->forward[i];
        }
    }

    // Update backward pointers
    if (node->forward[0] != nullptr) {
        node->forward[0]->prev = node->prev;
    } else {
        tail_ = node->prev;
    }

    delete node;
    --size_;
    return next_it;
}

/**
 * @brief Erases elements with given value
 * @param value Value to erase
 * @return Number of elements erased (0 or 1)
 */
template <typename T, typename Compare>
typename SkipList<T, Compare>::size_type SkipList<T, Compare>::erase(const T& value) {
    auto it = find(value);
    if (it != end()) {
        erase(it);
        return 1;
    }
    return 0;
}

/**
 * @brief Removes all elements from SkipList
 */
template <typename T, typename Compare>
void SkipList<T, Compare>::clear() {
    NodePtr current = head_->forward[0];
    while (current != nullptr) {
        NodePtr next = current->forward[0];
        delete current;
        current = next;
    }
    head_->forward.assign(max_level_ + 1, nullptr);
    current_max_level_ = 0;
    size_ = 0;
    tail_ = nullptr;
}

/**
 * @brief Finds element with specific value
 * @param value Value to search for
 * @return Iterator to element if found, end() otherwise
 */
template <typename T, typename Compare>
typename SkipList<T, Compare>::Iterator SkipList<T, Compare>::find(const T& value) {
    return Iterator(find_node(value));
}

/**
 * @brief Finds element with specific value (const version)
 * @param value Value to search for
 * @return ConstIterator to element if found, end() otherwise
 */
template <typename T, typename Compare>
typename SkipList<T, Compare>::ConstIterator SkipList<T, Compare>::find(const T& value) const {
    return ConstIterator(find_node(value));
}

/**
 * @brief Checks if element exists in SkipList
 * @param value Value to search for
 * @return true if element found, false otherwise
 */
template <typename T, typename Compare>
bool SkipList<T, Compare>::contains(const T& value) const {
    return find_node(value) != nullptr;
}

/**
 * @brief Checks if SkipList is empty
 * @return true if empty, false otherwise
 */
template <typename T, typename Compare>
bool SkipList<T, Compare>::empty() const noexcept {
    return size_ == 0;
}

/**
 * @brief Returns number of elements in SkipList
 * @return Number of elements
 */
template <typename T, typename Compare>
typename SkipList<T, Compare>::size_type SkipList<T, Compare>::size() const noexcept {
    return size_;
}

/**
 * @brief Equality comparison operator
 * @param other SkipList to compare with
 * @return true if contents are equal, false otherwise
 */
template <typename T, typename Compare>
bool SkipList<T, Compare>::operator==(const SkipList& other) const {
    if (size_ != other.size_) return false;
    auto it1 = begin();
    auto it2 = other.begin();
    while (it1 != end()) {
        if (*it1 != *it2) return false;
        ++it1;
        ++it2;
    }
    return true;
}

/**
 * @brief Inequality comparison operator
 * @param other SkipList to compare with
 * @return true if contents are different, false otherwise
 */
template <typename T, typename Compare>
bool SkipList<T, Compare>::operator!=(const SkipList& other) const {
    return !(*this == other);
}

// ==================== ITERATOR IMPLEMENTATIONS ====================

/**
 * @brief Dereference iterator
 * @return Reference to element
 * @throws std::out_of_range if dereferencing end iterator
 */
template <typename T, typename Compare>
typename SkipList<T, Compare>::Iterator::reference
SkipList<T, Compare>::Iterator::operator*() const {
    if (node_ == nullptr) {
        throw std::out_of_range("Dereferencing end iterator");
    }
    return node_->value;
}

/**
 * @brief Member access operator
 * @return Pointer to element
 * @throws std::out_of_range if dereferencing end iterator
 */
template <typename T, typename Compare>
typename SkipList<T, Compare>::Iterator::pointer
SkipList<T, Compare>::Iterator::operator->() const {
    if (node_ == nullptr) {
        throw std::out_of_range("Dereferencing end iterator");
    }
    return &node_->value;
}

/**
 * @brief Prefix increment
 * @return Reference to advanced iterator
 * @throws std::out_of_range if incrementing end iterator
 */
template <typename T, typename Compare>
typename SkipList<T, Compare>::Iterator&
SkipList<T, Compare>::Iterator::operator++() {
    if (node_ == nullptr) {
        throw std::out_of_range("Incrementing end iterator");
    }
    node_ = node_->forward[0];
    return *this;
}

/**
 * @brief Postfix increment
 * @return Copy of iterator before advancement
 */
template <typename T, typename Compare>
typename SkipList<T, Compare>::Iterator
SkipList<T, Compare>::Iterator::operator++(int) {
    Iterator temp = *this;
    ++(*this);
    return temp;
}

/**
 * @brief Prefix decrement
 * @return Reference to decremented iterator
 * @throws std::out_of_range if decrementing end iterator
 */
template <typename T, typename Compare>
typename SkipList<T, Compare>::Iterator&
SkipList<T, Compare>::Iterator::operator--() {
    if (node_ == nullptr) {
        throw std::out_of_range("Decrementing end iterator");
    }
    node_ = node_->prev;
    if (node_ != nullptr && node_->prev == nullptr) {
        node_ = nullptr;
    }
    return *this;
}

/**
 * @brief Postfix decrement
 * @return Copy of iterator before decrement
 */
template <typename T, typename Compare>
typename SkipList<T, Compare>::Iterator
SkipList<T, Compare>::Iterator::operator--(int) {
    Iterator temp = *this;
    --(*this);
    return temp;
}

/**
 * @brief Iterator equality comparison
 * @param other Iterator to compare with
 * @return true if iterators refer to same element
 */
template <typename T, typename Compare>
bool SkipList<T, Compare>::Iterator::operator==(const Iterator& other) const noexcept {
    return node_ == other.node_;
}

/**
 * @brief Iterator inequality comparison
 * @param other Iterator to compare with
 * @return true if iterators refer to different elements
 */
template <typename T, typename Compare>
bool SkipList<T, Compare>::Iterator::operator!=(const Iterator& other) const noexcept {
    return node_ != other.node_;
}

// ==================== CONST ITERATOR IMPLEMENTATIONS ====================

/**
 * @brief Dereference const iterator
 * @return Const reference to element
 * @throws std::out_of_range if dereferencing end iterator
 */
template <typename T, typename Compare>
typename SkipList<T, Compare>::ConstIterator::reference
SkipList<T, Compare>::ConstIterator::operator*() const {
    if (node_ == nullptr) {
        throw std::out_of_range("Dereferencing end iterator");
    }
    return node_->value;
}

/**
 * @brief Member access operator (const)
 * @return Pointer to element
 * @throws std::out_of_range if dereferencing end iterator
 */
template <typename T, typename Compare>
typename SkipList<T, Compare>::ConstIterator::pointer
SkipList<T, Compare>::ConstIterator::operator->() const {
    if (node_ == nullptr) {
        throw std::out_of_range("Dereferencing end iterator");
    }
    return &node_->value;
}

/**
 * @brief Prefix increment (const)
 * @return Reference to advanced iterator
 * @throws std::out_of_range if incrementing end iterator
 */
template <typename T, typename Compare>
typename SkipList<T, Compare>::ConstIterator&
SkipList<T, Compare>::ConstIterator::operator++() {
    if (node_ == nullptr) {
        throw std::out_of_range("Incrementing end iterator");
    }
    node_ = node_->forward[0];
    return *this;
}

/**
 * @brief Postfix increment (const)
 * @return Copy of iterator before advancement
 */
template <typename T, typename Compare>
typename SkipList<T, Compare>::ConstIterator
SkipList<T, Compare>::ConstIterator::operator++(int) {
    ConstIterator temp = *this;
    ++(*this);
    return temp;
}

/**
 * @brief Prefix decrement (const)
 * @return Reference to decremented iterator
 * @throws std::out_of_range if decrementing end iterator
 */
template <typename T, typename Compare>
typename SkipList<T, Compare>::ConstIterator&
SkipList<T, Compare>::ConstIterator::operator--() {
    if (node_ == nullptr) {
        throw std::out_of_range("Decrementing end iterator");
    }
    node_ = node_->prev;
    if (node_ != nullptr && node_->prev == nullptr) {
        node_ = nullptr;
    }
    return *this;
}

/**
 * @brief Postfix decrement (const)
 * @return Copy of iterator before decrement
 */
template <typename T, typename Compare>
typename SkipList<T, Compare>::ConstIterator
SkipList<T, Compare>::ConstIterator::operator--(int) {
    ConstIterator temp = *this;
    --(*this);
    return temp;
}

/**
 * @brief ConstIterator equality comparison
 * @param other Iterator to compare with
 * @return true if iterators refer to same element
 */
template <typename T, typename Compare>
bool SkipList<T, Compare>::ConstIterator::operator==(const ConstIterator& other) const noexcept {
    return node_ == other.node_;
}

/**
 * @brief ConstIterator inequality comparison
 * @param other Iterator to compare with
 * @return true if iterators refer to different elements
 */
template <typename T, typename Compare>
bool SkipList<T, Compare>::ConstIterator::operator!=(const ConstIterator& other) const noexcept {
    return node_ != other.node_;
}

// ==================== CONTAINER ITERATORS ====================

/**
 * @brief Returns iterator to first element
 * @return Iterator to beginning of list
 */
template <typename T, typename Compare>
typename SkipList<T, Compare>::Iterator SkipList<T, Compare>::begin() noexcept {
    return Iterator(head_->forward[0]);
}

/**
 * @brief Returns end iterator
 * @return Iterator to end of list
 */
template <typename T, typename Compare>
typename SkipList<T, Compare>::Iterator SkipList<T, Compare>::end() noexcept {
    return Iterator(nullptr);
}

/**
 * @brief Returns const iterator to first element
 * @return ConstIterator to beginning of list
 */
template <typename T, typename Compare>
typename SkipList<T, Compare>::ConstIterator SkipList<T, Compare>::begin() const noexcept {
    return ConstIterator(head_->forward[0]);
}

/**
 * @brief Returns const end iterator
 * @return ConstIterator to end of list
 */
template <typename T, typename Compare>
typename SkipList<T, Compare>::ConstIterator SkipList<T, Compare>::end() const noexcept {
    return ConstIterator(nullptr);
}

/**
 * @brief Returns const iterator to first element
 * @return ConstIterator to beginning of list
 */
template <typename T, typename Compare>
typename SkipList<T, Compare>::ConstIterator SkipList<T, Compare>::cbegin() const noexcept {
    return begin();
}

/**
 * @brief Returns const end iterator
 * @return ConstIterator to end of list
 */
template <typename T, typename Compare>
typename SkipList<T, Compare>::ConstIterator SkipList<T, Compare>::cend() const noexcept {
    return end();
}


#endif // SKIP_LIST_H
