// controller.cpp
#include <cstring>
#include <cstdio>
#include <cstdlib>

void processInput(char* inputData) {
    int leftJoystickX = 0;
    int leftJoystickY = 0;
    int rightJoystickX = 0;

    if (strncmp(inputData, "L3_x:", 5) == 0) // "L3_x:"という文字列で始まるかどうかを確認します
    {
        char *dataPointer = inputData + 5; // "L3_x:"の後の文字列の先頭ポインタを取得
        // 数字部分を読み取る
        leftJoystickX = atoi(dataPointer);
        // 数字を使って何かをする（ここでは単にPCに出力）
        printf("Left Joystick X: %d\n", leftJoystickX);
    }
    else if (strncmp(inputData, "L3_y:", 5) == 0) // "L3_y:"という文字列で始まるかどうかを確認します
    {
        char *dataPointer = inputData + 5; // "L3_y:"の後の文字列の先頭ポインタを取得
        // 数字部分を読み取る
        leftJoystickY = atoi(dataPointer);
        // 数字を使って何かをする（ここでは単にPCに出力）
        printf("Left Joystick Y: %d\n", leftJoystickY);
    }
    else if (strncmp(inputData, "R3_x:", 5) == 0) // "R3_x:"という文字列で始まるかどうかを確認します
    {
        char *dataPointer = inputData + 5; // "R3_x:"の後の文字列の先頭ポインタを取得
        // 数字部分を読み取る
        rightJoystickX = atoi(dataPointer);
        // 数字を使って何かをする（ここでは単にPCに出力）
        printf("Right Joystick X: %d\n", rightJoystickX);
    }
}