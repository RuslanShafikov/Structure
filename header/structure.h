//
// Created by ruslan on 5/21/25.
//

#ifndef STRUCTURE_H
#define STRUCTURE_H


#include <vector>
#include <cstdlib>
#include <random>
#include <stdexcept>
#include <memory>
#include <functional>

template <typename Key, typename Compare = std::less<Key>, typename Allocator = std::allocator<Key>>
class skip_set {
public:
    using key_type = Key;
    using value_type = Key;
    using key_compare = Compare;
    using allocator_type = Allocator;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = typename std::allocator_traits<Allocator>::pointer;
    using const_pointer = typename std::allocator_traits<Allocator>::const_pointer;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

private:
    struct Node {
        value_type value;
        std::vector<Node*> forward;

        Node(int level, const value_type& val) : value(val), forward(level + 1, nullptr) {}
        Node(int level) : value(), forward(level + 1, nullptr) {}
    };

    using node_allocator = typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;
    node_allocator alloc_;

    int max_level_;
    float probability_;
    key_compare comp_;
    Node* head_;
    int current_max_level_;
    size_type size_;

    std::random_device rd_;
    std::mt19937 gen_;
    std::uniform_real_distribution<> dist_;

public:
    class iterator;
    class const_iterator;

    explicit skip_set(const Compare& comp = Compare(), const Allocator& alloc = Allocator(),
        int max_level = 12, float prob = 0.5)
        : alloc_(alloc), comp_(comp), max_level_(max_level), probability_(prob),
        current_max_level_(0), size_(0), gen_(rd_()), dist_(0.0, 1.0) {
        head_ = create_node(max_level_);
    }

    skip_set(const skip_set&) = delete;
    skip_set& operator=(const skip_set&) = delete;

    ~skip_set() {
        clear();
        destroy_node(head_);
    }

    void clear() {
        Node* node = head_->forward[0];
        while (node != nullptr) {
            Node* next = node->forward[0];
            destroy_node(node);
            node = next;
        }
        for (int i = 0; i <= max_level_; ++i) {
            head_->forward[i] = nullptr;
        }
        current_max_level_ = 0;
        size_ = 0;
    }

    size_type size() const noexcept { return size_; }
    bool empty() const noexcept { return size_ == 0; }

    std::pair<iterator, bool> insert(const value_type& value) {
        std::vector<Node*> update(max_level_ + 1, nullptr);
        Node* current = head_;

        for (int i = current_max_level_; i >= 0; --i) {
            while (current->forward[i] != nullptr && comp_(current->forward[i]->value, value)) {
                current = current->forward[i];
            }
            update[i] = current;
        }

        current = current->forward[0];
        if (current != nullptr && !comp_(value, current->value) && !comp_(current->value, value)) {
            return std::make_pair(iterator(current), false);
        }

        int new_level = random_level();
        if (new_level > current_max_level_) {
            for (int i = current_max_level_ + 1; i <= new_level; ++i) {
                update[i] = head_;
            }
            current_max_level_ = new_level;
        }

        Node* new_node = create_node(new_level, value);
        for (int i = 0; i <= new_level; ++i) {
            new_node->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = new_node;
        }

        ++size_;
        return std::make_pair(iterator(new_node), true);
    }

    size_type erase(const key_type& key) {
        std::vector<Node*> update(max_level_ + 1, nullptr);
        Node* current = head_;

        for (int i = current_max_level_; i >= 0; --i) {
            while (current->forward[i] != nullptr && comp_(current->forward[i]->value, key)) {
                current = current->forward[i];
            }
            update[i] = current;
        }

        current = current->forward[0];
        if (current == nullptr || comp_(key, current->value) || comp_(current->value, key)) {
            return 0;
        }

        for (int i = 0; i < static_cast<int>(current->forward.size()); ++i) {
            update[i]->forward[i] = current->forward[i];
        }

        while (current_max_level_ > 0 && head_->forward[current_max_level_] == nullptr) {
            --current_max_level_;
        }

        destroy_node(current);
        --size_;
        return 1;
    }

