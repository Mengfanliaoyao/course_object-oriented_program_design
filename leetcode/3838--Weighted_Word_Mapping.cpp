#include <iostream>

#include <string>
#include <vector>
#include <map>

class Solution {
public:
  std::string mapWordWeights(std::vector<std::string>& words, std::vector<int>& weights)
  {
    std::string res;
    
    for(const std::string& s : words)
    {
      int weight = 0;
      for(char c:s)
      {
        weight+=weights[c-'a'];
      }
      weight%=26;
      res+=('z'-weight);
    }
    
    return res;
  }
};
