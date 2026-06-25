#include <iostream>

#include <vector>


  struct ListNode {
      int val;
      ListNode *next;
      ListNode() : val(0), next(nullptr) {}
      ListNode(int x) : val(x), next(nullptr) {}
      ListNode(int x, ListNode *next) : val(x), next(next) {}
  };
 
class Solution {
public:
  int pairSum(ListNode* head)
  {
    if(!head) return 0;
    if(!head->next) return head->val;
    
    std::vector<int> v;
    
    ListNode* it = head;
    while(it)
    {
      v.push_back(it->val);
      it = it->next;
    }
    int maxSum = 0;
    for(size_t i = 0, j = v.size()-1; i<=j; ++i, --j)
    {
      if(i==j)
        maxSum = std::max(maxSum, v[i]);
      else
        maxSum = std::max(maxSum, v[i]+v[j]);
    }
    return maxSum;
  }
};
