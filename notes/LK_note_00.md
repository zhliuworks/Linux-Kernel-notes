# LK Note 00
> SJTU-CS353 Linux Kernel

> Refer to the slides of Prof. Quan Chen, Dept. of CSE, SJTU.
## Process Management on Multi-processor System: Scheduling Domain
* 单 CPU 上的进程调度
    * CPU 使用 `schedule()` 函数从本地 runqueue 中挑选进程运行
    * 考虑的是进程对 CPU 使用的需求，通过设置 priority 实现
* 多 CPU 上的进程调度
    * 一个 CPU 只从本地 runqueue 中挑选进程，一个可运行进程只在一个队列中存在
    * 内核周期性检查各 CPU 运行队列是否平衡，必要时将进程迁往其它队列
    * 考虑的是各 CPU 的**负载均衡**，通过**调度域**实现
### 调度域的组成
* 调度域是一个 CPU 集合，这些 CPU 的工作量由内核保持平衡
* **调度域分级组织**
    * 每一级包含具有相同属性的 CPU（同一个物理核，同一个 NUMA 节点）
    * 每一级 CPU 之间进行负载均衡的开销不同
        * 同一物理核的逻辑 CPU < 不同物理核的逻辑 CPU
        * 同一 NUMA 节点的 CPU < 不同 NUMA 节点的 CPU
    * 内核自下而上进行 CPU 负载均衡（优先进行开销小的均衡）
    * 考虑一个 2 sockets 的机器（2 NUMA nodes），每个 socket 上安装了一块 2 cores 的处理器，每个物理 core 支持 2-SMT 超线程
        * *SMT（Simultaneous Multithreading，同步多线程），在一个 CPU 的时钟周期内执行来自多个线程的指令的硬件多线程技术。本质上，SMT 是一种将多 CPU 上的 TLP 转化为单 CPU 上的 ILP 的方法。SMT 是单 CPU 从多个硬件线程上下文同时分派指令的能力*

<p align="center"><img src="imgs/0/1.png"/></p>

### 基于调度域的负载均衡过程
* 每个调度域被划分为一个或多个**调度组**，每个组是该域 CPU 集合的一个子集
* 负载均衡是在调度组之间完成的
* 负载均衡过程
    * 注册软中断 `SCHED_SOFTIRQ`，当一个 CPU 的软中断触发时，**该 CPU** 检查是否需要进行负载均衡
    * 该 CPU 从最底层调度域开始，自下而上遍历调度域（保证各层级组间负载均衡）
    * 计算该层域内所有调度组的平均负载，找到最繁忙的调度组，以及该组最繁忙的队列
    * 若本 CPU 在该域中**所在调度组平均负载**低于最繁忙的组，**从最繁忙的队列中迁移一些进程到本 CPU 上**

<p align="center"><img src="imgs/0/2.png"/></p>



## Power Management: From Linux Kernel to Android



## New Directions of Operating System Kernel

