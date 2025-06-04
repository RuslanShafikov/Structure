#include "../header/structure.h"
#include <gtest/gtest.h>
#include <vector>
#include <algorithm>
#include <set>
#include <string>

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
TEST(SkipListStressTest, LargeDataset) {
    SkipList<int> list;
    std::set<int> reference;
    const int N = 10000;

    for (int i = 0; i < N; ++i) {
        int value = rand() % (N * 10);
        auto list_result = list.insert(value);
        auto ref_result = reference.insert(value);
        EXPECT_EQ(list_result.second, ref_result.second);
    }
    EXPECT_EQ(list.size(), reference.size());

    EXPECT_TRUE(std::equal(list.begin(), list.end(), reference.begin()));

    for (int i = 0; i < N/2; ++i) {
        int value = rand() % (N * 10);
        size_t list_erased = list.erase(value);
        size_t ref_erased = reference.erase(value);
        EXPECT_EQ(list_erased, ref_erased);
    }
    EXPECT_EQ(list.size(), reference.size());

    for (int value : reference) {
        EXPECT_TRUE(list.contains(value));
    }

    list.clear();
    reference.clear();
    EXPECT_TRUE(list.empty());
    EXPECT_TRUE(reference.empty());
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}