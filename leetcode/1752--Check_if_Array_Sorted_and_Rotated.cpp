#include <iostream>

#include <vector>

using std::vector;

class Solution
{
public:
  bool check(vector<int>& nums)
  {
    bool sorted = true, rotated = false;
    int last = nums[0];
    for(size_t i = 1; i < nums.size() && sorted; i++)
    {
      if(nums[i] < last)
      {
        if(!rotated)
          rotated = true;
        else
          sorted = false;
          
      }
        
      last = nums[i];
    }
    if(rotated && nums[0]< nums[nums.size()-1])
      sorted = false;
    
    return sorted;
  }
};
