/**
 * ESP32-S3 TCP服务器模板
 * 功能：创建稳定的TCP服务器，支持多客户端连接
 * 作者：张凯莉
 * 日期：2025年
 *
 * 使用说明：
 * 1. ESP32创建WiFi热点（AP模式）
 * 2. 手机/电脑连接此热点
 * 3. 使用TCP客户端连接ESP32
 * 4. 发送和接收数据
 * 
 * 特点：
 * - 支持AP和STA两种模式
 * - 多客户端连接（最多4个）
 * - 双端口监听
 * - 完整的数据处理
 * - 详细的日志输出
 */
#include <Arduino.h>    // Arduino核心库，必须包含
#include <WiFi.h>       // WiFi功能库，用于网络连接
// ========== 【第一步：配置参数】 ==========
// 在这里设置你的网络参数
// 模式选择：取消注释使用STA模式，注释掉使用AP模式
// #define USE_STA_MODE  // 取消这行注释使用STA模式（连接路由器）
// AP模式配置（ESP32创建热点）
const char* AP_SSID = "ESP32_TCP_Server";  // 热点名称
const char* AP_PASSWORD = "12345678";      // 热点密码，至少8位
// STA模式配置（ESP32连接路由器）
const char* STA_SSID = "你的WiFi名称";      // 你的WiFi名称
const char* STA_PASSWORD = "你的WiFi密码";  // 你的WiFi密码
// TCP服务器配置
#define MAIN_PORT 1122      // 主服务端口号
#define BACKUP_PORT 1123    // 备用端口号
#define MAX_CLIENTS 4       // 最大客户端连接数
// ========== 【第二步：TCP服务器管理类】 ==========
// 这个类封装了所有TCP服务器功能
class TCPServerManager {
private:
    // 成员变量定义
    WiFiServer mainServer;          // 主服务器对象
    WiFiServer backupServer;        // 备用服务器对象
    WiFiClient clients[MAX_CLIENTS]; // 客户端连接数组
    unsigned long dataReceived;      // 接收数据统计
    unsigned long dataSent;          // 发送数据统计
    int activeClients;               // 当前活动客户端数
    unsigned long startTime;         // 服务器启动时间
    
    /**
     * @brief 初始化WiFi（AP模式）
     * 解释：ESP32创建WiFi热点，其他设备连接这个热点
     */
    void initAPMode() {
        Serial.println("\n【1/4】设置AP模式...");
        
        // 1. 设置WiFi模式为AP（Access Point）
        WiFi.mode(WIFI_AP);
        
        // 2. 配置AP参数
        bool success = WiFi.softAP(AP_SSID, AP_PASSWORD);
        
        if (!success) {
            Serial.println("❌ AP模式启动失败！");
            return;
        }
        
        // 3. 获取并显示IP地址
        Serial.println("✅ AP模式启动成功");
        Serial.print("📡 热点名称(SSID): ");
        Serial.println(AP_SSID);
        Serial.print("🔑 热点密码: ");
        Serial.println(AP_PASSWORD);
        Serial.print("🌐 IP地址: ");
        Serial.println(WiFi.softAPIP());
        Serial.print("📶 MAC地址: ");
        Serial.println(WiFi.softAPmacAddress());
        
        // 4. 显示连接说明
        printConnectionInstructions();
    }
    
