#include <string>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cstring>
#include "d_uint.hpp"

namespace tom_lib{
   int d_uint::ceil_div(int x, int y) const{
      return((x + y - 1)/y);
   }
   d_uint::d_uint(unsigned long long* p, int sz) : m_data(p), m_size(sz) {
   }
   d_uint::d_uint(unsigned long long val): m_size(1) {
      m_data = new unsigned long long[1];
      *m_data = val;
   }
   d_uint::d_uint(const std::string& s) : m_size(ceil_div(s.size(), 2*m_data_size)) {
      m_data = new unsigned long long[m_size];
      for(int i = 0; i < m_size-1; ++i){
         *(m_data + i) = std::stoull(std::string(s.end()-(i+1)*2*m_data_size, s.end()-i*2*m_data_size), nullptr, 16);
      }
      *(m_data + m_size - 1) = std::stoull(std::string(s.begin(), s.end()-(m_size-1)*2*m_data_size), nullptr, 16);
   }
   d_uint::d_uint(const d_uint& a) : m_size(a.m_size){
      m_data = new unsigned long long[m_size];
      for(int i = 0; i < m_size; ++i){
         m_data[i] = a.m_data[i];
      }
   }
   d_uint::d_uint(d_uint&& a) : m_size(a.m_size){
      m_data = a.m_data;
      a.m_data = nullptr;
   }
   d_uint& d_uint::operator=(const d_uint& a){
      delete[](m_data);
      m_size = a.m_size;
      m_data = new unsigned long long[m_size];
      for(int i = 0; i < m_size; ++i){
         m_data[i] = a.m_data[i];
      }
      return(*this);
   }
   d_uint& d_uint::operator=(d_uint&& a){
      delete[](m_data);
      m_size = a.m_size;
      m_data = a.m_data;
      a.m_data = nullptr;
      return(*this);
   }
   d_uint& d_uint::operator=(unsigned long long a){
      if(m_size == 1){
         *m_data = a;
      }
      else{
         delete[](m_data);
         m_data = new unsigned long long[1];
         *m_data = a;
         m_size = 1;
      }
      return(*this);
   }
   d_uint::~d_uint(){
      delete[](m_data);
   }
   unsigned long long d_uint::operator()(int i) const{
      return(*(m_data + i));
   }
   d_uint d_uint::operator+(const d_uint& a) const{
      int max_size = std::max(m_size, a.m_size);
      int min_size = m_size + a.m_size - max_size;
      char ovf = 0;
      unsigned long long* new_data = new unsigned long long[max_size];
      for(int i = 0; i < min_size; ++i){
         unsigned long long x = (*this)(i);
         unsigned long long y = a(i);
         *(new_data + i) = x + y + ovf;
         if(ovf){
            ovf = !(x < ~y);
         }
         else{
            ovf = (x > ~y);
         }
      }
      const d_uint* max_size_ptr = (m_size >= a.m_size) ? this : &a;
      for(int i = min_size; i < max_size; i++){
         unsigned long long x = (*max_size_ptr)(i);
         *(new_data + i) = x + ovf;
         if(ovf){
            ovf = !(x < ~0ull);
         }
      }
      if(ovf){
         unsigned long long* tmp = new unsigned long long[max_size+1];
         std::memcpy(tmp, new_data, max_size*sizeof(unsigned long long));
         delete[](new_data);
         new_data = tmp;
         new_data[max_size] = 1;
         ++max_size;
      }
      return(d_uint(new_data, max_size));
   }
   d_uint& d_uint::operator+=(const d_uint& a){
      *this = *this + a;
      return(*this);
   }
   d_uint d_uint::operator<<(const int s) const {
      if (m_data[m_size-1] == 0){
         return d_uint();
      }
      int n = s/(8*m_data_size);
      int r = s % (8*m_data_size);
      int lop = 8*m_data_size-1;
      unsigned long long mask = (1ull << lop);
      while(!(mask & m_data[m_size-1])){
         mask >>= 1;
         --lop;
      }
      int m_size_n = m_size + n;
      if(r + lop >= 8*m_data_size){
         ++m_size_n;
      }
      unsigned long long *m_data_n = new unsigned long long[m_size_n];
      const unsigned long long bmask = (((1ull << (8*m_data_size - r - 1)) - 1ull) << 1) + 1ull;
      const unsigned long long tmask = ~bmask;
      for(int i = 0; i < m_size_n; ++i){
         if(i < n){
            m_data_n[i] = 0;
         }
         else if(i == n){
            m_data_n[i] = (m_data[0] & bmask) << r;
         }
         else if(i-n < m_size){
            m_data_n[i] = ((m_data[i-n] & bmask) << r) | ((m_data[i-1-n] & tmask) >> (8*m_data_size - r));
         }
         else{
            m_data_n[i] = ((m_data[m_size-1] & tmask) >> (8*m_data_size - r));
         }
      }
      return d_uint(m_data_n, m_size_n);
   }
   d_uint& d_uint::operator<<=(const int s){
      *this = (*this) << s;
      return *this;
   }
   d_uint d_uint::operator*(const d_uint& a) const{
      const unsigned long long b_mask = ((1ull << (4*m_data_size)) - 1ull);
      const unsigned long long t_mask = b_mask << (4*m_data_size);
      d_uint result;
      for(int i = 0; i < m_size; ++i){
         for(int j = 0; j < a.m_size; ++j){
            unsigned long long bt = m_data[i] & b_mask;
            unsigned long long tt = (m_data[i] & t_mask) >> (4*m_data_size);
            unsigned long long ba = a.m_data[j] & b_mask;
            unsigned long long ta = (a.m_data[j] & t_mask) >> (4*m_data_size);
            d_uint t = tt*ta;
            t <<= (8*m_data_size);
            d_uint m = bt*ta;
            m <<= (4*m_data_size);
            t += m;
            m = tt*ba;
            m <<= (4*m_data_size);
            t += m;
            t += bt*ba;
            result += (t << (8*m_data_size*(i + j)));
         }
      }
      return result;
   }
   d_uint& d_uint::operator*=(const d_uint& a){
      *this = (*this) * a;
      return *this;
   }
   d_uint d_uint::operator-(const d_uint& a) const{
      //implicitly assume m_size >= a.m_size
      char ovf = 1;
      unsigned long long* new_data = new unsigned long long[m_size];
      for(int i = 0; i < m_size; ++i){
         unsigned long long x = (*this)(i);
         unsigned long long y = (i < a.m_size) ? ~(a(i)) : ~(0ull);
         *(new_data + i) = x + y + ovf;
         if(ovf){
            ovf = !(x < ~y);
         }
         else{
            ovf = (x > ~y);
         }
      }
      int i = m_size - 1;
      while((new_data[i] == 0) && (i > 0)){
         --i;
      }
      int new_new_size = i + 1;
      unsigned long long* new_new_data = new unsigned long long[i];
      for(; i >= 0; --i){
         new_new_data[i] = new_data[i];
      }
      delete[] new_data;
      return(d_uint(new_new_data, new_new_size));
   }
   d_uint& d_uint::operator-=(const d_uint& a){
      *this = (*this) - a;
      return *this;
   }
   d_uint d_uint::operator-() const{
      return 0 - (*this);
   }
   bool d_uint::operator<(const d_uint& a) const{
      if(m_size > a.m_size){
         return false;
      }
      else if(m_size < a.m_size){
         return true;
      }
      for(int i = 0; i < m_size; ++i){
         if((*this)(i) > a(i)){
            return false;
         }
         else if((*this)(i) < a(i)){
            return true;
         }
      }
      return false;
   }
   bool d_uint::operator==(const d_uint& a) const{
      if(m_size != a.m_size){
         return false;
      }
      for(int i = 0; i < m_size; ++i){
         if((*this)(i) != a(i)){
            return false;
         }
      }
      return true;
   }
   d_uint operator+(unsigned long long x, const d_uint& y){
      return y + x;
   }
   d_uint operator*(unsigned long long x, const d_uint& y){
      return y*x;
   }
   d_uint operator-(unsigned long long x, const d_uint& y){
      return d_uint(x) - y;
   }
   bool operator<(unsigned long long x, const d_uint& y){
      return d_uint(x) < y;
   }
   bool operator==(unsigned long long x, const d_uint& y){
      return y == x;
   }
   std::ostream& operator<<(std::ostream& os, const d_uint& a){
      using std::operator<<;
      os << std::hex << a(a.get_size()-1);
      for(int i = a.get_size()-2; i >= 0 ; --i){
         unsigned long long tmp = a(i);
         os << std::hex << std::setfill('0') << std::setw(2*a.get_data_size()) << tmp;
      }
      return(os);
   }
}
