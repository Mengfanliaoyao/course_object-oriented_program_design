#include <string>
#include <map>

using std::string, std::map;

class Solution
{
public:
  int numberOfSpecialChars(string word)
  {
    int ans = 0;
    map<char, int> m;
    for(size_t i = 0; i < word.size(); ++i)
    {
      if((isupper(word[i])&&m.find(word[i])==m.end())||islower(word[i]))
        m[word[i]] = i;
    }
    for(size_t i = 0; i < 26; i++)
    {
      if(m.find(i+'a')!=m.end()&&m.find(i+'A')!=m.end()&&m[i+'a']<m[i+'A'])
        ans++;
    }
    return ans;
  }
};
