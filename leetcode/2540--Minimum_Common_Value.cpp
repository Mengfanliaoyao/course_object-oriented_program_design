#include <iostream>

#include <vector>


class Solution
{
public:
  int getCommon(std::vector<int>& nums1, std::vector<int>& nums2)
  {
    size_t i = 0, j = 0;
    bool find = false;
    while(i < nums1.size() && j < nums2.size())
    {
      if(nums1[i] == nums2[j])
      {
        find = true;
        break;
      }
      else if(nums1[i] > nums2[j])
        j++;
      else
        i++;
    }
    return find?nums1[i]:-1;
  }
};
