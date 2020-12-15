# SAMSUNG S5PV210

***

## 编译源码
```shell
make clean
make CROSS_COMPILE=/path/to/arm-none-linux-gnueabihf- PLATFORM=arm32-s5pv210
```

## 烧写到SD卡
```shell
sudo dd if=xboot.bin of=/dev/sdb bs=512 seek=1 conv=sync
```

## 运行
配置SD卡启动模式，插入SD卡，上电，串口2显示Debug信息。


# 相关网址

- XBOOT官方首页
  https://xboot.github.io/

- XBOOT源码(完全开源)，寄存在github，请直接`fork`一份代码，如需申请合并，直接`pull request`即可
  https://github.com/xboot/xboot

- XBOOT运行效果展示页面，首次接触的用户，建议观看，对XBOOT有个直观的感受
  https://github.com/xboot/xboot/blob/master/documents/xboot-show-case-zh-CN.md

- XBOOT系统开发指南
  https://github.com/xboot/xboot/blob/master/documents/xboot-system-development-guide-zh-CN.md

- XBOOT应用开发指南
  https://github.com/xboot/xboot/blob/master/documents/xboot-application-development-guide-zh-CN.md

- XBOOT官方QQ群，大佬聚集，请踊跃加入
  [658250248](https://jq.qq.com/?_wv=1027&k=5BOkXYO) (2000人)
