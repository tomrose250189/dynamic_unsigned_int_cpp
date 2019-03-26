#ifndef G_UINT_HPP_INCLUDED
#define G_UINT_HPP_INCLUDED

namespace tom_lib{
   class d_uint{
      static const int m_data_size = sizeof(unsigned long long);
      unsigned long long* m_data;
      int m_size;
      int ceil_div(int, int) const;
      d_uint(unsigned long long*, int);
   public:
      d_uint(unsigned long long val = 0);
      d_uint(const std::string&);
      d_uint(const d_uint&);
      d_uint(d_uint&&);
      d_uint& operator=(const d_uint&);
      d_uint& operator=(d_uint&&);
      d_uint& operator=(unsigned long long);
      ~d_uint();
      int get_size() const {return m_size;}
      int get_data_size() const {return m_data_size;}
      unsigned long long operator()(int) const;
      d_uint operator+(const d_uint&) const;
      d_uint& operator+=(const d_uint&);
      d_uint operator<<(const int) const;
      d_uint& operator<<=(const int);
      d_uint operator*(const d_uint&) const;
      d_uint& operator*=(const d_uint&);
      d_uint operator-(const d_uint&) const;
      d_uint& operator-=(const d_uint&);
      d_uint operator-() const;
      bool operator<(const d_uint&) const;
      bool operator==(const d_uint&) const;
   };
   d_uint operator+(unsigned long long, const d_uint&);
   d_uint operator*(unsigned long long, const d_uint&);
   d_uint operator-(unsigned long long, const d_uint&);
   bool operator<(unsigned long long, const d_uint&);
   bool operator==(unsigned long long, const d_uint&);
   std::ostream& operator<<(std::ostream&, const d_uint&);
}

#endif // G_UINT_HPP_INCLUDED
