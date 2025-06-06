//
// Created by ruslan on 5/21/25.
//

#ifndef STRUCTURE_H
#define STRUCTURE_H

#include <vector>
#include <random>
#include <stdexcept>
#include <iterator>
#include <type_traits>
#include <memory>
#include <algorithm>
#include <concepts>
#include <cassert>

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

    explicit SkipList(double probability = 0.5, int max_level = 32);
    SkipList(const SkipList& other);
    SkipList(SkipList&& other) noexcept;
    ~SkipList();


    SkipList& operator=(const SkipList& other);
    SkipList& operator=(SkipList&& other) noexcept;


    Iterator begin() noexcept;
    Iterator end() noexcept;
    ConstIterator begin() const noexcept;
    ConstIterator end() const noexcept;
    ConstIterator cbegin() const noexcept;
    ConstIterator cend() const noexcept;


    std::pair<Iterator, bool> insert(const T& value);
    Iterator erase(Iterator pos);
    size_type erase(const T& value);
    void clear();

    reference at(size_type index);
    const_reference at(size_type index) const;
    reference operator[](size_type index);
    const_reference operator[](size_type index) const;

    Iterator find(const T& value);
    ConstIterator find(const T& value) const;
    bool contains(const T& value) const;
    bool empty() const noexcept;
    size_type size() const noexcept;


    bool operator==(const SkipList& other) const;
    bool operator!=(const SkipList& other) const;

private:
    struct Node;
    using NodePtr = Node*;

    struct Node {
        T value;
        std::vector<NodePtr> forward;
        NodePtr prev;

        explicit Node(const T& val, int level);
    };


    double probability_;
    int max_level_;
    int current_max_level_;
    size_type size_;


    NodePtr head_;
    NodePtr tail_;


    Compare comp_;


    mutable std::mt19937 gen_;
    mutable std::uniform_real_distribution<double> dist_;


    int random_level() const;
    NodePtr find_node(const T& value) const;
    void link_forward(NodePtr node, int level, NodePtr next_node);
    void insert_node(NodePtr node, const std::vector<NodePtr>& predecessors);
};


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
    NodePtr node_;
    friend class SkipList;
};

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
    ConstIterator operator--(int);

    bool operator==(const ConstIterator& other) const noexcept;
    bool operator!=(const ConstIterator& other) const noexcept;

private:
    NodePtr node_;
    friend class SkipList;
};



template <typename T, typename Compare>
SkipList<T, Compare>::Node::Node(const T& val, int level)
    : value(val), forward(level + 1, nullptr), prev(nullptr) {}

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

template <typename T, typename Compare>
SkipList<T, Compare>::~SkipList() {
    clear();
    delete head_;
}

template <typename T, typename Compare>
SkipList<T, Compare>& SkipList<T, Compare>::operator=(const SkipList& other) {
    if (this != &other) {
        SkipList temp(other);
        std::swap(*this, temp);
    }
    return *this;
}

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

        // Сбрасываем состояние other
        other.head_ = new Node(T(), max_level_);
        other.current_max_level_ = 0;
        other.size_ = 0;
        other.tail_ = nullptr;
    }
    return *this;
}

template <typename T, typename Compare>
int SkipList<T, Compare>::random_level() const {
    int level = 0;
    while (dist_(gen_) < probability_ && level < max_level_) {
        ++level;
    }
    return level;
}

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

template <typename T, typename Compare>
void SkipList<T, Compare>::link_forward(NodePtr node, int level, NodePtr next_node) {
    node->forward[level] = next_node;
    if (next_node != nullptr) {
        next_node->prev = node;
    }
}

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

