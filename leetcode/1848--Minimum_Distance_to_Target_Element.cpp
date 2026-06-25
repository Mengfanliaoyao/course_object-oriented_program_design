
#include <iostream>

#include<vector>

class Solution
{
public:
  int getMinDistance(std::vector<int>& nums, int target, int start)
  {
    for(size_t offset = 0; offset <= nums.size()/2; offset++)
    {
      if(nums[(start+offset)%nums.size()]==target || nums[(start+nums.size()-offset)%nums.size()]==target)
        return offset;
    }
  }
};
