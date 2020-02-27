//
// Created by 梅峰源 on 2020-02-27.
//

#include "extension.h"
#include "gameplay.h"

#include "zmarket.h"

bool gameplay::SDK_OnLoad(char *error, size_t maxlength, bool late) {

    zmarket::Init();

    return true;
}

void gameplay::SDK_OnUnload() {

}
