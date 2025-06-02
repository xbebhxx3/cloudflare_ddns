# cf_ddns

一个跨平台的 Cloudflare 动态 DNS (DDNS) 自动更新工具，支持 IPv4 和 IPv6 地址，且可以灵活配置是否单独更新。

## 主要特性

- 自动检测当前公网 IPv4 和 IPv6 地址
- 根据配置选择更新 IPv4 和/或 IPv6 DNS 记录
- 支持 Cloudflare API，自动获取 Zone ID 和 DNS 记录 ID
- 跨平台支持 Linux 和 Windows
- 支持多种 CPU 架构：x86_64、x86、aarch64 和 armv7

## 支持平台与架构

| 平台   | 架构                   | 说明                      |
| ------ | ---------------------- | ------------------------- |
| Linux  | x86_64, x86, aarch64, armv7 | 预编译支持               |
| Windows| x86_64, x86            | 需自行编译，aarch64 暂不支持 |

## 快速开始

1. 在 Cloudflare 控制面板添加相应的 A 或 AAAA 解析记录。
2. 编辑项目根目录下的 `config.json` 配置文件，填写 API 密钥、邮箱、域名、记录名等信息。
3. 运行程序，自动检测当前公网 IP 并更新 DNS 记录。
4. 可将程序设置为系统定时任务，实现持续自动更新。

## 编译指南

### Linux

确保已安装 curl 开发库：

```bash
sudo apt-get update
sudo apt-get install -y libcurl4-openssl-dev g++ make
make
```

### Windows

1. 使用 vcpkg 安装依赖库：
    vcpkg install curl
2. 使用命令行编译（请根据实际路径调整）:
    g++ -std=c++17 main.cpp -I"path/to/vcpkg/installed/x64-windows/include" -L"path/to/vcpkg/installed/x64-windows/lib" -lcurl -o cf_ddns.exe


### 配置文件示例（config.json）
```json
{
  "cfkey": "你的Cloudflare全局API密钥",
  "cfuser": "你的Cloudflare注册邮箱",
  "zone_name": "主域名 例如example.com",
  "record_name": "ddns的子域名 ddns.example.com",
  "ttl": 120, 生存时间 单位为秒
  "update_ipv4": true,
  "update_ipv6": true,
  "ipv4_services": [
    "https://icanhazip.com",
    "https://ifconfig.me"
  ],
  "ipv6_services": [
    "https://api6.ipify.org",
    "https://ifconfig.co/ip"
  ]
}
```