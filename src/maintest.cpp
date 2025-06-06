#include "../header/structure.h"
#include <gtest/gtest.h>
#include <vector>
#include <algorithm>
#include <set>
#include <string>
#include <concepts>

TEST(SkipListTest, DefaultConstructor) {
    SkipList<int> list;
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0);
    EXPECT_EQ(list.begin(), list.end());
}

TEST(SkipListTest, InsertAndSize) {
    SkipList<int> list;
    auto [it1, inserted1] = list.insert(5);
    EXPECT_TRUE(inserted1);
    EXPECT_EQ(*it1, 5);
    EXPECT_EQ(list.size(), 1);
    EXPECT_FALSE(list.empty());

    auto [it2, inserted2] = list.insert(10);
    EXPECT_TRUE(inserted2);
    EXPECT_EQ(list.size(), 2);

    auto [it3, inserted3] = list.insert(5);
    EXPECT_FALSE(inserted3);
    EXPECT_EQ(list.size(), 2);
}

TEST(SkipListTest, FindAndContains) {
    SkipList<std::string> list;
    list.insert("apple");
    list.insert("banana");

    auto it = list.find("apple");
    EXPECT_NE(it, list.end());
    EXPECT_EQ(*it, "apple");

    it = list.find("orange");
    EXPECT_EQ(it, list.end());

    EXPECT_TRUE(list.contains("banana"));
    EXPECT_FALSE(list.contains("grape"));
}

TEST(SkipListTest, Erase) {
    SkipList<int> list;
    list.insert(5);
    list.insert(10);
    list.insert(15);

    EXPECT_EQ(list.erase(10), 1);
    EXPECT_EQ(list.size(), 2);
    EXPECT_FALSE(list.contains(10));

    EXPECT_EQ(list.erase(20), 0);

    auto it = list.find(5);
    it = list.erase(it);
    EXPECT_EQ(*it, 15);
    EXPECT_EQ(list.size(), 1);
}

TEST(SkipListTest, ForwardIteration) {
    SkipList<int> list;
    std::vector<int> values = {5, 3, 7, 1, 4, 9, 2};
    for (int v : values) {
        list.insert(v);
    }

    std::sort(values.begin(), values.end());
    std::vector<int> list_values;
    for (auto it = list.begin(); it != list.end(); ++it) {
        list_values.push_back(*it);
    }
    EXPECT_EQ(list_values, values);
}

TEST(SkipListTest, ReverseIteration) {
    SkipList<int> list;
    list.insert(1);
    list.insert(2);
    list.insert(3);

    auto it = list.end();
    --it;
    EXPECT_EQ(*it, 3);
    --it;
    EXPECT_EQ(*it, 2);
    --it;
    EXPECT_EQ(*it, 1);
    EXPECT_EQ(it, list.begin());

    EXPECT_THROW(--it, std::out_of_range);
}

TEST(SkipListTest, CopyConstructor) {
    SkipList<int> list1;
    list1.insert(10);
    list1.insert(20);
    list1.insert(30);

    SkipList<int> list2(list1);
    EXPECT_EQ(list1.size(), list2.size());
    EXPECT_TRUE(std::equal(list1.begin(), list1.end(), list2.begin()));

    list2.insert(40);
    EXPECT_NE(list1.size(), list2.size());
}

TEST(SkipListTest, MoveOperations) {
    SkipList<int> list1;
    list1.insert(100);
    list1.insert(200);

    SkipList<int> list2(std::move(list1));
    EXPECT_TRUE(list1.empty());
    EXPECT_EQ(list2.size(), 2);
    EXPECT_TRUE(list2.contains(100));
    EXPECT_TRUE(list2.contains(200));

    SkipList<int> list3;
    list3 = std::move(list2);
    EXPECT_TRUE(list2.empty());
    EXPECT_EQ(list3.size(), 2);
}

TEST(SkipListTest, Clear) {
    SkipList<int> list;
    for (int i = 0; i < 100; ++i) {
        list.insert(i);
    }
    EXPECT_EQ(list.size(), 100);

    list.clear();
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.begin(), list.end());
}

TEST(SkipListTest, ComparisonOperators) {
    SkipList<int> list1;
    list1.insert(1);
    list1.insert(2);
    list1.insert(3);

    SkipList<int> list2;
    list2.insert(1);
    list2.insert(2);
    list2.insert(3);

    EXPECT_TRUE(list1 == list2);

    list2.insert(4);
    EXPECT_TRUE(list1 != list2);

    SkipList<int> list3;
    EXPECT_FALSE(list1 == list3);
}

TEST(SkipListTest, CustomComparator) {
    struct CaseInsensitiveCompare {
        bool operator()(const std::string& a, const std::string& b) const {
            return std::lexicographical_compare(
                a.begin(), a.end(), b.begin(), b.end(),
                [](char c1, char c2) {
                    return std::tolower(c1) < std::tolower(c2);
                });
        }
    };

    SkipList<std::string, CaseInsensitiveCompare> list;
    list.insert("Apple");
    list.insert("banana");
    list.insert("cherry");

    EXPECT_TRUE(list.contains("APPLE"));
    EXPECT_TRUE(list.contains("BANANA"));
    EXPECT_TRUE(list.find("CHERRY") != list.end());

    auto it = list.find("apple");
    EXPECT_EQ(*it, "Apple");
}

