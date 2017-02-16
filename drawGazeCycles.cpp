#include <stdio.h>

#include "constants.h"
#include "drawGazeCycles.h"

int gazeXposition = ScreenWidth/2, gazeYposition = ScreenHeight/2;

void gazecycles(int eyeXposition, int eyeYposition){

    //gazeXposition:屏幕位置原坐标
    gazeXposition = gazeXposition + ScreenWidth /2 *(eyeXposition * 1.0 / EyeOffsetWidthMax);
    gazeYposition = gazeYposition + ScreenHeight /2 *(eyeYposition * 1.0 / EyeOffsetHeightMax);

    if (gazeXposition <= 0) {
        gazeXposition = 0;
    }
    if (gazeXposition >= ScreenWidth) {
        gazeXposition = ScreenWidth;
    }
    if (gazeYposition <= 0) {
        gazeYposition = 0;
    }
    if (gazeYposition >= ScreenHeight) {
        gazeYposition = ScreenHeight;
    }
    
    printf("gazeXposition = %d\n", gazeXposition);
    printf("gazeYposition = %d\n", gazeYposition);
}
