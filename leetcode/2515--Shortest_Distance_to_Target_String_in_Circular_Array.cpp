
#include <iostream>

#include <string>
#include <vector>

class Solution {
public:
  int closestTarget(std::vector<std::string>& words, std::string target, int startIndex)
  {
    for(size_t offset = 0; offset <= words.size()/2; offset++)
    {
      if(words[(startIndex+offset)%words.size()]==target || words[(startIndex+words.size()-offset)%words.size()]==target)
        return offset;
    }
    
    return -1;
  }
};