    /**
     * @brief 初始化WiFi（STA模式）
     * 解释：ESP32连接到现有的WiFi路由器
     */
    void initSTAMode() {
        Serial.println("\n【1/4】设置STA模式...");
        
        // 1. 设置WiFi模式为STA（Station）
        WiFi.mode(WIFI_STA);
        
        // 2. 开始连接WiFi
        Serial.print("📶 正在连接WiFi: ");
        Serial.println(STA_SSID);
        WiFi.begin(STA_SSID, STA_PASSWORD);
        
        // 3. 等待连接，最多尝试20次
        Serial.print("⏳ 连接中");
        int attempts = 0;
        while (WiFi.status() != WL_CONNECTED && attempts < 20) {
            delay(500);
            Serial.print(".");
            attempts++;
        }
        Serial.println();
        
        // 4. 检查连接结果
        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("✅ WiFi连接成功！");
            Serial.print("🌐 IP地址: ");
            Serial.println(WiFi.localIP());
            Serial.print("📶 MAC地址: ");
            Serial.println(WiFi.macAddress());
            Serial.print("📡 信号强度: ");
            Serial.print(WiFi.RSSI());
            Serial.println(" dBm");
        } else {
            Serial.println("❌ WiFi连接失败！");
            Serial.println("请检查：");
            Serial.println("1. WiFi名称和密码是否正确");
            Serial.println("2. 路由器是否正常工作");
            Serial.println("3. ESP32是否在路由器信号范围内");
        }
    }
    
    /**
     * @brief 打印连接说明
     */
    void printConnectionInstructions() {
        Serial.println("\n📱【连接步骤】📱");
        Serial.println("1. 用手机/电脑连接ESP32的WiFi热点");
        Serial.print("   热点: ");
        Serial.println(AP_SSID);
        Serial.print("   密码: ");
        Serial.println(AP_PASSWORD);
        Serial.println("\n2. 打开TCP客户端工具");
        Serial.println("   📱 手机：下载'网络调试助手'APP");
        Serial.println("   💻 电脑：使用NetAssist、Hercules或Putty");
        Serial.println("\n3. 创建TCP客户端连接");
        Serial.print("   IP地址: ");
#ifdef USE_STA_MODE
        Serial.println(WiFi.localIP());
#else
        Serial.println(WiFi.softAPIP());
#endif
        Serial.print("   端口号: ");
        Serial.print(MAIN_PORT);
        Serial.print(" 或 ");
        Serial.println(BACKUP_PORT);
        Serial.println("\n4. 连接成功后，可以发送数据测试");
        Serial.println("   试试发送: ping, status, help");
    }
    
    /**
     * @brief 查找空闲的客户端位置
     * @return 空闲位置索引，-1表示已满
     */
int findFreeClientSlot() {
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (!clients[i].connected()) {
                return i;
            }
        }
        return -1;  // 没有空闲位置
    }
    
    /**
     * @brief 处理客户端命令
     * @param clientId 客户端ID
     * @param command 接收到的命令
     * @return 响应字符串
     */
    String processClientCommand(int clientId, String command) {
        // 转换为小写，便于比较
        command.toLowerCase();
        String response = "";
        
        // 命令处理逻辑
        if (command == "ping") {
            response = "pong [时间戳: " + String(millis()) + "ms]\r\n";
        }
        else if (command == "status") {
            response = "📊 服务器状态:\r\n";
            response += "   客户端数: " + String(activeClients) + "/" + String(MAX_CLIENTS) + "\r\n";
            response += "   接收数据: " + String(dataReceived) + " bytes\r\n";
            response += "   发送数据: " + String(dataSent) + " bytes\r\n";
            response += "   运行时间: " + String((millis() - startTime) / 1000) + " 秒\r\n";
        }
        else if (command == "time") {
            response = "🕐 系统时间: " + String(millis()) + " ms\r\n";
        }
        else if (command == "help") {
            response = "📖 可用命令:\r\n";
            response += "   ping    - 测试连接\r\n";
            response += "   status  - 查看服务器状态\r\n";
            response += "   time    - 查看系统时间\r\n";
            response += "   help    - 显示帮助\r\n";
            response += "   echo [文本] - 回显文本\r\n";
            response += "   close   - 断开连接\r\n";
        }
        else if (command.startsWith("echo ")) {
            String text = command.substring(5);
            response = "📢 回显: " + text + "\r\n";
        }
        else if (command == "close") {
            response = "👋 正在断开连接...\r\n";
            if (clients[clientId].connected()) {
                clients[clientId].stop();
                activeClients--;
                Serial.print("客户端[");
                Serial.print(clientId);
                Serial.println("] 已主动断开");
            }
        }
        else {
            // 默认处理：回显接收到的内容
            response = "📩 收到: " + command + " (长度: " + String(command.length()) + ")\r\n";
        }
        
        return response;
    }
    
