
#include <iostream>

#include <vector>

class Solution
{
public:
  bool isGood(std::vector<int>& nums)
  {
    int count = 0;
    std::vector<bool> base;
    base.assign(nums.size(), 0);
    for(int n : nums)
    {
      if (n < 1 || n > (int)(nums.size()-1))
        return false;
      if(!base[n-1])
      {
        base[n-1] = 1;
        count++;
      }
      else if((n-1)==(int)(nums.size()-2)&&!base[n])
      {
        base[n]=1;
        count++;
      }
      
    }
    return (count == (int)nums.size());
  }
};