TEST(SkipListTest, ErrorHandling) {
    SkipList<int> list;
    list.insert(10);

    auto it = list.end();
    EXPECT_THROW(*it, std::out_of_range);
    EXPECT_THROW(it.operator->(), std::out_of_range);

    EXPECT_THROW(++it, std::out_of_range);

    auto begin_it = list.begin();
    EXPECT_THROW(--begin_it, std::out_of_range);
}

TEST(SkipListTest, SubscriptOperatorAccess) {
    SkipList<int> list;
    list.insert(10);
    list.insert(20);
    list.insert(30);

    EXPECT_EQ(list[0], 10);
    EXPECT_EQ(list[1], 20);
    EXPECT_EQ(list[2], 30);

    list[1] = 25;
    EXPECT_EQ(list[1], 25);
}

TEST(SkipListTest, ConstSubscriptOperator) {
    SkipList<int> mutableList;
    mutableList.insert(10);
    mutableList.insert(20);
    mutableList.insert(30);

    const auto& list = mutableList;

    EXPECT_EQ(list[0], 10);
    EXPECT_EQ(list[1], 20);
    EXPECT_EQ(list[2], 30);
}

TEST(SkipListTest, AtMethodAccess) {
    SkipList<int> list;
    list.insert(10);
    list.insert(20);
    list.insert(30);

    // Проверка доступа к элементам
    EXPECT_EQ(list.at(0), 10);
    EXPECT_EQ(list.at(1), 20);
    EXPECT_EQ(list.at(2), 30);

    // Проверка модификации элементов
    list.at(1) = 25;
    EXPECT_EQ(list.at(1), 25);
}

TEST(SkipListTest, ConstAtMethod) {
    SkipList<int> mutableList;
    mutableList.insert(10);
    mutableList.insert(20);
    mutableList.insert(30);

    const auto& list = mutableList;

    EXPECT_EQ(list.at(0), 10);
    EXPECT_EQ(list.at(1), 20);
    EXPECT_EQ(list.at(2), 30);
}

TEST(SkipListTest, OutOfRangeAccess) {
    SkipList<int> list;
    list.insert(10);
    list.insert(20);

    EXPECT_THROW(list.at(2), std::out_of_range);
    EXPECT_THROW(list.at(-1), std::out_of_range);
    EXPECT_THROW(list.at(100), std::out_of_range);
}

TEST(SkipListTest, EmptyListAccess) {
    SkipList<int> list;

    EXPECT_THROW(list.at(0), std::out_of_range);
    EXPECT_THROW(list.at(1), std::out_of_range);
}

TEST(SkipListTest, SequentialAccessConsistency) {
    SkipList<int> list;
    const int N = 100;

    for (int i = 0; i < N; i++) {
        list.insert(i);
    }

    for (int i = 0; i < N; i++) {
        EXPECT_EQ(list[i], i);
        EXPECT_EQ(list.at(i), i);
    }
}

TEST(SkipListTest, ReverseOrderInsertion) {
    SkipList<int> list;
    const int N = 100;

    for (int i = N-1; i >= 0; i--) {
        list.insert(i);
    }

    for (int i = 0; i < N; i++) {
        EXPECT_EQ(list[i], i);
    }
}

TEST(SkipListTest, AccessAfterModification) {
    SkipList<int> list;
    list.insert(1);
    list.insert(2);
    list.insert(3);

    list[0] = 10;
    list[1] = 20;
    list[2] = 30;

    EXPECT_EQ(list[0], 10);
    EXPECT_EQ(list[1], 20);
    EXPECT_EQ(list[2], 30);

    list.at(0) = 100;
    list.at(1) = 200;
    list.at(2) = 300;

    EXPECT_EQ(list.at(0), 100);
    EXPECT_EQ(list.at(1), 200);
    EXPECT_EQ(list.at(2), 300);
}

TEST(SkipListTest, StringTypeAccess) {
    SkipList<std::string> list;
    list.insert("apple");
    list.insert("banana");
    list.insert("cherry");

    EXPECT_EQ(list[0], "apple");
    EXPECT_EQ(list[1], "banana");
    EXPECT_EQ(list[2], "cherry");

    list[1] = "orange";
    EXPECT_EQ(list.at(1), "orange");
}

TEST(SkipListTest, LargeDatasetAccess) {
    SkipList<int> list;
    const int N = 10000;

    for (int i = 0; i < N; i++) {
        list.insert(i * 2);
    }

    for (int i = 0; i < 100; i++) {
        int index = rand() % N;
        EXPECT_EQ(list[index], index * 2);
    }
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}