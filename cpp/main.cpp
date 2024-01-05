/*!
    \file "main.cpp"

    Author: Matt Ervin <matt@impsoftware.org>
    Formatting: 4 spaces/tab (spaces only; no tabs), 120 columns.
    Doc-tool: Doxygen (http://www.doxygen.com/)

    https://leetcode.com/problems/binary-tree-level-order-traversal/
*/

//!\sa https://github.com/doctest/doctest/blob/master/doc/markdown/main.md
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <algorithm>
#include <cassert>
#include <chrono>
#include <deque>
#include <doctest/doctest.h> //!\sa https://github.com/doctest/doctest/blob/master/doc/markdown/tutorial.md
#include <iterator>
#include <memory>
#include <queue>
#include <vector>
#include <set>
#include <span>

using namespace std;

// [----------------(120 columns)---------------> Module Code Delimiter <---------------(120 columns)----------------]

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode() : val(0), left(nullptr), right(nullptr) {}
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
    virtual ~TreeNode() {
        if (left) {
            delete left;
        }
        if (right) {
            delete right;
        }
    }
};

/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode() : val(0), left(nullptr), right(nullptr) {}
 *     TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
 *     TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
 * };
 */
class Solution {
public:
    vector<vector<int>> levelOrder_countingLevelNodes(TreeNode* root) {
        vector<vector<int>> result{};
        
        size_t nodesInLevel = 1;

        std::deque<TreeNode*> queue{};
        queue.push_back(root);
        while (!queue.empty()) {
            vector<int> levelNodeValues{};

            // For each [possible] node in the level:
            for (auto nodeCount = nodesInLevel; nodeCount; --nodeCount) {
                auto node = queue.front();
                queue.pop_front();
                if (node) {
                    queue.push_back(node->left);
                    queue.push_back(node->right);
                    
                    levelNodeValues.push_back(node->val);
                } else {
                    queue.push_back(nullptr);
                    queue.push_back(nullptr);
                }
            }

            if (levelNodeValues.empty()) {
                break;
            }

            result.emplace_back(std::move(levelNodeValues));

            nodesInLevel *= 2;
        }
        
        return result;
    }

    vector<vector<int>> levelOrder(TreeNode* root) {
        vector<vector<int>> result{};

        if (root) {
            queue<TreeNode*> levelQueue{};
            levelQueue.push(root);

            while (!levelQueue.empty()) {
                vector<int> nodeValues{};

                // Currently queued items are all on the same level of the tree.
                for (auto levelNodeCount = levelQueue.size(); levelNodeCount; --levelNodeCount) {
                    auto node = levelQueue.front();
                    levelQueue.pop();

                    if (node->left) { levelQueue.push(node->left); }
                    if (node->right) { levelQueue.push(node->right); }
                    
                    nodeValues.push_back(node->val);
                }

                result.emplace_back(std::move(nodeValues));
            }
        }

        return result;
    }
};

// [----------------(120 columns)---------------> Module Code Delimiter <---------------(120 columns)----------------]

struct elapsed_time_t
{
    std::chrono::steady_clock::time_point start{};
    std::chrono::steady_clock::time_point end{};
    
    elapsed_time_t(
        std::chrono::steady_clock::time_point start
        , std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now()
    ) : start{std::move(start)}, end{std::move(end)} {}
};

std::ostream&
operator<<(std::ostream& os, elapsed_time_t const& et)
{
    auto const elapsed_time = et.end - et.start;
    os << std::chrono::duration_cast<std::chrono::microseconds>(elapsed_time).count()
       << '.' << (std::chrono::duration_cast<std::chrono::microseconds>(elapsed_time).count() % 1000)
       << " ms";
    return os;
}

TreeNode* createBst(std::vector<int> values) {
    TreeNode* root = nullptr;

    for (auto value : values) {
        // Search for parent node, if any.
        TreeNode* parent{};
        for (TreeNode* iter = root; iter; ) {
            parent = iter;
            iter = value < iter->val ? iter->left : iter->right;
        }
        
        // Find parent pointer.
        TreeNode** parentPtr{};
        if (parent) {
            if (value < parent->val) {
                parentPtr = &parent->left;
            } else {
                parentPtr = &parent->right;
            }
        } else {
            parentPtr = &root;
        }
        
        // Allocate new node.
        if (parentPtr) {
            *parentPtr = new TreeNode{std::move(value)};

            if (!root) {
                root = *parentPtr;
            }
        }
    }

    return root;
}

std::vector<int> inorder(TreeNode const* root) {
    std::vector<int> result{};

    if (root) {
        auto const leftResult = inorder(root->left);
        std::copy(leftResult.begin(), leftResult.end(), std::back_inserter(result));

        result.push_back(root->val);

        auto const rightResult = inorder(root->right);
        std::copy(rightResult.begin(), rightResult.end(), std::back_inserter(result));
    }

    return result;
}

