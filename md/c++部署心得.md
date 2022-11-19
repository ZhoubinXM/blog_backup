# C++ 部署心得

## 1. 语法篇

1. 使用vector， 用emplace_back而不用push_back，涉及到深拷贝。
2. 循环内不要创建复杂的数据类型，移至循环外。
3. 模型输入一致的情况下，输出要保持一致，不得有随机波动的现象。
4. C++ main函数中参数argc和argv含义及用法：

argc 是 argument count的缩写，表示传入main函数的参数个数；

argv 是 argument vector的缩写，表示传入main函数的参数序列或指针，并且第一个参数argv[0]一定是程序的名称，并且包含了程序所在的完整路径，所以确切的说需要我们输入的main函数的参数个数应该是argc-1个；

```c++
#include <iostream>

using namespace std;

void main(int argc,char *argv[])
{
	for(int i=0;i<argc;i++)
	{
		cout<<"argument["<<i<<"] is: "<<argv[i]<<endl;
	}
	system("pause");
}


```

5.  C++中的**mutable**关键字

   mutalbe的中文意思是“可变的，易变的”，跟constant（既C++中的const）是反义词。

   在C++中，mutable也是为了突破const的限制而设置的。被mutable修饰的变量，将永远处于可变的状态，即使在一个const函数中。

   我们知道，被const关键字修饰的函数的一个重要作用就是为了能够保护类中的成员变量。即：该函数可以使用类中的所有成员变量，但是不能修改他们的值。然而，在某些特殊情况下，我们还是需要在const函数中修改类的某些成员变量，因为要修改的成员变量与类本身并无多少关系，即使修改了也不会对类造成多少影响。当然，你可以说，你可以去掉该函数的const关键字呀！但问题是，我只想修改某个成员变量，其余成员变量仍然希望被const保护。

