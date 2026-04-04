#include "driver/mcpwm_prelude.h"

#define LM35_B 17 //A1
#define LM35_A 16 //A2

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

int currentAddr = 0;
bool channelAON = false;
bool channelBON = false;
bool channelAHeat = false;
bool channelBHeat = false;

float channelARefTemp = 0;
float channelBRefTemp = 0;

int stateA = -1; // -1=Default to Start, 0=OFF, 1=HEAT, 2=COOL
int stateB = -1;

int currentVal = 0;
unsigned long lastWriteTime = 0;
const long interval = 1000; // 1 seconds
bool toggle = false;

mcpwm_timer_handle_t timerA = NULL, timerB = NULL;
mcpwm_oper_handle_t oper1A = NULL, oper2A = NULL, oper1B = NULL, oper2B = NULL;
mcpwm_cmpr_handle_t cmpr1A = NULL, cmpr2A = NULL, cmpr1B = NULL, cmpr2B = NULL;
mcpwm_gen_handle_t g1AH, g1AL, g2AH, g2AL, g1BH, g1BL, g2BH, g2BL;

const uint32_t period = 250; 

void setup() {
    Serial.begin(115200);
    Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2); 
    analogSetAttenuation(ADC_11db);

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
    mcpwm_comparator_set_compare_value(cmpr1A, (duty * period) / 100);
    mcpwm_generator_set_force_level(g1AH, -1, true); 
    mcpwm_generator_set_force_level(g2AL, 1, true);  
}

void peltierA_Heat(int duty) {
    mcpwm_comparator_set_compare_value(cmpr2A, (duty * period) / 100);
    mcpwm_generator_set_force_level(g2AH, -1, true); 
    mcpwm_generator_set_force_level(g1AL, 1, true);  
}

void peltierB_OFF() {
    mcpwm_generator_set_force_level(g1BH, 0, true); mcpwm_generator_set_force_level(g1BL, 0, true);
    mcpwm_generator_set_force_level(g2BH, 0, true); mcpwm_generator_set_force_level(g2BL, 0, true);
}

void peltierB_Cool(int duty) {
    mcpwm_comparator_set_compare_value(cmpr1B, (duty * period) / 100);
    mcpwm_generator_set_force_level(g1BH, -1, true); 
    mcpwm_generator_set_force_level(g2BL, 1, true);  
}

void peltierB_Heat(int duty) {
    mcpwm_comparator_set_compare_value(cmpr2B, (duty * period) / 100);
    mcpwm_generator_set_force_level(g2BH, -1, true); 
    mcpwm_generator_set_force_level(g1BL, 1, true);  
}

bool readDWIN(int *addr, int *val) {
  if (Serial2.available() >= 3) {
    if (Serial2.read() == 0x5A) {
      if (Serial2.peek() == 0xA5) {
        Serial2.read();
        
        byte len = Serial2.read();
        byte frame[len];

        // Fill frame array
        for (int i = 0; i < len; i++) {
          uint32_t startWait = millis();
          while (Serial2.available() == 0) {
            if (millis() - startWait > 50) return false;
          }
          frame[i] = Serial2.read();
        }

        if (frame[0] == 0x83) {
          *addr = (frame[1] << 8) | frame[2];
          *val  = (frame[4] << 8) | frame[5];  
          return true;
        }
      }
    }
  }
  return false;
}

void writeDWINLong(int addr, long value) {
  byte frame[10]; 
  
  frame[0] = 0x5A;            
  frame[1] = 0xA5;            
  frame[2] = 0x07;           
  frame[3] = 0x82;            
  frame[4] = highByte(addr);  
  frame[5] = lowByte(addr);   
  
  frame[6] = (value >> 24) & 0xFF; 
  frame[7] = (value >> 16) & 0xFF; 
  frame[8] = (value >> 8)  & 0xFF;
  frame[9] = (value)       & 0xFF; 
  
  Serial2.write(frame, 10); 
  
  //Serial.printf("Sent Long to 0x%04X: %ld\n", addr, value);
}

