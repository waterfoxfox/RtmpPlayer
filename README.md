# Low delay windows h264 h265 rtmp player with simple api and single dll


## RtmpPlaySdk简介
一款低延时的极简接口RTMP播放器（Windows版和Android版）。其中Windows版播放器的特性如下：

* 1、支持Rtmp掉线自动重连。
* 2、支持非阻塞Rtmp连接，外层可随时中断。
* 3、支持多实例
* 4、支持任意AAC采样率、声道数，内部自动resample，支持音量调节。
* 5、支持H264/H265+AAC组合Rtmp流，支持同步TS\MP4文件录制。
* 6、支持渲染时保证画面宽高比而自适应加黑边。
* 7、支持外层可设置的Jitter Buff延时，设置为0时为极速模式，配合低延时推送端最小延时仅500ms。
* 8、仅六个接口，调用简洁，用户只需传入播放器窗口句柄即可。
* 9、整个系统仅由一个DLL组成，占用空间小。性能强劲，单路720P 30fps在i5 CPU上占用率5%以内。
* 10、C++开发，支持C、C++、C#
* 11、支持XP\WIN7\WIN8\WIN10

#### Android版本: https://github.com/waterfoxfox/RtmpPlayer-Android

## RtmpPlaySdk  C API

### 
* 环境初始化，系统只需调用一次<br>
@param: outputPath：日志文件输出的目录，若目录不存在将自动创建<br>
@param: outputLevel：日志输出的级别，只有等于或者高于该级别的日志输出到文件<br>
@return: <br>
void  `RtmpPlayer_Enviroment_Init`(const char * outputPath,  LOG_OUTPUT_LEVEL outputLevel);

### 
* 环境反初始化，系统只需调用一次<br>
@return:<br>
void  `RtmpPlayer_Enviroment_Free`();

### 
* 创建RtmpPlayer<br>
@return: 返回模块指针，为NULL则失败<br>
void*  `RtmpPlayer_Create`();

### 
* 销毁RtmpPlayer,注意：【涉及到资源销毁，使用者应该做好本接口与其他接口的互斥保护】<br>
@param pRtmpPlayer: 模块指针<br>
@return: <br>
void  `RtmpPlayer_Delete`(void* pRtmpPlayer);

### 
* 开始拉流Rtmp并播放<br>
@param pRtmpPlayer: 模块指针<br>
@param strRtmpPlayUrl: Rtmp地址<br>
@param unJitterBuffDelay: 内部缓存时间，缓存时间越大延时越大、流畅性越好。反之延时越小，流畅性越差。范围[0, 4000]，单位毫秒<br>
@param pDisplayHandle: 渲染输出的窗口句柄<br>
@return: TURE成功，FALSE失败<br>
BOOL  `RtmpPlayer_Start`(void* pRtmpPlayer, char *strRtmpPlayUrl, UINT unJitterBuffDelay, void* pDisplayHandle);

### 
* 停止拉流Rtmp播放<br>
@param pRtmpPlayer: 模块指针<br>
@return: <br>
void  `RtmpPlayer_Stop`(void* pRtmpPlayer);

### 
* 获取RTMP连接状态<br>
@param pRtmpPlayer: 模块指针<br>
@return: RTMP连接状态<br>
RtmpPlay_Status  `RtmpPlayer_GetRtmpStatus`(void* pRtmpPlayer);

### 
应用案例：i8财经直播 http://www.i8zhibo.cn/

### 
H265 over RTMP采用业内普遍使用的私有标准（SRS、金山云、大牛直播等均采用该标准），使用如下环境测试：<br>
推送端：ffmpeg补丁版 https://github.com/ossrs/srs/pull/1721#issuecomment-619460847  <br>
服务器：SRS 4.x https://github.com/ossrs/srs/tree/feature/h265  <br>

### 本库仅做演示用途，若需要商用或定制服务请联系 www.mediapro.cc
