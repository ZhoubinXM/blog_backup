@[TOC](C++ 中的Lambda 函数)

**Lambda 函数**（或简称为“lambda”）是一个**匿名（未命名）函数**，它在源代码中使用简洁的语法就地定义。Lambda 函数在 C++11 中引入，现已成为广泛使用的功能，尤其是与标准库函数结合使用。

# 1. Synatax 语法

C++ Lambda函数的基本句法是：

```c++
[capture-list](parameters) -> return_type {
  // function body
};
```

- Capture-list: 捕获列表，用于指定哪些外部变量可以在Lambda函数中使用。
- parameters：和常规函数一样的输入参数的列表。
- return_type：Lambda函数返回值的类型，可以省略，编译器在很多情况下可以推断出。
- function_body：定义Lambda函数操作的代码。



# 2. 使用示例

以下是几个示例来演示 C++ 中 lambda 函数的使用：

- 没有捕获列表、参数或返回类型的 Lambda 函数。

```c++
auto printHello = []() {
    std::cout << "Hello, World!" << std::endl;
};
printHello(); // Output: Hello, World!
```

- 带参数的 Lambda 函数。

```c++
auto add = [](int a, int b) {
    return a + b;
};
int result = add(3, 4); // result = 7
```

- 具有按值捕获功能的 Lambda 函数，会调用拷贝构造函数。

```
int multiplier = 3;
auto times = [multiplier](int a) {
    return a * multiplier;
};
int result = times(5); // result = 15
```

- 具有引用捕获功能的 Lambda 函数，使用引用。

```
int expiresInDays = 45;
auto updateDays = [&expiresInDays](int newDays) {
    expiresInDays = newDays;
};
updateDays(30); // expiresInDays = 30
```

>  Notice：当使用引用捕获时，对lambda 函数*内捕获的变量所做的任何更改都会影响其在周围范围内的值。*