#include <iostream>


  struct ListNode {
      int val;
      ListNode *next;
      ListNode() : val(0), next(nullptr) {}
      ListNode(int x) : val(x), next(nullptr) {}
      ListNode(int x, ListNode *next) : val(x), next(next) {}
  };
 
class Solution {
public:
  ListNode* deleteMiddle(ListNode* head)
  {
    if (!head || !head->next) return nullptr;
    
    int totalCount = 1, midCount = 1;
    ListNode* mid = head, *midPrev = nullptr;
    ListNode* curr = head;
    while(curr!=nullptr)
    {
#if DEBUG
      std::cout << " v: " << curr->val << '\n';
      std::cout << "totalCount: " << totalCount << " midCount: " << midCount << '\n';
#endif
      if(totalCount/2 == midCount)
      {
        midPrev = mid;
        mid = mid->next;
        ++midCount;
      }
      ++totalCount;
      curr = curr->next;
    }
    
    if(midPrev)
      midPrev->next = mid->next;
    
    delete mid;
    mid = nullptr;    
    
    return head;
  }
};
