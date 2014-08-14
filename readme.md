# 超核Kinetis 开发包(固件库)

包含Freescale Kinetis K 系列及 KL 系列底层驱动及实例代码

----------

## 如何开始

请按步骤逐步学习本固件库及Kinetis,先看免费入门视频教程.该教材包含了你想要的全部内容，从整体知识介绍到工具软件下载到入门提高。

1. [免费入门视频](http://www.moore8.com/courses/87)
2. [API 参考文档](http://git.oschina.net/yandld/CH-K-Lib/wikis/home)  
3. www.beyondcore.net
4. [固件库源代码包下载](http://git.oschina.net/yandld/CH-K-Lib/repository/archive?ref=master)

## 所支持的IDE
* Keil(MDK) V4.53 以上版本
* IAR 6.5 以上版本

## 适用型号
* MK60DNxxx
* MK60DNZxxx
* MK10DN32xxx
* MKL25Zxxx
* MKL46Zxxx

## 文件说明
**Libraries:**

* devices 一些外设芯片支持
* drivers 固件库源码 
* startup 芯片启动文件及头文件
* utilities 其他第三方组件

**Project**
这个目录下面存放所有硬件板子的例程，其中超核K60开发板-渡鸦 的例程最全面

* 超核K60开发板-渡鸦
* frdmk64f120m
* KL46镭核核心板
* 超核姿态模块固件(URANUS)
* 原子核V2 核心板

**Document**

* 固件库使用说明
* 芯片手册
* 评估硬件原理图


## 联系我们及获取最新固件库
 - 论坛 [www.beyondcore.net](www.beyondcore.net)
 - 技术讨论群:247160311

## 已经发现的存在问题
一些版本的IAR 不能打开中文路径以及带有空格的路径下的功能，如果遇到这个问题，请将路径改为全英文且没有空格的路径

2014/4/28 16:31:01