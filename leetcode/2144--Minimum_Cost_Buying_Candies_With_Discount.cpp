#include <iostream>

#include <vector>
#include <algorithm>

class Solution {
public:
  int minimumCost(std::vector<int>& cost)
  {
    int ans = 0;
    std::sort(cost.rbegin(), cost.rend());
    
    for(int i = 0; i < (int)cost.size(); i++)
    {
#if DEBUG
      std::cout << cost[i] << ' ';
#endif
      if((i+1)%3)
        ans+=cost[i];
    }
#if 0
    if(cost.size()%3)
      ans+=cost[cost.size()-1];
#endif
    return ans;
  }
};
