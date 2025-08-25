//
// Created by kw on 25-7-16.
//

#ifndef ENTRYPOINT_H
#define ENTRYPOINT_H

#include "JFMEngine/Core/Application.h"
#include "JFMEngine/Utils/Log.h"

// 引擎提供的主函数实现
extern JFM::Application* JFM::CreateApplication();

int main(int argc, char** argv) {
    // 初始化日志系统
    JFM::Log::Initialize();

    // 创建用户应用程序实例
    auto app = JFM::CreateApplication();

    // 运行应用程序
    app->Run();

    // 清理资源
    delete app;

    return 0;
}

#endif //ENTRYPOINT_H
