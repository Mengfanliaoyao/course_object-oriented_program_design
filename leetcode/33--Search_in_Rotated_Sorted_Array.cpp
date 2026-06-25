#include <iostream>

#include <vector>

class Solution
{
public:
  int search(std::vector<int>& nums, int target)
  {
    int min_pos = -1;
    
    bool rotated = (nums[0]>nums[nums.size()-1]);
    
    if(rotated) // first find breakpoint
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
      min_pos = r;
    }
//    std::cout << min_pos << '\n';
    int l = 0, r = nums.size()-1, m; // logical 0:size-1 -> min_pos:max_pos
    int real_m;
    while(l<r)
    {
      m = (r+l)/2;
      real_m = (rotated)?((m+min_pos)%nums.size()):m;
//      std::cout << "l: " << l << " m: " << m<< " r: " << r << " real_m: " << real_m << ' ' << nums[real_m] << '\n';
      if (nums[real_m]==target)
        return real_m;
      else if(nums[real_m]<target)
        l = m+1;
      else if (nums[real_m]>target)
        r = m-1;
    }
    m = (r+l)/2;
    real_m = (rotated)?((m+min_pos)%nums.size()):m;
    if (nums[real_m]==target)
      return real_m;
    
    return -1;
  }
};