public:
    /**
     * @brief 构造函数
     * 解释：创建对象时自动调用，初始化成员变量
     */
    TCPServerManager() 
        : mainServer(MAIN_PORT), backupServer(BACKUP_PORT) {
        // 初始化统计变量
        dataReceived = 0;
        dataSent = 0;
        activeClients = 0;
        startTime = millis();
        
        // 初始化客户端数组
        for (int i = 0; i < MAX_CLIENTS; i++) {
            clients[i] = WiFiClient();  // 创建空的客户端对象
        }
    }
    
    /**
     * @brief 初始化函数
     * 解释：设置WiFi和启动TCP服务器
     */
    void begin() {
        Serial.println("\n==========================================");
        Serial.println("       ESP32-S3 TCP服务器初始化");
        Serial.println("==========================================");
        
        // 【步骤1】初始化WiFi
#ifdef USE_STA_MODE
        initSTAMode();  // STA模式：连接路由器
#else
        initAPMode();   // AP模式：创建热点（默认）
#endif
        
        // 【步骤2】启动TCP服务器
        Serial.println("\n【2/4】启动TCP服务器...");
        mainServer.begin();
        backupServer.begin();
        Serial.print("✅ 主服务器端口: ");
        Serial.println(MAIN_PORT);
        Serial.print("✅ 备用服务器端口: ");
        Serial.println(BACKUP_PORT);
        
        // 【步骤3】显示服务器信息
        Serial.println("\n【3/4】服务器信息：");
        Serial.print("📊 最大客户端数: ");
        Serial.println(MAX_CLIENTS);
        Serial.print("⏰ 服务器启动时间: ");
        Serial.println(startTime);
        
        // 【步骤4】完成提示
        Serial.println("\n【4/4】✅ 初始化完成！");
        Serial.println("==========================================\n");
        Serial.println("🚀 服务器就绪，等待客户端连接...");
        Serial.println("💡 提示：输入 'm' 查看控制菜单");
        Serial.println("==========================================\n");
    }
    
/**
     * @brief 主更新函数
     * 解释：必须在loop()中循环调用，处理网络事件
     */
    void update() {
        // 【功能1】检查新连接（主端口）
        checkNewConnections(mainServer);
        
        // 【功能2】检查新连接（备用端口）
        checkNewConnections(backupServer);
        
        // 【功能3】处理客户端数据
        handleClientData();
        
        // 【功能4】清理断开连接的客户端
        cleanupDisconnectedClients();
        
        // 【功能5】定期显示状态（每10秒一次）
        static unsigned long lastStatusTime = 0;
        if (millis() - lastStatusTime > 10000) {
            lastStatusTime = millis();
            printServerStatus();
        }
    }
    
    /**
     * @brief 检查新连接
     * @param server 要检查的服务器对象
     */
    void checkNewConnections(WiFiServer& server) {
        // 检查是否有客户端尝试连接
        if (server.hasClient()) {
            // 查找空闲的客户端位置
            int freeSlot = findFreeClientSlot();
            
            if (freeSlot >= 0) {
                // 接受客户端连接
                clients[freeSlot] = server.available();
                
                // 配置客户端
                clients[freeSlot].setTimeout(100);  // 设置读取超时100ms
                
                // 记录日志
                Serial.print("🎉 新客户端连接 [ID:");
                Serial.print(freeSlot);
                Serial.print("] IP:");
                Serial.print(clients[freeSlot].remoteIP());
                Serial.print(" 端口:");
                Serial.println(clients[freeSlot].remotePort());
                
                // 发送欢迎消息
                String welcomeMsg = "👋 欢迎连接到ESP32 TCP服务器 [客户端" + String(freeSlot) + "]\r\n";
                welcomeMsg += "💡 输入 'help' 查看可用命令\r\n";
                clients[freeSlot].print(welcomeMsg);
                
                // 更新统计
                dataSent += welcomeMsg.length();
                activeClients++;
                
            } else {
                // 服务器已满，拒绝连接
                Serial.println("⚠️ 服务器已满，拒绝新连接");
                WiFiClient tempClient = server.available();
                tempClient.stop();  // 断开连接
            }
        }
    }
