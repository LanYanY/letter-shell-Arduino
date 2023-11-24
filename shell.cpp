#include "Shell.h"
Shell::Shell()
{
}

void Shell::init(uint32_t baud)
{
    Serial.begin(baud);
    Serial.print("\r\n\r\n");
    Serial.print("+=========================================================+\r\n");
    Serial.print("|                (C) COPYRIGHT 2018 Letter                |\r\n");
    Serial.print("|                    Letter shell " SHELL_VERSION "                    |\r\n");
    Serial.print("|               Build: "__DATE__
                 " "__TIME__
                 "               |\r\n");
    Serial.print("+=========================================================+\r\n");
    Serial.print(SHELL_COMMAND);
    for (int i = 0; i < SHELL_PARAMETER_MAX_NUMBER; i++)
    {
        commandPointer[i] = commandPara[i];
    }
}

void Shell::loop()
{
    uint8_t receiveData;
    if (Serial.available())
    {
        receiveData = Serial.read();
        shellHandler(receiveData);
    }
}
void Shell::shellHandler(uint8_t receiveData)
{
    static uint8_t runFlag;
    static CONTROL_Status controlFlag = CONTROL_FREE;
    switch (receiveData)
    {
    case '\r':
    case '\n':
        if (shellCommandIndex >= SHELL_COMMAND_MAX_LENGTH - 1)
        {
            Serial.write("\r\nError: Command is too long\r\n");
            shellCommandBuff[shellCommandIndex] = 0;
            shellCommandIndex = 0;
            Serial.write(SHELL_COMMAND);
            break;
        }
        if (shellCommandIndex == 0)
        {
            Serial.write(SHELL_COMMAND);
            break;
        }
        else
        {
            shellCommandBuff[shellCommandIndex++] = 0; // 去除最后一个空格
            commandCount = 0;                          // 记录参数个数

            uint8_t j = 0;
            for (int8_t i = 0; i < shellCommandIndex; i++)
            {
                if (shellCommandBuff[i] != ' ' &&  // 不是空格
                    shellCommandBuff[i] != '\t' && // 不是tab
                    shellCommandBuff[i] != 0)      // 不是最后一个字符
                {
                    commandPara[commandCount][j++] = shellCommandBuff[i]; // 记录参数
                }
                else
                {
                    if (j != 0) // 不是空字符
                    {
                        commandPara[commandCount][j] = 0; // 结束参数
                        commandCount++;                   // 记录参数个数
                        j = 0;                            // 重置参数长度
                    }
                }
            }
            shellCommandIndex = 0; // 清空命令

            if (commandCount == 0) // 是否为无效指令
            {
                Serial.write(SHELL_COMMAND);
                break;
            }

            shellStringCopy((char *)shellHistoryCommand[shellHistoryFlag++], (const char *)shellCommandBuff); // 记录历史命令
            if (++shellHistoryCount > SHELL_HISTORY_MAX_NUMBER)                                               // 历史命令记录数量限制
            {
                shellHistoryCount = SHELL_HISTORY_MAX_NUMBER; // 限制历史命令数量
            }
            if (shellHistoryFlag >= SHELL_HISTORY_MAX_NUMBER) // 历史命令记录数量限制
            {
                shellHistoryFlag = 0; // 重置历史命令记录标志
            }
            shellHistoryOffset = 0; // 重置历史命令记录偏移量

            Serial.write("\r\n"); // 回车换行
            runFlag = 0;          // 重置运行标志

            for (int8_t i = sizeof(shellCommandList) / sizeof(SHELL_CommandTypeDef) - 1;
                 i >= 0; i--) // 逆序遍历命令列表
            {
                if (strcmp((const char *)commandPara[0],                 // 命令名
                           (const char *)shellCommandList[i].name) == 0) // 匹配命令名
                {
                    runFlag = 1;                                                         // 运行标志置1
                    (this->*shellCommandList[i].function)(commandCount, commandPointer); // 执行命令
                    break;
                }
            }

            if (runFlag == 0) // 未找到命令
            {
                Serial.write("Command not found"); // 未找到命令
            }
        }
        Serial.write(SHELL_COMMAND); // 回到命令提示符
        break;
    case 0x08: // 退格
        if (shellCommandIndex != 0)
        {
            shellCommandIndex--;
            this->shellBackspace(1);
        }
        break;
    case '\t': // 制表符
        if (shellCommandIndex != 0)
        {
            for (int8_t i = sizeof(shellCommandList) / sizeof(SHELL_CommandTypeDef) - 1;
                 i >= 0; i--)
            {
                if (strncmp((const char *)shellCommandBuff,
                            (const char *)shellCommandList[i].name, shellCommandIndex) == 0)
                {
                    this->shellBackspace(shellCommandIndex);
                    shellCommandIndex = this->shellStringCopy((char *)shellCommandBuff, (const char *)shellCommandList[i].name);
                    Serial.print((const char *)shellCommandBuff);
                }
            }
        }
        else // 无历史命令，输入help
        {
            this->shellBackspace(shellCommandIndex);
            shellCommandIndex = 4;
            this->shellStringCopy((char *)shellCommandBuff, (const char *)"help");
            Serial.print((const char *)shellCommandBuff);
        }
        break;
    case 0x1B: // ESC
        controlFlag = CONTROL_STEP_ONE;
        break;
    default:
        switch ((uint8_t)controlFlag)
        {
        case CONTROL_STEP_TWO:
            if (receiveData == 0x41) // 方向上键
            {

                shellBackspace(shellCommandIndex);
                if (shellHistoryOffset-- <= -((shellHistoryCount > shellHistoryFlag)
                                                  ? shellHistoryCount
                                                  : shellHistoryFlag))
                {
                    shellHistoryOffset = -((shellHistoryCount > shellHistoryFlag)
                                               ? shellHistoryCount
                                               : shellHistoryFlag);
                }
                shellCommandIndex = shellStringCopy((char *)shellCommandBuff,
                                                    (const char *)shellHistoryCommand[(shellHistoryFlag + SHELL_HISTORY_MAX_NUMBER + shellHistoryOffset) % SHELL_HISTORY_MAX_NUMBER]);
                Serial.print((const char *)shellCommandBuff);
            }
            else if (receiveData == 0x42) // 方向下键
            {

                if (++shellHistoryOffset >= 0)
                {
                    shellHistoryOffset = -1;
                    break;
                }
                shellBackspace(shellCommandIndex);
                shellCommandIndex = shellStringCopy((char *)shellCommandBuff,
                                                    (const char *)shellHistoryCommand[(shellHistoryFlag + SHELL_HISTORY_MAX_NUMBER + shellHistoryOffset) % SHELL_HISTORY_MAX_NUMBER]);
                Serial.print((const char *)shellCommandBuff);
            }
            else if (receiveData == 0x43) // 方向右键
            {
                // Serial.print("right\r\n");
            }
            else if (receiveData == 0x44) // 方向左键
            {
                // Serial.print("left\r\n");
            }
            else
            {
                controlFlag = CONTROL_FREE;
                goto normal;
            }
            break;

        case CONTROL_STEP_ONE:
            if (receiveData == '[') // 0x58
            {
                controlFlag = CONTROL_STEP_TWO;
            }
            else
            {
                controlFlag = CONTROL_FREE;
                goto normal;
            }
            break;

        case CONTROL_FREE: // 正常按键处理
        normal:
            if (shellCommandIndex < SHELL_COMMAND_MAX_LENGTH - 1)
            {
                shellCommandBuff[shellCommandIndex++] = receiveData;
                Serial.write(receiveData);
            }
            else
            {
                shellCommandIndex++;
                Serial.write(receiveData);
            }
            break;
        }
        break;
    }
}
void Shell::shellBackspace(uint8_t length)
{
    while (length--)
    {
        Serial.write(0x08); //"\b \b"
        Serial.write(' ');
        Serial.write(0x08);
    }
}
uint32_t Shell::shellLetter(uint8_t commandCount, uint8_t *commandPointer[])
{
    Serial.print("Letter shell " SHELL_VERSION "\r\n");
    Serial.print("(C) Copyright 2018 Leter, All Right Reserved");
    return 0;
}
uint32_t Shell::shellReboot(uint8_t commandCount, uint8_t *commandPointer[])
{
    this->shellClear(commandCount, commandPointer);
    delay(5);
    HAL_NVIC_SystemReset();
    return 0;
}
uint32_t Shell::shellShowCommandList(uint8_t commandCount, uint8_t *commandPointer[])
{
    int32_t spaceLength;

    Serial.print("COMMAND                 FUNCTION\r\n\r\n");

    for (uint8_t i = 0; i < sizeof(shellCommandList) / sizeof(SHELL_CommandTypeDef); i++)
    {
        spaceLength = 24 - (int32_t)Serial.print(shellCommandList[i].name);
        spaceLength = (spaceLength > 0) ? spaceLength : 4;
        do
        {
            Serial.print(" ");
        } while (--spaceLength);
        Serial.print(shellCommandList[i].desc);
        Serial.print("\r\n");
    }
    return 0;
}
uint32_t Shell::shellClear(uint8_t commandCount, uint8_t *commandPointer[])
{
    Serial.print("\033[2J\033[1H");
    return 0;
}
uint32_t Shell::shellParaTest(uint8_t argc, uint8_t *argv[])
{
    printf("%d parameter(s)\r\n", argc);

    if (argc == 1)
    {
        return 0;
    }
    while (--argc > 0)
    {
        Serial.print((const char *)*++argv);
        Serial.print("\r\n");
    }
    return 0;
}
uint32_t Shell::shellStringCopy(char *dest, const char *src)
{
    return strlen(strcpy(dest, src));
}