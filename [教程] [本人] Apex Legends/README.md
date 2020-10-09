# 前言

[apex dump](https://github.com/CasualX/pelite/tree/master/examples/apex)

这个我怎么搞都搞不过,老是编译失败,会rust的兄弟可以自己去试试看

搞不过,还是要自己写一个sig匹配,无语.....

在这里说一下过EAC的办法吧(我自己现在用的,没被封禁)

1.bypass pg(没过这个后面很难搞的,动不动就是0x109蓝屏)

2.read/write process memory driver(没读写驱动就没戏呀,我用MDL方式读写内存的)

3.hide driver(把驱动隐藏了为妙,不然不太稳感觉)

4.clear all trace(比如你使用了漏洞驱动/泄露签名证书驱动....这些痕迹都要清空了去)

5.hdd spoofer(我的电脑被锁硬件了,要改)

6.over(相对安全了)
