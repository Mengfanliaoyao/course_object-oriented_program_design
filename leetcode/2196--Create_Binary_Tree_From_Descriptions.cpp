#include <iostream>

#include <vector>
#include <set>
#include <map>
#include <queue>

  struct TreeNode {
      int val;
      TreeNode *left;
      TreeNode *right;
      TreeNode() : val(0), left(nullptr), right(nullptr) {}
      TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
      TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
  };
 

class Solution
{
public:
  TreeNode* createBinaryTree(std::vector<std::vector<int>>& descriptions)
  {
    TreeNode* head = nullptr;
    std::map<int, TreeNode*> m; // <node val, node>
    std::map<int, bool> p; // find head
    std::queue<std::vector<int>> q; // to late build child links
    
    for(const auto& v : descriptions)
    {
      TreeNode* n = nullptr;
      
      if(m.find(v[0])!=m.end()) // parent already constructed
        n = m[v[0]];
      else
      {
        n = new TreeNode(v[0]);
        if(p.find(v[0])==p.end()) // only new node in p could be head
          p[v[0]] = true;
      }
      
      if(m.find(v[1])!=m.end()) // child node exist
      {
        if(v[2]) // left child
          n->left = m[v[1]];
        else
          n->right = m[v[1]];
      }
      else
      {
        TreeNode* c = new TreeNode(v[1]);// child haven't constructed
        m[v[1]] = c;
        q.push(v);
      }
      
      
      p[v[1]] = false; // child
      m[v[0]] = n;
    }
    while(!q.empty())
    {
      std::vector<int> v = q.front();
      q.pop();
      TreeNode* n = m[v[0]];
      
      if(v[2]) // left child
        n->left = m[v[1]];
      else
        n->right = m[v[1]];
    }
    for(auto& [val, isHead]: p)
    {
      if(isHead)
      {
        head = m[val];
        break;
      }
    }
    
    return head;
  }
  
};