    iterator find(const key_type& key) {
        Node* current = head_;
        for (int i = current_max_level_; i >= 0; --i) {
            while (current->forward[i] != nullptr && comp_(current->forward[i]->value, key)) {
                current = current->forward[i];
            }
        }
        current = current->forward[0];
        if (current != nullptr && !comp_(key, current->value) && !comp_(current->value, key)) {
            return iterator(current);
        }
        return end();
    }

    const_iterator find(const key_type& key) const {
        Node* current = head_;
        for (int i = current_max_level_; i >= 0; --i) {
            while (current->forward[i] != nullptr && comp_(current->forward[i]->value, key)) {
                current = current->forward[i];
            }
        }
        current = current->forward[0];
        if (current != nullptr && !comp_(key, current->value) && !comp_(current->value, key)) {
            return const_iterator(current);
        }
        return end();
    }

    iterator begin() noexcept { return iterator(head_->forward[0]); }
    const_iterator begin() const noexcept { return const_iterator(head_->forward[0]); }
    const_iterator cbegin() const noexcept { return const_iterator(head_->forward[0]); }

    iterator end() noexcept { return iterator(nullptr); }
    const_iterator end() const noexcept { return const_iterator(nullptr); }
    const_iterator cend() const noexcept { return const_iterator(nullptr); }

private:
    int random_level() {
        int level = 0;
        while (dist_(gen_) < probability_ && level < max_level_) {
            ++level;
        }
        return level;
    }

    Node* create_node(int level, const value_type& value = value_type()) {
        Node* node = std::allocator_traits<node_allocator>::allocate(alloc_, 1);
        try {
            std::allocator_traits<node_allocator>::construct(alloc_, node, level, value);
        }
        catch (...) {
            std::allocator_traits<node_allocator>::deallocate(alloc_, node, 1);
            throw;
        }
        return node;
    }

    void destroy_node(Node* node) {
        std::allocator_traits<node_allocator>::destroy(alloc_, node);
        std::allocator_traits<node_allocator>::deallocate(alloc_, node, 1);
    }
};

template <typename Key, typename Compare, typename Allocator>
class skip_set<Key, Compare, Allocator>::iterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = typename skip_set::value_type;
    using difference_type = typename skip_set::difference_type;
    using pointer = typename skip_set::pointer;
    using reference = typename skip_set::reference;

    iterator() : current_(nullptr) {}
    explicit iterator(Node* node) : current_(node) {}

    reference operator*() const {
        if (current_ == nullptr) {
            throw std::out_of_range("Dereferencing end iterator");
        }
        return current_->value;
    }

    pointer operator->() const {
        if (current_ == nullptr) {
            throw std::out_of_range("Dereferencing end iterator");
        }
        return &(current_->value);
    }

    iterator& operator++() {
        if (current_ != nullptr) {
            current_ = current_->forward[0];
        }
        return *this;
    }

    iterator operator++(int) {
        iterator temp = *this;
        ++(*this);
        return temp;
    }

    bool operator==(const iterator& other) const { return current_ == other.current_; }
    bool operator!=(const iterator& other) const { return current_ != other.current_; }

private:
    Node* current_;
    friend class const_iterator;
};

template <typename Key, typename Compare, typename Allocator>
class skip_set<Key, Compare, Allocator>::const_iterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = typename skip_set::value_type;
    using difference_type = typename skip_set::difference_type;
    using pointer = typename skip_set::const_pointer;
    using reference = typename skip_set::const_reference;

    const_iterator() : current_(nullptr) {}
    explicit const_iterator(Node* node) : current_(node) {}
    const_iterator(const iterator& it) : current_(it.current_) {}

    reference operator*() const {
        if (current_ == nullptr) {
            throw std::out_of_range("Dereferencing end iterator");
        }
        return current_->value;
    }

    pointer operator->() const {
        if (current_ == nullptr) {
            throw std::out_of_range("Dereferencing end iterator");
        }
        return &(current_->value);
    }

    const_iterator& operator++() {
        if (current_ != nullptr) {
            current_ = current_->forward[0];
        }
        return *this;
    }

    const_iterator operator++(int) {
        const_iterator temp = *this;
        ++(*this);
        return temp;
    }

    bool operator==(const const_iterator& other) const { return current_ == other.current_; }
    bool operator!=(const const_iterator& other) const { return current_ != other.current_; }

private:
    const Node* current_;
};



#endif //STRUCTURE_H
