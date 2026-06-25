#include <iostream>
  
#include <vector>

class Solution {
public:
  int largestAltitude(std::vector<int>& gain)
  {
    int ans = 0, curr = 0;
    for(int n: gain)
    {
      curr += n;
      ans = std::max(ans, curr);
    }
    return ans;
  }
};
