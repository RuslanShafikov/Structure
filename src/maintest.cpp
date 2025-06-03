#include <gtest/gtest.h>
#include "structure.h"
#include <set>
#include <vector>
#include <algorithm>

using namespace testing;

class SkipSetTest : public Test {
protected:
    skip_set<int> ss;

    void SetUp() override {
        ss = skip_set<int>(std::less<int>(), std::allocator<int>(), 12, 0.5);
    }
};

TEST_F(SkipSetTest, StartsEmpty) {
    EXPECT_TRUE(ss.empty());
    EXPECT_EQ(ss.size(), 0);
}

TEST_F(SkipSetTest, InsertIncreasesSize) {
    auto [it, inserted] = ss.insert(42);
    EXPECT_TRUE(inserted);
    EXPECT_EQ(ss.size(), 1);
    EXPECT_FALSE(ss.empty());
}

TEST_F(SkipSetTest, InsertDuplicatesFails) {
    ss.insert(10);
    auto [it, inserted] = ss.insert(10);
    EXPECT_FALSE(inserted);
    EXPECT_EQ(ss.size(), 1);
}

TEST_F(SkipSetTest, FindExistingElement) {
    ss.insert(5);
    auto it = ss.find(5);
    EXPECT_NE(it, ss.end());
    EXPECT_EQ(*it, 5);
}

TEST_F(SkipSetTest, FindMissingElement) {
    ss.insert(1);
    EXPECT_EQ(ss.find(2), ss.end());
}

TEST_F(SkipSetTest, EraseExistingElement) {
    ss.insert(3);
    EXPECT_EQ(ss.erase(3), 1);
    EXPECT_EQ(ss.size(), 0);
    EXPECT_EQ(ss.find(3), ss.end());
}

TEST_F(SkipSetTest, EraseMissingElement) {
    ss.insert(4);
    EXPECT_EQ(ss.erase(5), 0);
    EXPECT_EQ(ss.size(), 1);
}

TEST_F(SkipSetTest, IteratorTraversal) {
    std::vector<int> vals = {3, 1, 4, 2};
    for (int v : vals) ss.insert(v);

    std::sort(vals.begin(), vals.end());
    std::vector<int> result;
    for (auto it = ss.begin(); it != ss.end(); ++it) {
        result.push_back(*it);
    }
    EXPECT_EQ(result, vals);
}

TEST_F(SkipSetTest, ConstIterator) {
    ss.insert(7);
    const auto& css = ss;
    auto it = css.find(7);
    EXPECT_NE(it, css.end());
    EXPECT_EQ(*it, 7);
}

TEST_F(SkipSetTest, ClearResets) {
    ss.insert(10);
    ss.insert(20);
    ss.clear();

    EXPECT_TRUE(ss.empty());
    EXPECT_EQ(ss.size(), 0);
    EXPECT_EQ(ss.begin(), ss.end());
}

TEST_F(SkipSetTest, InsertAscendingOrder) {
    for (int i = 0; i < 100; ++i) ss.insert(i);

    int count = 0;
    int last = -1;
    for (int v : ss) {
        EXPECT_GT(v, last);
        last = v;
        count++;
    }
    EXPECT_EQ(count, 100);
}

TEST_F(SkipSetTest, EraseMaintainsOrder) {
    std::set<int> ref = {1, 2, 3, 4, 5};
    for (int v : ref) ss.insert(v);

    ss.erase(3);
    ref.erase(3);

    std::vector<int> svec(ss.begin(), ss.end());
    std::vector<int> rvec(ref.begin(), ref.end());
    EXPECT_EQ(svec, rvec);
}

TEST_F(SkipSetTest, EndIteratorDereferenceThrows) {
    auto it = ss.end();
    EXPECT_THROW(*it, std::out_of_range);
    EXPECT_THROW(it.operator->(), std::out_of_range);
}
