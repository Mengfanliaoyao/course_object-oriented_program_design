
#include <string>

using std::string;

class Solution
{
public:
  int numberOfSpecialChars(string word)
  {
    int ans = 0;
    bool lower[26]={0};
    bool upper[26]={0};
    for(char c : word)
    {
      if(islower(c)&&!lower[c-'a'])
      {
        if(upper[c-'a'])
          ans++;
        lower[c-'a'] = 1;
      }
      else if (isupper(c)&&!upper[c-'A'])
      {
        if(lower[c-'A'])
          ans++;
        upper[c-'A'] = 1;
      }
    }
    return ans;
  }
};
