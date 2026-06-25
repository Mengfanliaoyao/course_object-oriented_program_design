#include <iostream>

#include <vector>

class Solution
{
public:
  int maxDistance(std::vector<int>& colors)
  {
    int i = 0, j = colors.size()-1;
    while(i<j)
    {
      if(colors[i]!=colors[colors.size()-1])
        return colors.size()-1-i;
      if(colors[j]!=colors[0])
        return j;
      i++;
      j--;
    }
    return colors.size()-1-i;
  }
};
