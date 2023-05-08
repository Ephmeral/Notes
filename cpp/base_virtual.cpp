#include <iostream>
using namespace std;

class A{
  public:
    A ():m_iVal(0){test();}
    virtual void func() { std::cout<<m_iVal<<' ';}
    void test(){func();}
  public:
      int m_iVal;
};
class B : public A{
  public:
    B(){test();}
    virtual void func(){
      ++m_iVal;
      std::cout << m_iVal << ' ';
      }
};
int main(int argc ,char* argv[]){
  A*p = new B;
  p->test();
  return 0;
}