/**
     * @brief 处理客户端数据
     */
    void handleClientData() {
        // 遍历所有客户端
        for (int i = 0; i < MAX_CLIENTS; i++) {
            // 检查客户端是否连接且有数据可读
            if (clients[i].connected() && clients[i].available()) {
                // 读取一行数据（以换行符结束）
                String receivedData = clients[i].readStringUntil('\n');
                receivedData.trim();  // 去除首尾空白字符
                
                // 检查数据是否有效
                if (receivedData.length() > 0) {
                    // 更新统计
                    dataReceived += receivedData.length();
                    
                    // 打印接收日志
                    Serial.print("📥 客户端[");
                    Serial.print(i);
                    Serial.print("] 发送: ");
                    Serial.println(receivedData);
                    
                    // 处理命令并获取响应
                    String response = processClientCommand(i, receivedData);
                    
                    // 发送响应（如果有）
                    if (response.length() > 0) {
                        clients[i].print(response);
                        dataSent += response.length();
                        
                        // 打印发送日志
                        Serial.print("📤 回复客户端[");
                        Serial.print(i);
                        Serial.print("]: ");
                        Serial.print(response.substring(0, min(30,(int)response.length())));
                        if (response.length() > 30) Serial.print("...");
                        Serial.println();
                    }
                }
            }
        }
    }
    
    /**
     * @brief 清理断开连接的客户端
     */
    void cleanupDisconnectedClients() {
        // 遍历所有客户端
        for (int i = 0; i < MAX_CLIENTS; i++) {
            // 检查客户端对象是否有效
            if (clients[i]) {
                // 检查是否断开连接
                if (!clients[i].connected()) {
                    // 停止客户端连接
                    clients[i].stop();
                }
            }
        }
        
        // 重新计算活动客户端数
        int count = 0;
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].connected()) {
                count++;
            }
        }
        activeClients = count;
    }
    
    /**
     * @brief 打印服务器状态
     */
    void printServerStatus() {
        Serial.println("\n📊 === 服务器状态报告 ===");
        Serial.print("⏰ 运行时间: ");
        Serial.print((millis() - startTime) / 1000);
        Serial.println(" 秒");
        
        Serial.print("👥 活动客户端: ");
        Serial.print(activeClients);
        Serial.print("/");
        Serial.println(MAX_CLIENTS);
        
        Serial.print("📥 接收数据: ");
        Serial.print(dataReceived);
        Serial.println(" bytes");
        
        Serial.print("📤 发送数据: ");
        Serial.print(dataSent);
        Serial.println(" bytes");
        
        Serial.print("🌐 IP地址: ");
#ifdef USE_STA_MODE
        Serial.println(WiFi.localIP());
#else
        Serial.println(WiFi.softAPIP());
#endif
        Serial.println("============================\n");
    }
    
    /**
     * @brief 打印客户端列表
     */
    void printClientList() {
        Serial.println("\n👥 === 客户端列表 ===");
        if (activeClients == 0) {
            Serial.println("   暂无客户端连接");
        } else {
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i].connected()) {
                    Serial.print("   [");
                    Serial.print(i);
                    Serial.print("] IP:");
                    Serial.print(clients[i].remoteIP());
                    Serial.print(" 端口:");
                    Serial.println(clients[i].remotePort());
                }
            }
        }
        Serial.println("=====================\n");
    }
