# 第20届智能车竞赛 · 双车跟随

前车沿赛道巡线，后车用摄像头看前车尾灯并跟随。两车固件都跑在英飞凌 AURIX TC387 上，用逐飞开源库，在 AURIX Development Studio（ADS / TASKING）里分别编译、烧录。

代码作者 **DJL**（GitHub：[DJlin06](https://github.com/DJlin06)）。

## 两车怎么配合

这是视觉跟随，不是两车之间的无线编队。

前车自己看赛道。图像在 `F_Car-use-test-end/code/images.c`（最长白列巡线、十字等）和 `code/island.c`（环岛，配合陀螺仪积分）。发车时 `code/UI.c` 的 `Begin()` 会打开涵道，并把 `P10_3` 拉高，点亮尾灯，再给出速度。

后车不和前车通信。`C_Car_Test/code/images.c` 用 MT9V03X 图像做二值化、连通域标记，按圆形度筛出两个光斑（`find_circles`），用两灯质心算横向偏差，再用光斑纵向方差估计与前车的距离（`Get_err`、`LQ_Deal_Image`）。丢灯或两灯被误判时走保护（`Protect`、`Img_dissapear`）。看到两盏灯后 `Begin()` 延时发车，这段延时按前车亮灯流程对齐。转向由舵机完成（`pid.c` 的 `Servo_ctrl`）。

无线串口只连电脑上位机，用来调 PID、速度等参数，并把偏差等数据打回上位机。前车、后车都有 `PID_send()`；中断里注明串口 2 默认接无线转串口模块。

## 目录

三个目录都是独立的 ADS 工程。每个工程自带一份 `libraries/`（逐飞 TC387 开源库和英飞凌 iLLD），体积接近，头文件有少量差别。重复是故意的，这样单个工程就能单独导入编译。

| 目录 | ADS 工程名（`.project`） | 作用 |
| --- | --- | --- |
| `F_Car-use-test-end/` | `F_Car-use-test-end` | 前车 / 头车。巡线，含环岛等赛道元素，发车时点尾灯 |
| `C_Car_Test/` | `C_Car_Test_End` | 从车 / 后车。视觉跟尾灯光斑，舵机转向 |
| `Seekfree_TC387_Opensource_Library/` | `264 to 387 Test` | 逐飞开源库工程模板，不是比赛固件 |

比赛逻辑主要在各工程的：

- `code/`：图像、PID、UI、Flash、初始化。新增源文件直接放在这个目录里，不要再套子文件夹，然后在工程里把文件加进去再编译（见 `code/本文件夹作用.txt`）。
- `user/`：CPU0–CPU3 入口和 `isr.c`。前车 CPU0 做初始化和上位机收参，CPU1 跑图像，CPU3 跑按键和屏幕；后车 CPU0 同样收参并向上位机发送，CPU1 跑图像，CPU2 跑按键和屏幕。
- `libraries/`：逐飞驱动与英飞凌底层。版本见 `libraries/doc/version.txt`（本树为 V3.3.1）。头文件中的开发环境为 ADS v1.9.20，适用平台 TC387QP。

## 技术栈

- 语言：C
- 芯片：英飞凌 AURIX TC387QP
- 库：逐飞 TC387 开源库（公共层、外设、逐飞助手组件）以及英飞凌 iLLD
- 工具：AURIX Development Studio，TASKING 工具链（链接脚本 `Lcf_Tasking_Tricore_Tc.lsl`）
- 摄像头：MT9V03X（`mt9v03x_init`）
- 显示：IPS200，SPI
- 调参：无线转串口（`wireless_uart`）对电脑上位机
- 前车还会初始化 IMU660RA，用于环岛等角度积分；工程里另有 `code/icm42688.c`

引脚以工程里的初始化代码和注释为准，这里不另列一套接线表。前车 `code/init.c` 里写明：`P10_3` 是灯光控制，`P02_5` 是蜂鸣器。后车舵机 PWM 在 `code/init.c` 中初始化为 `ATOM1_CH3_P10_3`。逐飞主板的推荐分配和不宜使用的 boot 引脚见各工程根目录的 `推荐IO分配.txt`、`尽量不要使用的引脚.txt`。

## 编译与烧录

1. 安装 AURIX Development Studio（工程按 ADS v1.9.20 维护）。
2. **File → Import → Existing Projects into Workspace**，选中要烧的那个工程目录（前车、后车或库模板）。文件夹名和 ADS 工程名可以不一致，以后车为例：目录是 `C_Car_Test`，工程名是 `C_Car_Test_End`。
3. 导入后在工程上 **Refresh**，再编译。库自带注释说明工程默认关闭优化，需要的话在工程属性里把 Optimization level 调到常用的 2 级。
4. 用逐飞英飞凌 TriCore 调试下载器接到核心板调试口下载。供电、跳线以手头核心板资料为准。后车 `user/cpu0_main.c` 里保留了逐飞例程对下载器和 USB-TTL 的连接说明（调试串口默认波特率见 `zf_common_debug.h`）。
5. 要改 ADS 工程名时，用该工程根目录的 `AURIX修改工程名称.bat`。说明见同目录 `AURIX修改工程名称教程.txt`（视频：<https://www.bilibili.com/video/BV1nBkMYMEwE>）。
6. `删除临时文件.bat` 会删掉该工程下的 `Debug` 目录和 `*.launch`，适合清理本地编译产物。

## 关于本仓库里没有的编译产物

上传时已去掉全部 `Debug/`（目标文件、elf、hex、map 等），以及 `bin/`、`obj/`、`.vs/` 一类本地生成文件。克隆后在 ADS 里重新编译即可。

根目录 `.gitignore` 按 ADS / Eclipse 和 Windows 工程习惯忽略这些内容，包括：

- `Debug/`、`Release/`、`bin/`、`obj/`
- `*.o`、`*.elf`、`*.hex`、`*.map` 等编译输出
- `.ads/`、`*.launch`
- `.vs/`、`*.suo` 以及编辑器临时文件

`.project`、`.cproject`、`.settings` 需要保留，否则 ADS 无法按原工程导入。

## 许可证

`libraries/` 里的逐飞 TC387 开源库使用 **GPL-3.0**。英文许可声明在各工程的 `libraries/doc/GPL3_permission_statement.txt`；源文件头部也有 GPL-3.0 说明和成都逐飞科技的版权声明。再分发或修改该库时请遵守 GPL-3.0，并保留逐飞的版权声明。GPL 全文见 <https://www.gnu.org/licenses/>。

`code/`、`user/` 里的比赛业务代码由 DJL 编写，建立在上述开源库之上。
