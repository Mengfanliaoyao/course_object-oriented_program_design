#include <iostream>

#include <vector>

#define DEBUG 1

class Solution
{
public:
  int maxPathScore(std::vector<std::vector<int>>& grid, int k)
  {
    int maxCost = std::min(k, (int)(grid.size()+grid[0].size()-2));
    std::vector<std::vector<std::vector<int>>> dp;
    
    // init
    dp.resize(grid.size());
    for(size_t i = 0; i < grid.size(); ++i)
    {
      dp[i].resize(grid[0].size());
      for(size_t j = 0; j < grid[0].size(); ++j)
      {
        dp[i][j].assign(maxCost+1, -1);
      }
    }
    dp[0][0][0] = 0;
    
    // dp select right or down with cost dim, ij: current cord, c: cost
    // dp[i][j][c] = best score to ij
    for(int i = 0; i < (int)grid.size(); ++i)
    {
      for(int j = 0; j < (int)grid[i].size(); ++j)
      {
        for(int c = 0; c <= maxCost; ++c)
        {
          if(grid[i][j]&&!(c>0)) continue;
          
          // from left
          if(j>0)
          {
            switch (grid[i][j])
            {
            case 1:
              if(dp[i][j-1][c-1]!=-1 && dp[i][j-1][c-1] + 1 > dp[i][j][c])
                dp[i][j][c] = dp[i][j-1][c-1] + 1;
              break;
            case 2:
              if(dp[i][j-1][c-1]!=-1 && dp[i][j-1][c-1] + 2 > dp[i][j][c])
                dp[i][j][c] = dp[i][j-1][c-1] + 2;
              break;
            default:
              if(dp[i][j-1][c]!=-1 && dp[i][j-1][c] > dp[i][j][c])
                dp[i][j][c] = dp[i][j-1][c];
              break;
            }
          }
          
          // from up
          if(i>0)
          {
            switch (grid[i][j])
            {
            case 1:
              if(dp[i-1][j][c-1]!=-1 && dp[i-1][j][c-1] + 1 > dp[i][j][c])
                dp[i][j][c] = dp[i-1][j][c-1] + 1;
              break;
            case 2:
              if(dp[i-1][j][c-1]!=-1 && dp[i-1][j][c-1] + 2 > dp[i][j][c])
                dp[i][j][c] = dp[i-1][j][c-1] + 2;
              break;
            default:
              if(dp[i-1][j][c]!=-1 && dp[i-1][j][c] > dp[i][j][c])
                dp[i][j][c] = dp[i-1][j][c];
              break;
            }
          }
        }
      }
    }
#if DEBUG
    for(int i = 0; i < (int)grid.size(); ++i)
    {
      for(int j = 0; j < (int)grid[0].size(); ++j)
      {
        std::cout << i << ' ' << j << ": ";
        for(int c : dp[i][j])
          std::cout << c << ' ';
        std::cout << '\n';
      }
    }
#endif
    int ans = -1;
    for(int c = maxCost; c >= 0; --c)
    {
      ans = std::max(ans, dp[grid.size()-1][grid[0].size()-1][c]);
    }
    return ans;
  }
};

int main(void)
{
  Solution s;
  
  std::vector<std::vector<int>> grid = {{0,2},{0,2},{1,0},{1,2},{1,0},{2,2},{0,0}}; // {{0,0,2},{0,1,0},{2,0,2}};//{{0,1},{2,0}}; //{{0,2,2},{1,1,1},{0,0,2}};
  int k = 4;
  
  s.maxPathScore(grid, k);
  
  return 0;
}