float getTemp(int pin) {
  float sum = 0;
  for(int i = 0; i < 20; i++) {
    sum += (analogRead(pin) * 3.3 / 4095.0) * 100.0;
  }
  return sum / 20.0;
}

void readInputState(){
        if (readDWIN(&currentAddr, &currentVal)) {
        Serial.printf("Addr: 0x%04X | Val: %d\n", currentAddr, currentVal);
        
        if (currentAddr == 0x6500 && currentVal == 1) {
            Serial.println("Turn ON Channel A");
            channelAON = true;
        }
        else if (currentAddr == 0x6500 && currentVal == 0) {
            Serial.println("Turn OFF Channel A");
            channelAON = false;
        }

        if (currentAddr == 0x6700 && currentVal == 1) {
            Serial.println("Turn ON Channel B");
            channelBON = true;
        }
        else if (currentAddr == 0x6700 && currentVal == 0) {
            Serial.println("Turn OFF Channel B");
            channelBON = false;
        }

        if (currentAddr == 0x6600 && currentVal == 1) {
            Serial.println("Heat Channel A");
            channelAHeat = true;
        }
        else if (currentAddr == 0x6600 && currentVal == 0) {
            Serial.println("Cool Channel A");
            channelAHeat = false;
        }

        if (currentAddr == 0x6800 && currentVal == 1) {
            Serial.println("Heat Channel B");
            channelBHeat = true;
        }
        else if (currentAddr == 0x6800 && currentVal == 0) {
            Serial.println("Cool Channel B");
            channelBHeat = false;
        }
        if (currentAddr == 0x2000) {
            Serial.print("Set Temperature of Channel A to ");
            Serial.print(currentVal);
            Serial.println();
            channelARefTemp = currentVal/10.0;
            writeDWINLong(0x5500, currentVal);
            delay(50);
        }
        if (currentAddr == 0x2100) {
            Serial.print("Set Temperature of Channel B to ");
            Serial.print(currentVal);
            Serial.println();
            channelBRefTemp = currentVal/10.0;
            writeDWINLong(0x5700, currentVal);
            delay(50);
        }
    }
}

void updateTemp(){
    float channelATemp = getTemp(LM35_A);
    float channelBTemp = getTemp(LM35_B);

    if (millis() - lastWriteTime >= interval) {
        lastWriteTime = millis();
        writeDWINLong(0x5600, (long)(channelATemp * 10));
        delay(50);
        writeDWINLong(0x5800, (long)(channelBTemp * 10));
    }
}

void loop() {
//NOTE FOR JAWAD: Always have both peltier OFF before turning ON/OFF a peltier or switching from heat/cool
    // peltierA_OFF(); peltierB_OFF();
    //peltierA_Cool(90);
    //peltierB_Cool(90);
    //delay(60000);

    updateTemp();
    readInputState();

    if (channelAON){
        Serial.printf("Ref: %.2f, Actual: %.2f\n", channelARefTemp, getTemp(LM35_A));
        if (channelAHeat){
            if (channelARefTemp > getTemp(LM35_A)+0.5){
                peltierA_Heat(90);
            }
            else{
                peltierA_OFF();
            }
            
        }
        else{
            if (channelARefTemp < getTemp(LM35_A)-0.5){
                peltierA_Cool(90);
            }
            else{
                peltierA_OFF();
            }
        }
    }
    else{
        peltierA_OFF();
    }


    if (channelBON){
        Serial.printf("Channel B Ref: %.2f, Actual: %.2f\n", channelBRefTemp, getTemp(LM35_B));
        if (channelBHeat){
            if (channelBRefTemp > getTemp(LM35_B)+0.5){
                peltierB_Heat(90);
            }
            else{
                peltierB_OFF();
            }
            
        }
        else{
            if (channelBRefTemp < getTemp(LM35_B)-0.5){
                peltierB_Cool(90);
            }
            else{
                peltierB_OFF();
            }
        }
    }
    else{
        peltierB_OFF();
    }
}
