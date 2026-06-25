
#include <iostream>

#include <cmath>

class Solution {
public:
  int mirrorDistance(int n)
  {
    
    int a[10] = {0};
    int i=0;
    while(n)
    {
      a[i++] = n%10;
      n/=10;
    }
    i--;
    int diff = 0;
    
    for(int j = i; j >= 0; j--)
    {
      diff += (pow(10, j)*(a[j]-a[i-j]));
    }
    return abs(diff);
  }
};
