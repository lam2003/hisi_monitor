## 海思NVR
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
#### 注意事项:
- 由于 **hi3516A100V** 只有 **32M** 的内存，无法开启 **1080P(会导致内存无法分配)** ，最高支持 **720P 25FPS**

- **mp4v2**有很长一段时间没有更新了，库本身存在一些bug，录制时长不能超过**90**分钟，否者会出出现时间戳异常的问题，本项目使用分段避免了该问题。储存空间有限，无法使用 **ffmpeg** 等流行的多媒体第三方库。

*欢迎问题反馈，交流学习，联系方式linmin093@gmail.com*
