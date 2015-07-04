1、XLS生成的文件后，需要手动将0x040字段改成如下地址：
00008000	// 0x040 : Device Read Address from 2ndboot Device.

2、PLL1为CPU时钟源，PLL3为DDR时钟源

3、其余外设总线时钟使用PLL0以及PLL2

4、修正各种频率至系统要求的频率范围，避免因超频引起的不稳定隐患。