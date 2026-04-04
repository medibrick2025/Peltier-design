#include "driver/mcpwm_prelude.h"

#define RXD2 38
#define TXD2 39


// Channel/Bridge A
#define A_L1_H 11 
#define A_L1_L 12
#define A_L2_H 18
#define A_L2_L 13

// Channel/Bridge B
#define B_L1_H 5
#define B_L1_L 6
#define B_L2_H 10
#define B_L2_L 9

mcpwm_timer_handle_t timerA = NULL, timerB = NULL;
mcpwm_oper_handle_t oper1A = NULL, oper2A = NULL, oper1B = NULL, oper2B = NULL;
mcpwm_cmpr_handle_t cmpr1A = NULL, cmpr2A = NULL, cmpr1B = NULL, cmpr2B = NULL;
mcpwm_gen_handle_t g1AH, g1AL, g2AH, g2AL, g1BH, g1BL, g2BH, g2BL;

const uint32_t period = 250; 

void setup() {
    Serial.begin(115200);
    Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2); 

    // Timer Setup
    mcpwm_timer_config_t timer_conf = {
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .resolution_hz = 1000000, 
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
        .period_ticks = period, 
    };
    
    timer_conf.group_id = 0; mcpwm_new_timer(&timer_conf, &timerA);
    timer_conf.group_id = 1; mcpwm_new_timer(&timer_conf, &timerB);

    mcpwm_comparator_config_t cmpr_conf = {.flags = {.update_cmp_on_tez = true}};
    mcpwm_generator_config_t gen_conf = {};

    // --- H-BRIDGE A ---
    mcpwm_operator_config_t oper_confA = {.group_id = 0};
    mcpwm_new_operator(&oper_confA, &oper1A);
    mcpwm_new_operator(&oper_confA, &oper2A);
    mcpwm_operator_connect_timer(oper1A, timerA);
    mcpwm_operator_connect_timer(oper2A, timerA);
    
    mcpwm_new_comparator(oper1A, &cmpr_conf, &cmpr1A);
    mcpwm_new_comparator(oper2A, &cmpr_conf, &cmpr2A);

    gen_conf.gen_gpio_num = A_L1_H; mcpwm_new_generator(oper1A, &gen_conf, &g1AH);
    gen_conf.gen_gpio_num = A_L1_L; mcpwm_new_generator(oper1A, &gen_conf, &g1AL);
    gen_conf.gen_gpio_num = A_L2_H; mcpwm_new_generator(oper2A, &gen_conf, &g2AH);
    gen_conf.gen_gpio_num = A_L2_L; mcpwm_new_generator(oper2A, &gen_conf, &g2AL);

    // --- H-BRIDGE B  ---
    mcpwm_operator_config_t oper_confB = {.group_id = 1};
    mcpwm_new_operator(&oper_confB, &oper1B);
    mcpwm_new_operator(&oper_confB, &oper2B);
    mcpwm_operator_connect_timer(oper1B, timerB);
    mcpwm_operator_connect_timer(oper2B, timerB);

    mcpwm_new_comparator(oper1B, &cmpr_conf, &cmpr1B);
    mcpwm_new_comparator(oper2B, &cmpr_conf, &cmpr2B);

    gen_conf.gen_gpio_num = B_L1_H; mcpwm_new_generator(oper1B, &gen_conf, &g1BH);
    gen_conf.gen_gpio_num = B_L1_L; mcpwm_new_generator(oper1B, &gen_conf, &g1BL);
    gen_conf.gen_gpio_num = B_L2_H; mcpwm_new_generator(oper2B, &gen_conf, &g2BH);
    gen_conf.gen_gpio_num = B_L2_L; mcpwm_new_generator(oper2B, &gen_conf, &g2BL);

    // Generate PWM
    mcpwm_generator_set_action_on_timer_event(g1AH, MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH));
    mcpwm_generator_set_action_on_compare_event(g1AH, MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, cmpr1A, MCPWM_GEN_ACTION_LOW));
    mcpwm_generator_set_action_on_timer_event(g2AH, MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH));
    mcpwm_generator_set_action_on_compare_event(g2AH, MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, cmpr2A, MCPWM_GEN_ACTION_LOW));

    mcpwm_generator_set_action_on_timer_event(g1BH, MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH));
    mcpwm_generator_set_action_on_compare_event(g1BH, MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, cmpr1B, MCPWM_GEN_ACTION_LOW));
    mcpwm_generator_set_action_on_timer_event(g2BH, MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH));
    mcpwm_generator_set_action_on_compare_event(g2BH, MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, cmpr2B, MCPWM_GEN_ACTION_LOW));

    mcpwm_timer_enable(timerA);
    mcpwm_timer_start_stop(timerA, MCPWM_TIMER_START_NO_STOP);
    mcpwm_timer_enable(timerB);
    mcpwm_timer_start_stop(timerB, MCPWM_TIMER_START_NO_STOP);

    peltierA_OFF();
    peltierB_OFF();

    //peltierB_Cool(90); // TEST RUN JD
}
void peltierA_OFF() {
    mcpwm_generator_set_force_level(g1AH, 0, true); mcpwm_generator_set_force_level(g1AL, 0, true);
    mcpwm_generator_set_force_level(g2AH, 0, true); mcpwm_generator_set_force_level(g2AL, 0, true);
}

void peltierA_Cool(int duty) {
    peltierA_OFF();
    mcpwm_comparator_set_compare_value(cmpr1A, (duty * period) / 100);
    mcpwm_generator_set_force_level(g1AH, -1, true); 
    mcpwm_generator_set_force_level(g2AL, 1, true);  
}

void peltierA_Heat(int duty) {
    peltierA_OFF();
    mcpwm_comparator_set_compare_value(cmpr2A, (duty * period) / 100);
    mcpwm_generator_set_force_level(g2AH, -1, true); 
    mcpwm_generator_set_force_level(g1AL, 1, true);  
}

void peltierB_OFF() {
    mcpwm_generator_set_force_level(g1BH, 0, true); mcpwm_generator_set_force_level(g1BL, 0, true);
    mcpwm_generator_set_force_level(g2BH, 0, true); mcpwm_generator_set_force_level(g2BL, 0, true);
}

void peltierB_Cool(int duty) {
    peltierB_OFF();
    mcpwm_comparator_set_compare_value(cmpr1B, (duty * period) / 100);
    mcpwm_generator_set_force_level(g1BH, -1, true); 
    mcpwm_generator_set_force_level(g2BL, 1, true);  
}

void peltierB_Heat(int duty) {
    peltierB_OFF();
    mcpwm_comparator_set_compare_value(cmpr2B, (duty * period) / 100);
    mcpwm_generator_set_force_level(g2BH, -1, true); 
    mcpwm_generator_set_force_level(g1BL, 1, true);  
}

void loop() {

     //peltierA_OFF(); //peltierB_OFF();
    // peltierA_Cool(90);
     //peltierA_Heat(90);
     peltierB_Cool(90);
     delay(60000);
}
