/*
    Aluno: Lucas Carneiro de Araújo Lima
*/

#include <stdio.h>
#include <stdlib.h> 
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "hardware/irq.h" 
#include "hardware/pwm.h" 
#include "pico/bootrom.h"
#include "inc/ssd1306.h"
#include "inc/font.h"

// Pinagem dos componentes
#define I2C_PORT i2c1 // Define que o barramento I2C usado será o "i2c1"
#define I2C_SDA 14 // Define que o pino GPIO 14 será usado como SDA (linha de dados do I2C)
#define I2C_SCL 15 // Define que o pino GPIO 15 será usado como SCL (linha de clock do I2C)
#define address 0x3C // Define o endereço I2C do dispositivo (0x3C é o endereço padrão de muitos displays OLED SSD1306)
#define JOY_Y 26 // Eixo Y do joystick 
#define JOY_X 27 // Eixo X do joystick 
#define BTNJ 22 // Botão do joystick
#define BTNA 5 // Botão A
#define BTNB 6 // Botão B
#define RED 13 // LED vermelho
#define BLUE 12 // LED azul
#define GREEN 11 // LED verde

// Parâmetros do PWM 
/*
    fpwm = fckl / (di * (wrap + 1)), fpwm = 1KHz (Tpwm = 1ms)
    wrap = 4095, normalizar com os nívels ADC
    1KHz = 125Mhz/(div*(4095 + 1))
    1KHz = 30.52KHz/div
    1KHz*div = 30.52KHz
    div = 30.52KHz/1KHz
    div = 30.52
*/
#define WRAP 4095 // Wrap
#define DIV 30.52 // Divisor inteiro

bool gstate = false; // Estado do LED verde
bool rb_state = true; // Estado dos LEDs vermelho e azul
bool reset = false; // True para modo de gravação

static volatile uint32_t last_time = 0; // Armazena o tempo do último evento (em microssegundos)

void setup(); // Prototipação da função que define os LEDs RGB como saídas e os botões como entradas
void i2c_setup();
void pwm_setup();
void ssd1306_setup(ssd1306_t* ssd);
void gpio_irq_handler(uint gpio, uint32_t events);
void adc_setup();

// Retorna a leitura de um determinado canal ADC
uint16_t select_adc_channel(unsigned short int channel) {
    adc_select_input(channel);
    return adc_read();
}

int main() {
    ssd1306_t ssd;
    bool color = true;
    
    stdio_init_all(); // Inicialização dos recursos de entrada e saída padrão
    adc_setup(); // Inicialização e configuração dos pinos ADC
    setup(); // Inicialização e configuração dos LEDs e botões 
    i2c_setup(); // Inicialização e configuração da comunicação serial I2C 
    pwm_setup(); // Inicialização e configuração do PWM
    ssd1306_setup(&ssd); // Inicializa a estrutura do display
    
    gpio_set_irq_enabled_with_callback(BTNJ, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler); //Callback de interrupção do Botão do Joystick
    gpio_set_irq_enabled_with_callback(BTNA, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler); //Callback de interrupção do Botão A
    gpio_set_irq_enabled_with_callback(BTNB, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler); //Callback de interrupção do Botão B

    uint slice_blue = pwm_gpio_to_slice_num(BLUE); // Slice PWM do pino 12
    uint slice_red = pwm_gpio_to_slice_num(RED); // Slice PWM do pino 13

    while (true) {
        if(!reset) {
            uint16_t x = select_adc_channel(1); // Eixo X (0 - 4095).
            uint16_t y = select_adc_channel(0); // Eixo Y (0 - 4095).
    
            int refx = 60; // Referência do centro
            int refy = 28; // Referência do centro
    
            ssd1306_fill(&ssd, !color); // Limpa ou mostra a tela
            if(gstate) ssd1306_rect(&ssd, 4, 4, 120, 56, true, false);  // Retângulo extra
            ssd1306_rect(&ssd, 0, 0, 128, 64, color, !color); // Retângulo padrão
            ssd1306_draw_string(&ssd, "#", refx + ((x - 2048)*refx)/2048, refy + ((2048 - y)*refy)/2048); // Movimenta o quadrado ao longo da tela   
            ssd1306_send_data(&ssd); // Atualiza o display 
    
            pwm_set_enabled(slice_blue, rb_state); // Desliga ou liga o LED conforme o estado
            pwm_set_enabled(slice_red, rb_state); // Desliga ou liga o LED conforme o estado

            // Define o nível do PWM conforme o módulo do valor analógico normalizado por 2048. 
            (abs(x - 2048) > 40) ? pwm_set_gpio_level(RED, abs(x - 2048)) :  pwm_set_gpio_level(RED, 0); // Se o joystick não se deslocou longe o suficiente do neutro, mantém apagado
            (abs(y - 2048) > 40) ? pwm_set_gpio_level(BLUE, abs(y - 2048)) :  pwm_set_gpio_level(BLUE, 0); // Se o joystick não se deslocou longe o suficiente do neutro, mantém apagado
        } else {
            printf("Saindo para o modo de gravação...\n\n");

            ssd1306_fill(&ssd, false); // Limpa a tela
            ssd1306_draw_string(&ssd, "MODO DE", 28, 28); 
            ssd1306_draw_string(&ssd, "GRAVACAO", 24, 40);
            ssd1306_send_data(&ssd); // Envia os dados para o display

            reset_usb_boot(0,0); // Sai para o modo de gravação
        }
    }
}

