<div align="center">
    <img src="https://raw.githubusercontent.com/guodongxiaren/logo/master/tardis/tardis.png" width="10%">
    <h1>Tardis<br><sub>C++本地词典文件解析库</sub></h1>
    <a href="./LICENSE"><img src="https://img.shields.io/badge/Open_source-MIT-green.svg?logo=git&logoColor=green"></a>
</div>



## 编译运行
```
make
```
## 使用方法
### 1. 词表proto和key
在proto目录下，编写proto文件，每个字段对应词表文件一列
```
syntax="proto3";
import "tardis.proto";

message Student {
    int32 id = 1[(tardis.key) = true];
    string name = 2;
    int32 score = 3;
}
```
主键（key）需要使用commdict.key指定，支持多个字段做联合主键，比如：
```
message UserBid {
    int32 userid = 1[(tardis.key) = true];
    string bidword = 2[(tardis.key) = true];
}

```
### 2. 自定义类型
字段支持自定义类型（如下Address，表示地址），但需要自定义parse函数（parseAddress）。

```
message Address {
    string city = 1;
    string street = 2;
    string building = 3;
};

message Student {
    int32 id = 1[(tardis.key) = true];
    string name = 2;
    int32 score = 3;
    Address addr = 4;
}

```
自定义类型需要在commdict/factory.cpp文件中增加parse函数。
另外定义完之后需要使用`REGISTER_PARSER`宏来注册。注册之后才能被工厂类找到。
```c++
int parse_address(const string& line, Address* addr) {
    try {
        vector<string> vs = split(line, "|");

        if (vs.size() < 3) {
            return -1;
        }

        addr->set_city(vs[0]);
        addr->set_street(vs[1]);
        addr->set_building(vs[2]);
        return 0;
    } catch (...) {
        return -1;
    }
}

Factory::Register register_address("Address", parse_address);
```
如上表示|分隔的字段表示地址，在词表中
```
北京|西北旺|唐家岭新城

```
### 3.数组表示和定义
数组使用proto的repeated即可。增加一列hobby，表示数组
```
message Student {
    int32 id = 1[(tardis.key) = true];
    string name = 2;
    int32 score = 3;
    Address addr = 4;
    string hobby = 5;
}

```
在词表中：
```
3:电影,动漫,运动
```
完整的词表示例(TAB分隔)
```
1	王维	100	北京|西北旺|唐家岭新城	电影,动漫
2	李磊	99	深圳|前海路|星海名城	做饭,唱歌
3	李明	69	南昌|学府路|绿园	唱歌,运动,演讲
```
### 4. 代码调用参考
```
#include <iostream>
#include <string>
#include "tardis/dict.h"
#include "student.pb.h"

using std::string;
using std::cout;
using std::endl;

int main() {
    string dict_name = "../data/student.dict";
    auto cd = CommonDict<Student>::get_instance(dict_name);
    auto student = cd->find(1); // find函数
    cout << student->name() << endl;
    cout << student->hobby(0) << endl;
    cout << student->addr().city() << endl;

}

```
find函数的参数就是主键的值，支持基本数据类型，如果主键是自定义类型，则使用它的字符串表示(string类型)。
如果是联合主键，find也支持可变参数。比如：
```c++
    dict_name = "/../data/userbid.dict";
    auto cub = CommonDict<UserBid>::get_instance(dict_name);
    auto ub = cub->find(101, "租房"); // find函数
    cout << ub->bidword() << endl;
```
## 其他
注意ut在可执行文件所在的目录里执行
