#include <Arduino.h>
#ifndef SHELL_H
#define SHELL_H

#define SHELL_VERSION "v1.6" // 版本

#define SHELL_COMMAND_MAX_LENGTH 50   // shell命令最大长度
#define SHELL_PARAMETER_MAX_LENGTH 10 // shell命令参数最大长度
#define SHELL_PARAMETER_MAX_NUMBER 5  // shell命令参数最大数量

#define SHELL_HISTORY_MAX_NUMBER 5 // 历史命令记录数量

#define SHELL_COMMAND "\r\nletter>>" // shell提示符

class Shell
{
private:
    typedef uint32_t (Shell::*shellFunction)(uint8_t, uint8_t **);
    typedef struct
    {
        const char *name;       // shell命令名称
        shellFunction function; // shell命令函数
        const char *desc;       // shell命令描述
    } SHELL_CommandTypeDef;     // shell命令定义

    typedef enum
    {
        CONTROL_FREE = 0,
        CONTROL_STEP_ONE,
        CONTROL_STEP_TWO,
    } CONTROL_Status;

    uint8_t shellCommandBuff[SHELL_COMMAND_MAX_LENGTH] = {'\0'};                     // 命令缓存
    uint8_t shellCommandIndex = 0;                                                   // 命令缓存索引
    uint8_t commandPara[SHELL_PARAMETER_MAX_NUMBER][SHELL_PARAMETER_MAX_LENGTH];     // 命令参数缓冲区
    uint8_t commandCount = 0;                                                        // 命令计数
    uint8_t *commandPointer[SHELL_PARAMETER_MAX_NUMBER];                             // 命令参数指针
    uint8_t shellHistoryCommand[SHELL_HISTORY_MAX_NUMBER][SHELL_COMMAND_MAX_LENGTH]; // 历史命令缓冲区
    uint8_t shellHistoryCount = 0;                                                   // 已记录的历史命令数量
    int8_t shellHistoryFlag = 0;                                                     // 当前记录位置
    int8_t shellHistoryOffset = 0;                                                   // 偏移量，用于记录当前记录位置

    const SHELL_CommandTypeDef shellCommandList[5] = // 定义命令表
        {
            /*command               function                description*/
            {"letter", &Shell::shellLetter, "letter shell"},
            {"reboot", &Shell::shellReboot, "reboot system"},
            {"help", &Shell::shellShowCommandList, "show command list"},
            {"clear", &Shell::shellClear, "clear command line"},
            {"paraTest", &Shell::shellParaTest, "test parameter"},
    };

private:
    void shellHandler(uint8_t receiveData);
    void shellBackspace(uint8_t length);
    uint32_t shellStringCopy(char *dest, const char *src);
    uint32_t shellLetter(uint8_t commandCount, uint8_t *commandPointer[]);
    uint32_t shellReboot(uint8_t commandCount, uint8_t *commandPointer[]);
    uint32_t shellShowCommandList(uint8_t commandCount, uint8_t *commandPointer[]);
    uint32_t shellClear(uint8_t commandCount, uint8_t *commandPointer[]);
    uint32_t shellParaTest(uint8_t argc, uint8_t *argv[]);

public:
    Shell();
    void init(uint32_t baud);
    void start(void);
    void loop(void);
};

#endif // SHELL_H
