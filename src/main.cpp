// includeファイル
#include "mbed.h"
#include "PID.hpp"
#include "firstpenguin.hpp"

// firstpenguin_ID
constexpr uint32_t can_id = 35;

// 変数宣言
int value_lx = 0;
int value_ly = 0;
int value_rx = 0;
int16_t sokudo = 0;
int16_t sokudo1 = 0;
int mokuhyou_r = 0;
int mokuhyou_l = 0;

// シリアル通信
BufferedSerial pc(USBTX, USBRX, 250000);
Timer timer;

// CAN通信
CAN can1(PA_11, PA_12, (int)1e6);
CAN can2(PB_12, PB_13, (int)1e6);
FirstPenguin penguin{can_id, can2};
uint8_t DATA[8] = {};

// PID制御
const float kp = 0.4;
const float ki = 0.5;
const float kd = 0.001;
const float sample_time = 0.02; // 20ms sample time

// PID制御器のインスタンスを作成
PID pid_controller(kp, ki, kd, sample_time);
PID pid_controller1(kp, ki, kd, sample_time);

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
                { // 改行文字を無視します
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

// can
void can_send()
{
    while (1)
    {
        float output = pid_controller.calculate(mokuhyou_r, sokudo);
        float output1 = pid_controller1.calculate(mokuhyou_l, sokudo1);

        penguin.send();

        int16_t output_int16 = static_cast<int16_t>(output);
        DATA[0] = output_int16 >> 8;   // MSB
        DATA[1] = output_int16 & 0xFF; // LSB

        int16_t output1_int16 = static_cast<int16_t>(output1);
        DATA[2] = output1_int16 >> 8;   // MSB
        DATA[3] = output1_int16 & 0xFF; // LSB

        CANMessage msg0(0x200, DATA, 8);
        can1.write(msg0);

        CANMessage msg1, msg2;
        if (can1.read(msg1) && msg1.id == 0x201)
        { // CAN ID 1のメッセージをチェック
            // msg.dataを適切な変数に格納

            sokudo = (msg1.data[2] << 8) | msg1.data[3];
            // dataValueを使用した処理...
        }
        if (can1.read(msg2) && msg2.id == 0x202)
        {
            // int8_t h_rank = msg2.data[2];
            // int8_t l_rank = msg2.data[3];
            sokudo1 = (msg2.data[2] << 8) | msg2.data[3];
        }
        printf("sokudo = %d , sokudo1 = %d\n", sokudo, sokudo1);

        // printf("pwm[2]: %d\n", penguin.pwm[2]);
    }
}

int main()
{
    char output_buf[20]; // 出力用のバッファを作成します
    Thread thread;
    thread.start(can_send); // can_sendスレッドを開始
    while (1)
    {
        readUntilPipe(output_buf, sizeof(output_buf)); // '|'が受け取られるまでデータを読み込みます
        processInput(output_buf);

        mokuhyou_l = value_ly+value_rx;
        mokuhyou_r = -value_ly+value_rx;
    }
}