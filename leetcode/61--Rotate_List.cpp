#include <iostream>


struct ListNode {
    int val;
    ListNode *next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode *next) : val(x), next(next) {}
};

#define DEBUG 0

class Solution {
public:
  ListNode* rotateRight(ListNode* head, int k)
  {
    if(!head) return nullptr;
    
    ListNode* tail = head;
    int nodeCount = 0;
    while(tail->next!=nullptr)
    {
      tail=tail->next;
      ++nodeCount;
    }
    ++nodeCount;
    k%=nodeCount;
    
#if DEBUG
    std::cout << nodeCount << ' ' << k;;
#endif
    
    int rotate = nodeCount - k;
    while(rotate--)
    {
      tail->next = head;
      head = head->next;
      tail = tail->next;
      tail->next = nullptr;
    }
    
    return head;
  }
};
