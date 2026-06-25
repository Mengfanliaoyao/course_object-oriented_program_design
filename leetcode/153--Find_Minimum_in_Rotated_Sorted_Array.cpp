
#include <iostream>

#include <vector>

class Solution
{
public:
  int findMin(std::vector<int>& nums)
  {
    if((nums[0]>nums[nums.size()-1])) // find breakpoint
    {
      int l = 0, r = nums.size()-1, m = (r-l)/2;
      while(l<r)
      {
        m = (r+l)/2;
        if (m==l || m==r)
          break;
//        std::cout << "l: " << l << " m: " << m<< " r: " << r << '\n';
        if(nums[l]>nums[m])
          r = m;
        else if (nums[m]>nums[r])
          l = m;
        
      }
      return nums[r];
    }
    return nums[0];
  }
};
