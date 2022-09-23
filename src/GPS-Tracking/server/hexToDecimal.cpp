#include "GPS-Tracking/server/hexToDecimal.hpp"

#include <sstream>
#include <bitset>
#include <iostream>

namespace karlo {
  
  const char* hex_char_to_bin(char c)
  {
    // TODO handle default / error
    switch(toupper(c))
      {
      case '0': return "0000";
      case '1': return "0001";
      case '2': return "0010";
      case '3': return "0011";
      case '4': return "0100";
      case '5': return "0101";
      case '6': return "0110";
      case '7': return "0111";
      case '8': return "1000";
      case '9': return "1001";
      case 'A': return "1010";
      case 'B': return "1011";
      case 'C': return "1100";
      case 'D': return "1101";
      case 'E': return "1110";
      case 'F': return "1111";
      }
  }

  std::string hex_str_to_bin_str(const std::string& hex)
  {
    std::string bin;
    for(unsigned i = 0; i != hex.length(); ++i)
      bin += hex_char_to_bin(hex[i]);
    return bin;
  }

  std::string addBinary (std::string num1, std::string num2) {
    std::string result = "";
    int temp = 0;
    int size_a = num1.size() - 1;
    int size_b = num2.size() - 1;
    while (size_a >= 0 || size_b >= 0 || temp == 1){
      temp += ((size_a >= 0)? num1[size_a] - '0': 0);
      temp += ((size_b >= 0)? num2[size_b] - '0': 0);
      result = char(temp % 2 + '0') + result;
      temp /= 2;
      size_a--; size_b--;
    }
    return result;
  }

  int twoComplement (std::string binary) {
    std::string one = "";
    for(int i = 0; i < binary.size(); i++) {
      if(binary[i] == '1'){
        binary[i] = '0';
      } else {
        binary[i] = '1';
      }
      one = one + '0';
    }
    one[binary.size()-1] = '1';

    std::string num = addBinary(binary, one);
    int dec_value = 0;
 
    // Initializing base value to 1, i.e 2^0
    int base = 1;
 
    int len = num.length();
    for (int i = len - 1; i >= 0; i--) {
      if (num[i] == '1')
        dec_value += base;
      base = base * 2;
    }
    //std::cout << "dec_value : " << dec_value * -1 << std::endl;
 
    return dec_value * -1;
  }

  int hexToDecimal (std::string hex){
    return twoComplement(hex_str_to_bin_str(hex));
  }

} // namespace karlo
