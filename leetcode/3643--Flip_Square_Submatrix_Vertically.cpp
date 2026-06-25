#include <iostream>

#include <cstring>

#include <array>
#include <vector>



#if DEBUG
class Solution {
public:
  std::vector<std::vector<int>> reverseSubmatrix(std::vector<std::vector<int>>&& grid, int x, int y, int k) {
    
    int buff[2];
    for(int i = x, j = x + k - 1; i < j; i++, j--)
    {
      memmove(buff, grid[i].data() + y, k * sizeof(int));
      memmove(grid[i].data() + y, grid[j].data() + y, k * sizeof(int));
      memmove(grid[j].data() + y, buff, k * sizeof(int));
    }
    
  return grid;
  }
};
 
int main(void)
{
  Solution s;
  s.reverseSubmatrix({{1,2,3,4},{5,6,7,8},{9,10,11,12},{13,14,15,16}}, 1,0,3);
  s.reverseSubmatrix({{3,4,2,3},{2,3,4,2}}, 0,2,2);
}

#else
class Solution {
public:
  std::vector<std::vector<int>> reverseSubmatrix(std::vector<std::vector<int>>& grid, int x, int y, int k) {
    
    std::vector<int> buff;
    buff.reserve(k);
    for(int i = x, j = x + k - 1; i < j; i++, j--) // from the top of submatrix wanna reverse, swap the content with the bottom till reach center
    {
      memmove(buff.data(), grid[i].data() + y, k * sizeof(int));
      memmove(grid[i].data() + y, grid[j].data() + y, k * sizeof(int));
      memmove(grid[j].data() + y, buff.data(), k * sizeof(int));
    }
    
    return grid;
  }
};

#endif

