#include <iostream>

#include <vector>


class Solution {
public:
  bool findRotation(std::vector<std::vector<int>>& mat, std::vector<std::vector<int>>& target)
  {
    for(int dir = 0; dir < Direction::SYMMETRIC + 1; dir++)
    {
      if(validateMat(mat,target,(Direction)dir))
        return true;
    }
    return false;
  }
private:
  enum Direction
  {
    DEFAULT = 0, CLOCKWISE, COUNTERCLOCKWISE, SYMMETRIC
  };
  bool validateMat(std::vector<std::vector<int>>& mat_a, std::vector<std::vector<int>>& mat_b, Direction dir)
  {
    size_t row = mat_a.size(), col = mat_a[0].size();
    
    // size validation (for non square matrix extension)
    
    if((dir == Direction::DEFAULT || dir == Direction::SYMMETRIC) && (row != mat_b.size() || col != mat_b[0].size()))
      return false;
    else if((dir == Direction::CLOCKWISE || dir == Direction::COUNTERCLOCKWISE) && (row != mat_b[0].size() || col != mat_b.size()))
      return false;
    
    for(size_t i = 0; i < row; i++)
    {
      for(size_t j = 0; j < col; j++)
      {
        switch (dir)
        {
        case Direction::DEFAULT:
          if(mat_a[i][j] != mat_b[i][j])
            return false;
          break;
        case Direction::CLOCKWISE:
          if(mat_a[i][j] != mat_b[col-1 - j][i])
            return false;
          break;
        case Direction::SYMMETRIC:
          if(mat_a[i][j] != mat_b[row-1 - i][col-1 - j])
            return false;
          break;
        case Direction::COUNTERCLOCKWISE:
          if(mat_a[i][j] != mat_b[j][row-1 - i])
            return false;
          break;
        default:
          return false;
        }
        
      }
    }
    
    return true;
  }
};
/*
0 0  0 1  0 2   2 0  1 0  0 0
1 0  1 1  1 2   2 1  1 1  0 1
2 0  2 1  2 2   2 2  1 2  0 2

0 2  1 2  2 2   2 2  2 1  2 0
0 1  1 1  2 1   1 2  1 1  1 0
0 0  1 0  2 0   0 2  0 1  0 0
*/