/**
     * @brief 广播消息给所有客户端
     * @param message 要广播的消息
     */
    void broadcastMessage(String message) {
        Serial.print("📢 广播消息: ");
        Serial.println(message);
        
        int sentCount = 0;
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].connected()) {
                clients[i].print(message + "\r\n");
                sentCount++;
            }
        }
        
        dataSent += message.length() * sentCount;
        Serial.print("✅ 已发送到 ");
        Serial.print(sentCount);
        Serial.println(" 个客户端");
    }
    
    /**
     * @brief 发送消息到指定客户端
     * @param clientId 客户端ID
     * @param message 消息内容
     */
    void sendToClient(int clientId, String message) {
        if (clientId >= 0 && clientId < MAX_CLIENTS) {
            if (clients[clientId].connected()) {
                clients[clientId].print(message + "\r\n");
                dataSent += message.length();
                
                Serial.print("📤 发送到客户端[");
                Serial.print(clientId);
                Serial.print("]: ");
                Serial.println(message);
            } else {
                Serial.print("❌ 客户端[");
                Serial.print(clientId);
                Serial.println("] 未连接");
            }
        } else {
            Serial.println("❌ 无效的客户端ID");
        }
    }
    
    /**
     * @brief 获取服务器IP地址
     * @return IP地址字符串
     */
    String getServerIP() {
#ifdef USE_STA_MODE
        return WiFi.localIP().toString();
#else
        return WiFi.softAPIP().toString();
#endif
    }
    
    /**
     * @brief 获取客户端数量
     * @return 当前连接的客户端数
     */
    int getClientCount() {
        return activeClients;
    }
};
// ========== 【第三步：创建全局对象】 ==========
// 创建TCP服务器管理器对象
TCPServerManager tcpServer;
// ========== 【第四步：串口控制菜单】 ==========
/**
 * @brief 显示控制菜单
 */
void showControlMenu() {
    Serial.println("\n==========================================");
    Serial.println("           TCP服务器控制菜单");
    Serial.println("==========================================");
    Serial.println("【服务器状态】");
    Serial.println("  1 - 显示服务器状态");
    Serial.println("  2 - 显示客户端列表");
    
    Serial.println("\n【数据发送】");
    Serial.println("  3 - 广播测试消息");
    Serial.println("  4 [ID] [消息] - 发送到指定客户端");
    Serial.println("     示例: 4 0 Hello");
    
    Serial.println("\n【系统控制】");
    Serial.println("  5 - 重新初始化服务器");
    Serial.println("  6 - 显示连接说明");
    
    Serial.println("\n【帮助信息】");
    Serial.println("  m - 显示此菜单");
    Serial.println("  h - 显示详细帮助");
    Serial.println("  t - 发送测试数据");
    Serial.println("==========================================\n");
}
/**
 * @brief 显示详细帮助
 */
void showDetailedHelp() {
    Serial.println("\n📖 === 详细使用帮助 ===");
    
    Serial.println("\n🔧 【硬件连接】");
    Serial.println("1. 用USB线连接ESP32-S3到电脑");
    Serial.println("2. 确保开发板供电正常（红灯亮）");
    
    Serial.println("\n📡 【网络连接】");
#ifdef USE_STA_MODE
    Serial.println("模式：STA模式（连接到路由器）");
    Serial.println("1. 确保ESP32和电脑在同一WiFi网络");
    Serial.print("2. 服务器IP: ");
    Serial.println(tcpServer.getServerIP());
#else
    Serial.println("模式：AP模式（ESP32创建热点）");
    Serial.println("1. 用手机/电脑连接ESP32的热点");
    Serial.print("   热点名称: ");
    Serial.println(AP_SSID);
    Serial.print("   密码: ");
    Serial.println(AP_PASSWORD);
    Serial.print("2. 服务器IP: ");
    Serial.println(tcpServer.getServerIP());
#endif
    
    Serial.println("\n🔌 【客户端连接】");
    Serial.println("📱 手机端：");
    Serial.println("  1. 下载'网络调试助手'或'TCP调试助手'APP");
    Serial.println("  2. 创建TCP客户端");
    Serial.println("  3. 输入IP和端口（1122或1123）");
    Serial.println("  4. 点击连接");
    
    Serial.println("\n💻 电脑端：");
    Serial.println("  推荐工具：");
    Serial.println("  - NetAssist（网络调试助手）");
    Serial.println("  - Hercules");
    Serial.println("  - Putty（选择Raw连接）");
    
    Serial.println("\n📝 【测试命令】");
    Serial.println("连接成功后，可以发送：");
    Serial.println("  ping    - 测试连接");
    Serial.println("  status  - 查看状态");
    Serial.println("  help    - 显示帮助");
    Serial.println("  echo 你好 - 回显消息");
    
    Serial.println("\n⚙️ 【故障排除】");
    Serial.println("❌ 无法连接：");
    Serial.println("  1. 检查IP地址和端口是否正确");
    Serial.println("  2. 确保设备在同一网络");
    Serial.println("  3. 检查防火墙设置");
    
    Serial.println("❌ 发送失败：");
    Serial.println("  1. 确保以换行符结束");
    Serial.println("  2. 检查网络连接");
    
    Serial.println("\n============================\n");
}
// ========== 【第五步：主程序】 ==========
/**
 * @brief 初始化函数
 * 解释：ESP32启动时自动执行一次，用于初始化
 */
