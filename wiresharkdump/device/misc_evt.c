/**
 ***********************************************************************************************************************
 * Copyright (c) 2021, China Mobile Communications Group Co.,Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with 
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 *
 * @file        misc_evt.c
 *
 * @brief       Create a task to deal some misc funcs.
 *
 * @details
 *
 * @revision
 * Date          Author          Notes
 * 2021-06-07    OneOS Team      First Version
 ***********************************************************************************************************************
 */
#include <stdint.h>
#include <stdlib.h>
#include <kernel.h>
#include "misc_evt.h"

#define MQ_MAX_MSG 16
#define MSG_SIZE sizeof(misc_evt_t)
#define MQ_POLL_SIZE (MQ_MAX_MSG * MSG_SIZE)
static char __aligned(4) s_mq_pool[MQ_POLL_SIZE];
static struct k_msgq s_mq;

#define MISC_EVT_STACK_SIZE 512
K_THREAD_STACK_DEFINE(misc_evt_stack, MISC_EVT_STACK_SIZE);
static struct k_thread s_thread;

struct k_msgq *misc_evt_mq_get(void)
{
    return &s_mq;
}

static void misc_evt_deal(void *p0, void *p1, void *p2)
{
    misc_evt_t recv_evt;
    while (1)
    {
        k_msgq_get(&s_mq, &recv_evt, K_FOREVER);
        recv_evt.handle(recv_evt.arg);
    }
}

int misc_evt_init(void)
{
    k_msgq_init(&s_mq, s_mq_pool, MSG_SIZE, MQ_MAX_MSG);

    k_tid_t my_tid = k_thread_create(&s_thread, misc_evt_stack,
                                     K_THREAD_STACK_SIZEOF(misc_evt_stack),
                                     misc_evt_deal,
                                     NULL,
                                     NULL,
                                     NULL,
                                     4,
                                     0,
                                     K_NO_WAIT);

    return 0;
}
