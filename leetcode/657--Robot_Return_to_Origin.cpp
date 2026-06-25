
#include <iostream>

#include<string>

class Solution
{
public:
  bool judgeCircle(std::string moves)
  {
    int l=0,r=0,u=0,d=0;
    for(char c : moves)
    {
      switch(c)
      {
        case 'L':l++;break;
        case 'R':r++;break;
        case 'U':u++;break;
        case 'D':d++;break;
      }
    }
    return (l==r)&&(u==d);
  }
};
