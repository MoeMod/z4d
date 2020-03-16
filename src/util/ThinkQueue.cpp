//
// Created by 梅峰源 on 2020-03-16.
//

#include <utility>
#include "ThinkQueue.h"

void ThinkQueue::AddTask(ThinkQueue::BaseTask *task) {

    BaseTask *old_head = head.load();
    do
        task->next = old_head;
    while(!head.compare_exchange_weak(old_head, task));
}

void ThinkQueue::CallAndClear() {
    // 获取并清空旧的链表
    BaseTask *cur_head = head.exchange(nullptr);
    while(cur_head != nullptr) {
        (*cur_head)();
        delete std::exchange(cur_head, cur_head->next);
    }
}