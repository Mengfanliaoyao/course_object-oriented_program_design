
#include <iostream>

#include <string>

class Solution {
public:
  int furthestDistanceFromOrigin(std::string moves)
  {
    int l=0,r=0,magic=0;
    for(char c : moves)
    {
      switch(c)
      {
        case 'L':l++;break;
        case 'R':r++;break;
        case '_':magic++;break;
      }
    }
    return abs(l-r)+magic;
  }
};
