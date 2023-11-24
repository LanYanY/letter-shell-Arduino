# 移植说明
当前移植版本v1.6

原工程使用C语言，该版本使用C++面向对象特性进行移植，理论上可以在所有支持Arduino IDE的设备上运行

# letter shell
一个体积极小的嵌入式shell

## 更新日志
### 2018/4/20   v1.0
- 第一版
### 2018/4/23   v1.1
- 加入对带参命令的支持
### 2018/4/25   v1.2
合并带参函数和不带参函数命令表
### 2018/7/10   v1.3
- 修复带参命令最后输入空格时传入参数数量出错的问题
- 修复不带参数调用带参命令时程序跑死的问题，指令处理相关代码优化
- 加入对于输入超出命令长度的情况的处理措施
### 2018/7/11   v1.4
- 新增对于方向键的处理，暂时未对方向键添加具体功能
- 修复单独的空格，制表符等无效操作被识别为指令进行解析，造成程序死机的问题
- 取消制表符作为参数分隔符的用途
### 2018/7/12   v1.5
- 新增历史命令的功能，使用上下方向键操作
- 新增tab键输入最近一条命令
- 无历史记录时，tab键输入help命令
- 新增一条用于清屏的默认指令
### 2018/7/18   v1.6
- 修改tab键功能，加入自动补全
- 无输入情况下，按下tab输入help命令
- 有输入情况下，进行自动补全
