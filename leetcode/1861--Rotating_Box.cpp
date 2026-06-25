#include <iostream>

#include <vector>
#include <stack>

#define DEBUG 1

class Solution
{
public:
  std::vector<std::vector<char>> rotateTheBox(std::vector<std::vector<char>>& boxGrid)
  {
    
#if DEBUG
    print_grid(boxGrid);
#endif
    std::vector<std::vector<char>> res;
    res.resize(boxGrid[0].size());
    
    for(int i = boxGrid.size()-1; i >= 0; --i)
    {
      int stone = 0;
      int stop = boxGrid[i].size()-1;
      std::stack<char> st;
      for(int j = boxGrid[i].size()-1; j >= 0; --j)
      {
        switch (boxGrid[i][j]) {
          case '#': stone++; break;
          case '*': 
            while(stop > j)
            {
              if(stone)
              {
                st.push('#');
                stone--;
              }
              else
                st.push('.');
              
              stop--;
            }
            st.push('*');
            stop--;
            break;
          default: break;
        }
      }
      while(stop >= 0)
      {
        if(stone)
        {
          st.push('#');
          stone--;
        }
        else
          st.push('.');
        
        stop--;
      }
      for(int j = 0; j < (int)boxGrid[i].size(); ++j)
      {
        res[j].emplace_back(st.top());
        st.pop();
      }
    }
#if DEBUG
    print_grid(res);
#endif
    
    return res;
  }
#if DEBUG
  void print_grid(const std::vector<std::vector<char>>& a) const
  {
    for(const auto& v : a)
    {
      for(char c : v)
        std::cout << c << ' ';
      std::cout << '\n';
    }
  }
#endif
};


int main()
{
  Solution s;
  
  std::vector<std::vector<char>> a = {{'#','.','*','.'}, {'#','#','*','.'}};
  
  std::vector<std::vector<char>> b = s.rotateTheBox(a);
  
#if 0
  s.print_grid(a);
  std::cout << '\n';
  s.print_grid(b);
#endif  
  return 0;
}