template <typename T, typename Compare>
std::pair<typename SkipList<T, Compare>::Iterator, bool> SkipList<T, Compare>::insert(const T& value) {
    std::vector<NodePtr> predecessors(max_level_ + 1, head_);
    NodePtr current = head_;

    for (int i = current_max_level_; i >= 0; --i) {
        while (current->forward[i] != nullptr && comp_(current->forward[i]->value, value)) {
            current = current->forward[i];
        }
        predecessors[i] = current;
    }
    current = current->forward[0];

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

template <typename T, typename Compare>
typename SkipList<T, Compare>::Iterator SkipList<T, Compare>::erase(Iterator pos) {
    if (pos == end()) {
        return end();
    }

    NodePtr node = pos.node_;
    Iterator next_it = pos;
    ++next_it;

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

    if (node->forward[0] != nullptr) {
        node->forward[0]->prev = node->prev;
    } else {
        tail_ = node->prev;
    }

    delete node;
    --size_;
    return next_it;
}

template <typename T, typename Compare>
typename SkipList<T, Compare>::size_type SkipList<T, Compare>::erase(const T& value) {
    auto it = find(value);
    if (it != end()) {
        erase(it);
        return 1;
    }
    return 0;
}

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

template <typename T, typename Compare>
typename SkipList<T, Compare>::Iterator SkipList<T, Compare>::find(const T& value) {
    return Iterator(find_node(value));
}

template <typename T, typename Compare>
typename SkipList<T, Compare>::ConstIterator SkipList<T, Compare>::find(const T& value) const {
    return ConstIterator(find_node(value));
}

template <typename T, typename Compare>
bool SkipList<T, Compare>::contains(const T& value) const {
    return find_node(value) != nullptr;
}

template <typename T, typename Compare>
bool SkipList<T, Compare>::empty() const noexcept {
    return size_ == 0;
}

template <typename T, typename Compare>
typename SkipList<T, Compare>::size_type SkipList<T, Compare>::size() const noexcept {
    return size_;
}

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

template <typename T, typename Compare>
bool SkipList<T, Compare>::operator!=(const SkipList& other) const {
    return !(*this == other);
}

template <typename T, typename Compare>
typename SkipList<T, Compare>::Iterator::reference SkipList<T, Compare>::Iterator::operator*() const {
    if (node_ == nullptr) {
        throw std::out_of_range("Dereferencing end iterator");
    }
    return node_->value;
}

template <typename T, typename Compare>
typename SkipList<T, Compare>::Iterator::pointer SkipList<T, Compare>::Iterator::operator->() const {
    if (node_ == nullptr) {
        throw std::out_of_range("Dereferencing end iterator");
    }
    return &node_->value;
}

template <typename T, typename Compare>
typename SkipList<T, Compare>::Iterator& SkipList<T, Compare>::Iterator::operator++() {
    if (node_ == nullptr) {
        throw std::out_of_range("Incrementing end iterator");
    }
    node_ = node_->forward[0];
    return *this;
}

template <typename T, typename Compare>
typename SkipList<T, Compare>::Iterator SkipList<T, Compare>::Iterator::operator++(int) {
    Iterator temp = *this;
    ++(*this);
    return temp;
}

template <typename T, typename Compare>
typename SkipList<T, Compare>::Iterator& SkipList<T, Compare>::Iterator::operator--() {
    if (node_ == nullptr) {
        throw std::out_of_range("Decrementing end iterator");
    }
    node_ = node_->prev;
    if (node_ != nullptr && node_->prev == nullptr) {
        node_ = nullptr;
    }
    return *this;
}

template <typename T, typename Compare>
typename SkipList<T, Compare>::Iterator SkipList<T, Compare>::Iterator::operator--(int) {
    Iterator temp = *this;
    --(*this);
    return temp;
}

template <typename T, typename Compare>
bool SkipList<T, Compare>::Iterator::operator==(const Iterator& other) const noexcept {
    return node_ == other.node_;
}

template <typename T, typename Compare>
bool SkipList<T, Compare>::Iterator::operator!=(const Iterator& other) const noexcept {
    return node_ != other.node_;
}

template <typename T, typename Compare>
typename SkipList<T, Compare>::ConstIterator::reference SkipList<T, Compare>::ConstIterator::operator*() const {
    if (node_ == nullptr) {
        throw std::out_of_range("Dereferencing end iterator");
    }
    return node_->value;
}

template <typename T, typename Compare>
typename SkipList<T, Compare>::ConstIterator::pointer SkipList<T, Compare>::ConstIterator::operator->() const {
    if (node_ == nullptr) {
        throw std::out_of_range("Dereferencing end iterator");
    }
    return &node_->value;
}

template <typename T, typename Compare>
typename SkipList<T, Compare>::ConstIterator& SkipList<T, Compare>::ConstIterator::operator++() {
    if (node_ == nullptr) {
        throw std::out_of_range("Incrementing end iterator");
    }
    node_ = node_->forward[0];
    return *this;
}

template <typename T, typename Compare>
typename SkipList<T, Compare>::ConstIterator SkipList<T, Compare>::ConstIterator::operator++(int) {
    ConstIterator temp = *this;
    ++(*this);
    return temp;
}

template <typename T, typename Compare>
typename SkipList<T, Compare>::ConstIterator& SkipList<T, Compare>::ConstIterator::operator--() {
    if (node_ == nullptr) {
        throw std::out_of_range("Decrementing end iterator");
    }
    node_ = node_->prev;
    if (node_ != nullptr && node_->prev == nullptr) {
        node_ = nullptr;
    }
    return *this;
}

template <typename T, typename Compare>
typename SkipList<T, Compare>::ConstIterator SkipList<T, Compare>::ConstIterator::operator--(int) {
    ConstIterator temp = *this;
    --(*this);
    return temp;
}

template <typename T, typename Compare>
bool SkipList<T, Compare>::ConstIterator::operator==(const ConstIterator& other) const noexcept {
    return node_ == other.node_;
}

template <typename T, typename Compare>
bool SkipList<T, Compare>::ConstIterator::operator!=(const ConstIterator& other) const noexcept {
    return node_ != other.node_;
}

template <typename T, typename Compare>
typename SkipList<T, Compare>::Iterator SkipList<T, Compare>::begin() noexcept {
    return Iterator(head_->forward[0]);
}

template <typename T, typename Compare>
typename SkipList<T, Compare>::Iterator SkipList<T, Compare>::end() noexcept {
    return Iterator(nullptr);
}

template <typename T, typename Compare>
typename SkipList<T, Compare>::ConstIterator SkipList<T, Compare>::begin() const noexcept {
    return ConstIterator(head_->forward[0]);
}

template <typename T, typename Compare>
typename SkipList<T, Compare>::ConstIterator SkipList<T, Compare>::end() const noexcept {
    return ConstIterator(nullptr);
}

template <typename T, typename Compare>
typename SkipList<T, Compare>::ConstIterator SkipList<T, Compare>::cbegin() const noexcept {
    return begin();
}

template <typename T, typename Compare>
typename SkipList<T, Compare>::ConstIterator SkipList<T, Compare>::cend() const noexcept {
    return end();
}

template <typename T, typename Compare>
typename SkipList<T, Compare>::reference SkipList<T, Compare>::at(size_type index) {
    if (index >= size_) {
        throw std::out_of_range("SkipList::at: index out of range");
    }
    NodePtr current = head_->forward[0];
    for (size_type i = 0; i < index; ++i) {
        current = current->forward[0];
    }
    return current->value;
}

template <typename T, typename Compare>
typename SkipList<T, Compare>::const_reference SkipList<T, Compare>::at(size_type index) const {
    if (index >= size_) {
        throw std::out_of_range("SkipList::at: index out of range");
    }
    NodePtr current = head_->forward[0];
    for (size_type i = 0; i < index; ++i) {
        current = current->forward[0];
    }
    return current->value;
}

template <typename T, typename Compare>
typename SkipList<T, Compare>::reference SkipList<T, Compare>::operator[](size_type index) {
    assert(index < size_ && "SkipList::operator[]: index out of range");
    NodePtr current = head_->forward[0];
    for (size_type i = 0; i < index; ++i) {
        current = current->forward[0];
    }
    return current->value;
}

template <typename T, typename Compare>
typename SkipList<T, Compare>::const_reference SkipList<T, Compare>::operator[](size_type index) const {
    assert(index < size_ && "SkipList::operator[]: index out of range");
    NodePtr current = head_->forward[0];
    for (size_type i = 0; i < index; ++i) {
        current = current->forward[0];
    }
    return current->value;
}


#endif //STRUCTURE_H
