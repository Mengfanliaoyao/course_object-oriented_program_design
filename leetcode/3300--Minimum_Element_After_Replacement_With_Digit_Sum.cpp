
#include <iostream>

#include <vector>

class Solution 
{
public:
  int minElement(std::vector<int>& nums)
  {
    int min = 2147483647;
    for(int n : nums)
    {
      int sum = 0;
      while(n)
      {
        sum += (n%10);
        n/=10;
      }
      min = std::min(min, sum);
    }
    
    return min;
  }
};
