#include <iostream>

#include <vector>


class Solution {
public:
  void rotate(std::vector<std::vector<int>>& matrix)
  {
    size_t matSize = matrix.size() - 1, submatSize = matrix.size() - 1;
    
    for(size_t i = 0; i <= matSize / 2; i++, submatSize--) // avoid swap repeatly, each iteration decrease the limit of inner iter, only considering submatrix
    {
      for(size_t j = i; j < submatSize; j++)
      {
        // clockwise swap
        int temp = matrix[i][j];
        matrix[i][j] = matrix[matSize - j][i];
        matrix[matSize - j][i] = matrix[matSize - i][matSize - j];
        matrix[matSize - i][matSize - j] = matrix[j][matSize - i];
        matrix[j][matSize - i] = temp;
      }
    }
    
  }
};
