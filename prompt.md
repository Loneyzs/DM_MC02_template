### 这是一个DM_MC02开发板的Zephyr适配模板工程

### 待办清单

* 上轮对话的几个小问题修正：
  * MU加热是TIM3 CH4没错，引脚分配里写错了，修改一下。
  * 开chosen节点，RTT作为终端输出，释放一个uart。
* BMI088复用上游驱动。
* 参考`E:\Files\MCU_projects\dm-mc02-master\例程\CtrBoard-H7_IMU_TempCtrl`中例程的imu加热控制，将加热控制写成一个板级驱动。
* 参考`E:\zephyr_repo\RM_C_template`另一个Zephyr模板仓库里的最小测试，把这块板子有的外设（ist8310这块板子没有，九轴融合IMU也没有）的最小测试也加进来一份。

### 通用规则

* 严禁改动Zephyr源码和sdk内容
* 如果有硬件资料不明确的问题，请你直接停止并向我询问