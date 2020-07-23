# 前言

## 说明
作者大学生一个,技术水平有限,而且学习方向也不是破解逆向相关的,有错误的地方还望大佬指点。

## 再次说明
教程以交流技术为主要目的,只学技术,可别卖挂啊!!!!

## 针对对象
教程针对的是刚刚入门相关技术的，所以会简单点。

## 需要工具
Cheat Engine(CE) - 找游戏内基址和偏移，略微懂一点就好。

Visual Studio(VS) - 编写和编译C++代码，应该哪一个版本的都没问题

## 教程安排
1.先看一下辅助软件的最终效果，看看结果满意不

2.开始用CE查找游戏内的基址和偏移

3.编写代码实现游戏的hook相关操作

4.使用imgui界面库实现菜单功能

5.编写代码实现游戏的透视和方框操作

6.编写代码实现游戏的自瞄操作

7.完善代码和添加一些其它的功能

8.讲解一下Osiris和Gladiatorcheatz-v2.1代码 -> 怎么实现人物辉光、怎么实现武器换肤、怎么实现大陀螺(anti aim)、怎么实现无限举报玩家.....

9.发现啥牛逼的技术再进行更新

## 给我点个Star咯 ^_^ 让我有更多动力给你们分享更多教程

## 防锁机

[wmi-static-spoofer  ->  告诉你怎么样防止游戏锁电脑硬件](https://github.com/Alex3434/wmi-static-spoofer)

[hwid  ->  应用层和内核层一起防锁](https://github.com/btbd/hwid)

[negativespoofer  ->  SMBIOS tables修改](https://github.com/SamuelTulach/negativespoofer)

## 过检测
[kdmapper  ->  国外用的人还蛮多,驱动程序手动映射器](https://github.com/z175/kdmapper)

[kdmapper-1803-1903  ->  同上](https://github.com/alxbrn/kdmapper-1803-1903)

[kdmapper  ->  这个是Win10的1909版本的](https://github.com/Dark7oveRR/kdmapper)

[Kernel_Driver_Hack  ->  见名知意](https://github.com/TheCruZ/Kernel_Driver_Hack)

[ScyllaHide  ->  游戏有反调试?那我们就用反反调试!](https://github.com/x64dbg/ScyllaHide)

[kernel-read-write-using-ioctl  ->  简单的ioctl内核读写](https://github.com/beans42/kernel-read-write-using-ioctl)

[efi-memory  ->  EFI驱动读写程序](https://github.com/SamuelTulach/efi-memory)

[EfiGuard  ->  禁用PatchGuard和驱动程序签名强制DSE](https://github.com/Mattiwatti/EfiGuard)

[EAC Bypass  ->  调戏EAC保护的游戏](https://github.com/Schnocker/EAC_dbp)

[Kernelmode-manual-mapping-through-IAT  ->  不需要创建线程的Dll注入器](https://github.com/mactec0/Kernelmode-manual-mapping-through-IAT)

[Splendid Implanter  ->  告诉你怎么绕过BE](https://github.com/haram/splendid_implanter)

[BattlEye  ->  绕过BE,堡垒之夜也能用?](https://github.com/Schnocker/NoEye)

[access  ->  不需要句柄也能访问游戏进程](https://github.com/btbd/access)

[modmap  ->  模块扩展手动映射器](https://github.com/btbd/modmap)

[drvmap  ->  似乎也是一个驱动映射](https://github.com/not-wlan/drvmap)

[NoBastian  ->  一个应用层的绕过例子](https://github.com/mlghuskie/NoBastian)

[EUPMAccess  ->  物理内存的访问](https://github.com/waryas/EUPMAccess/tree/master/EnablePhysicalMemory)

[KernelBhop  ->  驱动级的读写](https://github.com/Zer0Mem0ry/KernelBhop)

[DSEFix  ->  Windows x64驱动程序签名强制替代](https://github.com/hfiref0x/DSEFix)

[smap  ->  DLL手动映射器](https://github.com/btbd/smap)

[HLeaker  -> 一个使用DuplicateHandle的例子](https://github.com/Schnocker/HLeaker)

[vmread  ->  内存读写相关](https://github.com/h33p/vmread)

[Eac-Bypass  ->  教你们怎么绕过VAC的](https://github.com/Flashyyyyyy/Eac-Bypass)

## 相关库
[nt_wrapper  ->  更快更容易更难反编译](https://github.com/JustasMasiulis/nt_wrapper)

[skCrypter  ->  C++11的轻量级字符串加密库](https://github.com/skadro-official/skCrypter)

[hidden  ->  强大的隐藏工具,能隐藏什么?去看看吧](https://github.com/JKornev/hidden)

## 过VAC检测
[VAC Bypass  ->  看了一下,用Dll注入Hook相关检测函数](https://github.com/danielkrupinski/VAC-Bypass)

[VAC  ->  介绍了VAC是如何检测作弊的](https://github.com/danielkrupinski/VAC)

[VAC Bypass Loader  ->  启动一个没有VAC的Steam,我尝试不行,可能要稍作调试](https://github.com/danielkrupinski/VAC-Bypass-Loader)

## CS:GO
[Osiris  ->  将近1k个Star,功能齐全,应该蛮多人用的哦](https://github.com/danielkrupinski/Osiris)

[RPM-WPM-project  ->  外部的,里面会教你怎么防人物锁墙](https://github.com/mikeeek/RPM-WPM-project)

## APEX
[ApexGaming.py  ->  python写的](https://github.com/AnusReaper/ApexGaming.py)

[EzApexDMAAimbot  ->  APEX辉光自瞄](https://github.com/Y33Tcoder/EzApexDMAAimbot)

## GTA5
[gta5_cheats_1.50  ->  这是我自己根据其它大佬的代码重写的,可以去试试看](https://github.com/FiYHer/gta5_cheats_1.50)

## 相关网站

[一号选手-英文](https://www.unknowncheats.me/forum/index.php):一个国外的辅助论坛，CSGO、使命、APEX、CF.....的源码、地址、偏移、思路.......

[二号选手-英文](https://guidedhacking.com/)
这是一个真正游戏作弊教程网站


























