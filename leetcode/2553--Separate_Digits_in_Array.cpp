
#include <iostream>

#include <vector>
#include <stack>

class Solution
{
public:
  std::vector<int> separateDigits(std::vector<int>& nums)
  {
    std::vector<int> res;
    std::stack<int> st;
    for(int n : nums)
    {
      while(n)
      {
        st.push(n%10);
        n/=10;
      }
      while(!st.empty())
      {
        res.push_back(st.top());
        st.pop();
      }
    }
    return res;
  }
};
