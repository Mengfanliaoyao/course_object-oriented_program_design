#include <iostream>

#include <vector>


#define DEBUG 1

class Solution {
public:
  int maxProductPath(std::vector<std::vector<int>>& grid)
  {
    size_t row = grid.size(), col = grid[0].size();
    
    std::vector<std::pair<long long, long long>> v; // store the max (absolute) product value of each element of grid
    
    v.reserve(col);
    
    for(size_t i = 0; i < col; i++) // first row
    {
      // since each single grid element only has value [-4,4], use 5's factor to filt out invalid value
      if(!grid[0][i])
        v[i] = {0, 0};
      else
      {
        if(grid[0][i] > 0)
        {
          v[i].first = (i)?((v[i-1].first>=0)?(v[i-1].first * grid[0][i]):-5):grid[0][i];
          v[i].second = (i)?((v[i-1].second<=0)?(v[i-1].second * grid[0][i]):5):grid[0][i];
        }
        else
        {
          v[i].second = (i)?((v[i-1].first>=0)?(v[i-1].first * grid[0][i]):5):grid[0][i];
          v[i].first = (i)?((v[i-1].second<=0)?(v[i-1].second * grid[0][i]):-5):grid[0][i];
        } 
      }
      #if DEBUG
      std::cout << v[i].first << ',' << v[i].second << ' ';
#endif
    }
    for(size_t i = 1; i < row; i++)
    {
      for(size_t j = 0; j < col; j++)
      {
#if 0
        std::cout << v[j].first << ',' << v[j].second << ' ';
#endif
        if(!grid[i][j])
        {
          v[j].first = 0;
          v[j].second = 0;
          continue;
        }
        
        long long preMax = -5, preMin = 5;
        preMax = (j)?std::max(v[j-1].first, v[j].first):v[j].first; // for current element, only considering its left and up result
        preMin = (j)?std::min(v[j-1].second, v[j].second):v[j].second;
        if (grid[i][j] > 0)
        {
          v[j].first = (preMax>=0)?(preMax * grid[i][j]):-5;
          v[j].second = (preMin<=0)?(preMin * grid[i][j]):5;
        }
        else
        {
          v[j].first = (preMin<=0)?(preMin * grid[i][j]):-5;
          v[j].second = (preMax>=0)?(preMax * grid[i][j]):5;
        }
        #if DEBUG
        std::cout << v[j].first << ',' << v[j].second << ' ';
#endif
      }
#if DEBUG
      std::cout << '\n';
#endif
    }
    return (v[col-1].first==-5)?-1:(v[col-1].first)%1000000007;    
  }
};
