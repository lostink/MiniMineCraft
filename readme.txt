说明：
项目构建于QT 4.8框架下，需要OpenGL支持。程序入口为MyGL.cpp。内存管理优化中。

项目操作：
wsad可以操作前进后退，按C关闭白天黑夜循环，按F打开无碰撞飞行模式。
在飞行模式下按QE可以操纵人物上下移动。
鼠标左键可以删除范围内当前选择方块，右键可以放置当前所持有的方块。
有声效。
ESC退出项目

项目分工：
CPP文件对应的.h文件为同一个人维护，未标明文件为框架代码

独立文件：
胡雅伦：
src\proceduralterrain.cpp 柏林函数生成地形、地貌以及柏林函数生成山洞
src\audiomanager.cpp 声音管理

刘家豪：
src\scene\screencenter.cpp 屏幕中间的小十字架 
src\scene\inventory.cpp    简单的装备栏
src\character.cpp          包括LSystem，玩家的控制，碰撞检测，放置/删除方块

胡雨昕：
scene\block.cpp
scene\chunk.cpp
scene\chunkmanager.cpp
glsl\lambert.frag.glsl
glsl\lambert.vert.glsl

共同维护文件：
src\mygl.cpp
