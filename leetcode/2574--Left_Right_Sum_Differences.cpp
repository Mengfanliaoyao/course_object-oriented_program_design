#include <iostream>

#include <vector>


class Solution {
public:
  std::vector<int> leftRightDifference(std::vector<int>& nums)
  {
    std::vector<int> res, left, right;
    res.assign(nums.size(), 0);
    left.assign(nums.size(), 0);
    right.assign(nums.size(), 0);
    
    left[0] = 0, right[nums.size()-1] = 0;
    
    for(size_t i = 1 ; i < nums.size(); ++i)
    {
      left[i] = left[i-1] + nums[i-1];
      right[nums.size()-1-i] = right[nums.size()-i] + nums[nums.size()-i];
      
      if(i>=(nums.size()-1-i))
      {
        res[i] = abs(left[i] - right[i]);
        res[nums.size()-1-i] = abs(left[nums.size()-1-i] - right[nums.size()-1-i]);
      }
      
    }
    res[0] = abs(left[0] - right[0]);
    res[nums.size()-1] = abs(left[nums.size()-1] - right[nums.size()-1]);
    
    return res;
  }
};