std::vector<int> preorder(TreeNode const* root) {
    std::vector<int> result{};

    if (root) {
        result.push_back(root->val);

        auto const leftResult = preorder(root->left);
        std::copy(leftResult.begin(), leftResult.end(), std::back_inserter(result));

        auto const rightResult = preorder(root->right);
        std::copy(rightResult.begin(), rightResult.end(), std::back_inserter(result));
    }

    return result;
}

constexpr auto const null = (std::numeric_limits<int>::min)();

/*!
    \brief Convert array of node values in level order to a tree of nodes.

    The array of node values is formatted like those used for leetcode problems.
    E.g. [3,9,20,null,null,15,7,null,null,null,null,30,40]
         describes:
                       03
               09              20
           --      --      15      07
         --  --  --  --  30  40  --  --
         Note that the '--'s at the end of the bottom row are excluded from the array.
*/
std::unique_ptr<TreeNode> createLevelOrderBT(std::vector<int> const& levelOrderNodes) {
    std::unique_ptr<TreeNode> result{};
    
    std::deque<TreeNode*> treeNodes{};
    size_t levelWidth = 1; // Node count in current level.
    size_t levelOrderNodesIdx = 0;
    while (levelOrderNodes.size() > levelOrderNodesIdx) {
        // Process all [child] nodes in current tree level.
        TreeNode* parent = nullptr;
        for (auto childIdx = 0
             ; levelWidth > childIdx
               && levelOrderNodes.size() > levelOrderNodesIdx
             ; ++childIdx
        ) {
            auto const firstChild = 0 == childIdx % 2;
            
            // Determine parent node for new child nodes.
            parent = !firstChild
                     ? parent // Continue using same parent.
                     : treeNodes.empty()
                       ? nullptr
                       : [&]{ auto r = treeNodes.front(); treeNodes.pop_front(); return r; }();
            
            auto const nodeValue = levelOrderNodes[levelOrderNodesIdx++];
            // Create new child node.
            treeNodes.push_back(null == nodeValue ? nullptr : new TreeNode{nodeValue});
            if (null != nodeValue) {
                if (parent) { (firstChild ? parent->left : parent->right) = treeNodes.back(); }
                if (!result) { result.reset(treeNodes.back()); }
            }
        }

        levelWidth *= 2;
    }

    return result;
}

/*!
    \brief Convert a tree of nodes to array of node values in level order.

    E.g. [3,9,20,null,null,15,7,null,null,null,null,30,40]
         describes:
                       03
               09              20
           --      --      15      07
         --  --  --  --  30  40  --  --
         Note that the '--'s at the end of the bottom row are excluded from the array.
*/
std::vector<int> btToLevelOrder(TreeNode* root) {
    std::vector<int> result{};

    std::queue<TreeNode*> levelNodes{};
    levelNodes.push(root);
    auto lastValidResultSize = result.size();
    while (!levelNodes.empty()) {
        bool atLeastOneValidNodeEnqueued = false;
        for (auto levelNodeCount = levelNodes.size(); levelNodeCount; --levelNodeCount) {
            auto node = levelNodes.front();
            levelNodes.pop();

            if (node) {
                levelNodes.push(node->left);
                levelNodes.push(node->right);
                result.push_back(node->val);
                lastValidResultSize = result.size();
                atLeastOneValidNodeEnqueued = true;
            } else {
                levelNodes.push(nullptr);
                levelNodes.push(nullptr);
                result.push_back(null);
            }
        }
        if (!atLeastOneValidNodeEnqueued) {
            result.erase(result.begin() + lastValidResultSize, result.end());
            break;
        }
    }

    return result;
}

TEST_CASE("Case 1")
{
    cerr << "Case 1" << '\n';
    std::vector<int> const levelOrderRep{3,9,20,null,null,15,7};
    auto const expected = std::vector<std::vector<int>>{
        std::vector<int>{3}
        ,std::vector<int>{9,20}
        ,std::vector<int>{15,7}
    };
    auto tree = createLevelOrderBT(levelOrderRep);
    { // New scope.
        auto const start = std::chrono::steady_clock::now();
        auto const result = Solution{}.levelOrder(tree.get());
        CHECK(result == expected);
        cerr << "Elapsed time: " << elapsed_time_t{start} << '\n';
    }
    cerr << '\n';
}

TEST_CASE("Case 10")
{
    cerr << "Case 10" << '\n';
    std::vector<int> const levelOrderRep{3,9,20,null,null,15,7,null,null,null,null,30,40};
    auto const expected = std::vector<std::vector<int>>{
        std::vector<int>{3}
        ,std::vector<int>{9,20}
        ,std::vector<int>{15,7}
        ,std::vector<int>{30,40}
    };
    auto tree = createLevelOrderBT(levelOrderRep);
    { // New scope.
        auto const start = std::chrono::steady_clock::now();
        auto const result = Solution{}.levelOrder(tree.get());
        CHECK(result == expected);
        cerr << "Elapsed time: " << elapsed_time_t{start} << '\n';
    }
    cerr << '\n';
}

/*
    End of "main.cpp"
*/