// Inicializa e configura os LEDs RGB como saída. Inicializa e configura os botões como entradas.
void setup() {
    gpio_init(GREEN);
    gpio_set_dir(GREEN, GPIO_OUT);
    gpio_put(GREEN,false);
  
    gpio_init(BTNA);
    gpio_set_dir(BTNA, GPIO_IN);
    gpio_pull_up(BTNA);  
  
    gpio_init(BTNB);
    gpio_set_dir(BTNB, GPIO_IN);
    gpio_pull_up(BTNB);  

    gpio_init(BTNJ);
    gpio_set_dir(BTNJ, GPIO_IN);
    gpio_pull_up(BTNJ); 
}

// Inicializa e configura os pinos do joystick como periféricos ADC
void adc_setup() {
    adc_init();
    adc_gpio_init(JOY_Y);
    adc_gpio_init(JOY_X);
}

// Inicializa e configura a comunicação serial I2C 
void i2c_setup() {
    i2c_init(I2C_PORT, 4e2 * 1e3); // Inicialização I2C.
  
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Define a função do pino GPIO para I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Define a função do pino GPIO para I2C
    gpio_pull_up(I2C_SDA); // Pull up para linha de dados
    gpio_pull_up(I2C_SCL); // Pull up para linha de clock
}

// Limpa o display
void clear(ssd1306_t* ssd) {
    ssd1306_fill(ssd, false); // Limpa o display. O display inicia com todos os pixels apagados.
    ssd1306_send_data(ssd);
}

// Inicializa e configura o display
void ssd1306_setup(ssd1306_t* ssd) {
    ssd1306_init(ssd, WIDTH, HEIGHT, false, address, I2C_PORT); 
    ssd1306_config(ssd); 
    ssd1306_send_data(ssd);
    clear(ssd);

  
    ssd1306_draw_char(ssd, '#', 28, 60);  
    ssd1306_send_data(ssd);
}

// Inicializa e configura os pinos 13 e 12 como PWM
void pwm_setup() {
    gpio_set_function(BLUE, GPIO_FUNC_PWM); // Define o pino como PWM
    gpio_set_function(RED, GPIO_FUNC_PWM); // Define o pino como PWM

    uint slice_blue = pwm_gpio_to_slice_num(BLUE); // Obtém o slice
    uint slice_red = pwm_gpio_to_slice_num(RED); // Obtém o slice
    
    pwm_set_clkdiv(slice_blue, DIV); // Define o divisor inteiro de clock
    pwm_set_wrap(slice_blue, WRAP); // Define o wrap
    pwm_set_enabled(slice_blue, true);

    pwm_set_clkdiv(slice_red, DIV); // Define o divisor inteiro de clock
    pwm_set_wrap(slice_red, WRAP); // Define o wrap
    pwm_set_enabled(slice_red, true);
    
}

// Função de interrupção com debouncing
void gpio_irq_handler(uint gpio, uint32_t events) {
    uint32_t current_time = to_us_since_boot(get_absolute_time()); // Obtém o tempo atual em microssegundos
  
    if (current_time - last_time > 2e5) { // 200 ms de debouncing
        last_time = current_time; 
        if(gpio == BTNA) {
            rb_state = !rb_state;
            (rb_state) ? printf("LED PWM Ativado!!!\n\n")  : printf("LED PWM Desativado!!!\n\n");
        } else if(gpio == BTNJ) {
            gstate = !gstate;
            rb_state = !rb_state;
            (gstate) ? printf("LED verde ligado!!!\n\n")  : printf("LED verde desligado!!!\n\n");
            gpio_put(GREEN, gstate);
        } else if(gpio == BTNB) reset = true;
    }
}



