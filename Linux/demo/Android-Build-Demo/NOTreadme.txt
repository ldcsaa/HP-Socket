# 这不是说明文件



先定义 PATH 路径，将交叉工具的bin目录设置进来
然后指定C++编译器调用make
类似于：
CXX=arm-linux-androideabi-g++ AR=arm-linux-androideabi-ar make
只有当生成静态库的时候才用 AR 选项
CXXFLAGS 最好参考 makefile 里面的设置，不然概不负责
其实就算你参考了 makefile 里面的设置出了问题我也不会负责的，可能是因为你没穿女装

## 遇到 BUG 穿上女装后再试试

### 看不懂 makefile ？怪我吗？

到底是游戏不好玩，还是小说不好看，要来学编程？