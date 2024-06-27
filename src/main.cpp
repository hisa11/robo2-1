// includeファイル
#include "mbed.h"
#include "PID.hpp"
#include "firstpenguin.hpp"

// firstpenguin_ID
constexpr uint32_t penguinID = 35;

// 変数宣言
int leftJoystickX = 0;
int leftJoystickY = 0;
int rightJoystickX = 0;
int16_t currentSpeed = 0;
int16_t currentSpeed1 = 0;
int targetSpeedRight = 0;
int targetSpeedLeft = 0;

// シリアル通信
BufferedSerial pc(USBTX, USBRX, 250000);
Timer timer;

// CAN通信
CAN can1(PA_11, PA_12, (int)1e6);
CAN can2(PB_12, PB_13, (int)1e6);
FirstPenguin penguin{penguinID, can2};
uint8_t DATA[8] = {};

// PID制御
const float kp = 0.4;
const float ki = 0.5;
const float kd = 0.001;
const float sampleTime = 0.02; // 20ms sample time

// PID制御器のインスタンスを作成
PID pidControllerRight(kp, ki, kd, sampleTime);
PID pidControllerLeft(kp, ki, kd, sampleTime);

void processInput(char *output_buf); // processInput関数のプロトタイプを宣言

// シリアル通信読み取りのプログラム
void readUntilPipe(char *output_buf, int output_buf_size)
{
    char buf[20];
    int output_buf_index = 0;
    while (1)
    {
        if (pc.readable())
        {
            ssize_t num = pc.read(buf, sizeof(buf) - 1); // -1 to leave space for null terminator
            buf[num] = '\0';
            for (int i = 0; i < num; i++)
            {
                if (buf[i] == '|')
                {
                    output_buf[output_buf_index] = '\0';
                    return;
                }
                else if (buf[i] != '\n' && output_buf_index < output_buf_size - 1)
                {
                    output_buf[output_buf_index++] = buf[i];
                }
            }
        }
        if (output_buf_index >= output_buf_size - 1) // Prevent buffer overflow
        {
            output_buf[output_buf_index] = '\0';
            return;
        }
    }
}

// CAN通信送信
void canSend()
{
    while (1)
    {
        float outputRight = pidControllerRight.calculate(targetSpeedRight, currentSpeed);
        float outputLeft = pidControllerLeft.calculate(targetSpeedLeft, currentSpeed1);

        penguin.send();

        int16_t outputRightInt16 = static_cast<int16_t>(outputRight);
        DATA[0] = outputRightInt16 >> 8;   // MSB
        DATA[1] = outputRightInt16 & 0xFF; // LSB

        int16_t outputLeftInt16 = static_cast<int16_t>(outputLeft);
        DATA[2] = outputLeftInt16 >> 8;   // MSB
        DATA[3] = outputLeftInt16 & 0xFF; // LSB

        CANMessage msg0(0x200, DATA, 8);
        can1.write(msg0);

        CANMessage msg1, msg2;
        if (can1.read(msg1) && msg1.id == 0x201)
        {
            currentSpeed = (msg1.data[2] << 8) | msg1.data[3];
        }
        if (can1.read(msg2) && msg2.id == 0x202)
        {
            currentSpeed1 = (msg2.data[2] << 8) | msg2.data[3];
        }
        printf("currentSpeed = %d , currentSpeed1 = %d\n", currentSpeed, currentSpeed1);
    }
}

int main()
{
    char output_buf[20]; // 出力用のバッファを作成します
    Thread thread;
    thread.start(canSend); // canSendスレッドを開始
    while (1)
    {
        readUntilPipe(output_buf, sizeof(output_buf)); // '|'が受け取られるまでデータを読み込みます
        processInput(output_buf);

        targetSpeedLeft = leftJoystickY + rightJoystickX;
        targetSpeedRight = -leftJoystickY + rightJoystickX;
    }
}
