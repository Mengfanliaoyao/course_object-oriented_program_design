#include <iostream>

#define MOD (int)(1e9+7)

class Solution 
{
public:
  
  int concatenatedBinary(int n)
  {
    unsigned long long res = 0; // result
    char res_bits = 0; // record now result has how many digits in binary
    char cur_bits = 1; // record now iterated number 'i' has how many digits in binary
    for(int i = 1; i <= n; i++)
    {
      if(i==(1<<cur_bits)) // update digits of i when i == 2^(cur_bits)
        cur_bits++;
      
      if(res_bits + cur_bits > 63) // unsigned long long has 64 bit to operate, minize the module operate
      {
        res %= MOD;
        for(int j = 0; j <= 64; j++)
        {
          if(!(res>>j)) // reculculate the digits of resule
          {
            res_bits = j;
            break;
          }
        }
      }
#if DEBUG
      std::cout<<"res: "<<res<<" res_bits: "<<(int)res_bits<<" cur_bits: "<<(int)cur_bits<<'\n';
#endif
      // concatenation i after res
      res <<= cur_bits;
      res |= i;
      res_bits += cur_bits;
    }
    res %= MOD;
    
    return (int)res;
  }
};
