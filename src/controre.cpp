// controller.cpp
#include <cstring>
#include <cstdio>
#include <cstdlib>

void processInput(char* output_buf) {
    int value_lx = 0;
    int value_ly = 0;
    int value_rx = 0;

    if (strncmp(output_buf, "L3_x:", 5) == 0) // "L3_x:"という文字列で始まるかどうかを確認します
    {
        char *ptr = output_buf + 5; // "L3_x:"の後の文字列の先頭ポインタを取得
        // 数字部分を読み取る
        value_lx = atoi(ptr);
        // 数字を使って何かをする（ここでは単にPCに出力）
        printf("Lx: %d\n", value_lx);
    }
    else if (strncmp(output_buf, "L3_y:", 5) == 0) // "L3_y:"という文字列で始まるかどうかを確認します
    {
        char *ptr = output_buf + 5; // "L3_y:"の後の文字列の先頭ポインタを取得
        // 数字部分を読み取る
        value_ly = atoi(ptr);
        // 数字を使って何かをする（ここでは単にPCに出力）
        printf("Ly: %d\n", value_ly);
    }
    else if (strncmp(output_buf, "R3_x:", 5) == 0) // "R3_x:"という文字列で始まるかどうかを確認します
    {
        char *ptr = output_buf + 5; // "R3_x:"の後の文字列の先頭ポインタを取得
        // 数字部分を読み取る
        value_rx = atoi(ptr);
        // 数字を使って何かをする（ここでは単にPCに出力）
        printf("Rx: %d\n", value_rx);
    }
}