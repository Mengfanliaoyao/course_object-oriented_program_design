#include <iostream>

#include <string>
#include <vector>

class Solution {
public:
  int maxNumberOfBalloons(std::string text)
  {
    int ans = 2147483647;
    int counts[5] = {0}; // b a l o n
    for(char c : text)
    {
      switch (c)
      {
        case 'b': ++counts[0]; break;
        case 'a': ++counts[1]; break;
        case 'l': ++counts[2]; break;
        case 'o': ++counts[3]; break;
        case 'n': ++counts[4]; break;
        default: break;
      }
    }
    for(int i = 0; i < 5; i++)
      ans=std::min(ans, (i==2||i==3)?(counts[i]/2):counts[i]);
    return ans;
  }
};