void setup() {
    // 【步骤1】初始化串口通信
    Serial.begin(115200);
    delay(1000);  // 等待串口稳定（重要！）
    
    Serial.println("\n");
    Serial.println("╔══════════════════════════════════════╗");
    Serial.println("║     ESP32-S3 TCP服务器模板         ║");
    Serial.println("║      版本: 2.0 (详细版)            ║");
    Serial.println("╚══════════════════════════════════════╝");
    
    // 【步骤2】初始化TCP服务器
    tcpServer.begin();
    
    // 【步骤3】显示控制菜单
    showControlMenu();
    
    Serial.println("✅ 系统初始化完成！");
    Serial.println("💡 提示：");
    Serial.println("   1. 输入 'm' 显示控制菜单");
    Serial.println("   2. 输入 'h' 查看详细帮助");
    Serial.println("   3. 按上面说明连接客户端");
    Serial.println("\n🚀 等待客户端连接中...\n");
}
/**
 * @brief 主循环函数
 * 解释：ESP32不断重复执行此函数
 */
void loop() {
    // 【功能1】更新TCP服务器状态（必须调用）
    tcpServer.update();
    
    // 【功能2】检查串口输入（控制命令）
    if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n');
        input.trim();  // 去除首尾空白字符
        
        if (input.length() > 0) {
            char command = input.charAt(0);
            
            // 根据命令执行相应操作
            switch (command) {
                case '1':  // 显示服务器状态
                    Serial.println("> 显示服务器状态");
                    tcpServer.printServerStatus();
                    break;
                    
                case '2':  // 显示客户端列表
                    Serial.println("> 显示客户端列表");
                    tcpServer.printClientList();
                    break;
                    
                case '3':  // 广播测试消息
                    Serial.println("> 广播测试消息");
                    tcpServer.broadcastMessage("🔔 测试广播消息 [" + String(millis()) + "]");
                    break;
                    
                case '4':  // 发送到指定客户端
                    {
                        // 解析命令格式: 4 [ID] [消息]
                        int firstSpace = input.indexOf(' ', 2);
                        if (firstSpace > 0) {
                            int secondSpace = input.indexOf(' ', firstSpace + 1);
                            if (secondSpace > 0) {
                                int clientId = input.substring(2, firstSpace).toInt();
                                String message = input.substring(firstSpace + 1);
                                tcpServer.sendToClient(clientId, message);
                            }
                        }
                    }
                    break;
                    
                case '5':  // 重新初始化
                    Serial.println("> 重新初始化服务器");
                    tcpServer.begin();
                    break;
                    
                case '6':  // 显示连接说明
                    Serial.println("> 显示连接说明");
                    Serial.println("\n📱【快速连接指南】");
                    Serial.print("IP地址: ");
                    Serial.println(tcpServer.getServerIP());
                    Serial.print("端口: ");
                    Serial.print(MAIN_PORT);
                    Serial.print(" 或 ");
                    Serial.println(BACKUP_PORT);
                    break;
                    
                case 'm':  // 显示菜单
                    showControlMenu();
                    break;
                    
                case 'h':  // 显示详细帮助
                    showDetailedHelp();
                    break;
                    
                case 't':  // 发送测试数据
                    Serial.println("> 发送测试数据");
                    tcpServer.broadcastMessage("📡 服务器测试数据 [" + String(millis()) + "]");
                    break;
                    
                default:
                    Serial.print("❌ 未知命令: ");
                    Serial.println(input);
                    Serial.println("💡 输入 'm' 查看可用命令");
            }
        }
    }
    
    // 【功能3】非阻塞延迟（让其他任务有机会运行）
    delay(10);
}