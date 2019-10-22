海思NVR
=====


基于 *****hi3516A***** SOC开发,可进行rtmp推流、移动侦测、mp4录制。

#### 依赖库:
- srs_rtmp
- mp4v2
- jsoncpp

#### 编译方法:
已经将交叉编译后的库上传到[https://github.com/lam2003/monitor_3rdparty.git](https://github.com/lam2003/monitor_3rdparty.git)
```
#安装交叉工具链arm-hisiv500-linux(uclib)
#进入源码根目录
mkdir ./build
cmake ../
#自动下载依赖
make 
#编译好的elf在./build/bin/monitor
#在开发板上运行
./monitor -c [配置文件路径]
```



*出售HI3531/HI3532级联板 课堂录播完整解决方案，带源码出售，联系方式 notify@linmin.xyz*
