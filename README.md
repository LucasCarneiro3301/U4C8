# U4C8
Repositório para a atividade da unidade 4 e capítulo 8: Conversores A/D

__Aluno:__
Lucas Carneiro de Araújo Lima

## ATIVIDADE 
__Descrição:__
O projeto consiste no controle de LEDs RGB e na movimentação de um quadrado em um _display_ SSD1306 utilizando um _joystick_ analógico. O eixo X do _joystick_ regula a intensidade do LED vermelho, enquanto o eixo Y ajusta o brilho do LED azul, ambos via PWM, com brilho máximo nos extremos e apagados na posição central. Além disso, o _display_ exibe um quadrado de 8x8 pixels que se desloca conforme os valores do _joystick_. O botão do _joystick_ alterna o estado do LED verde e modifica a borda do _display_ a cada pressionamento, enquanto o botão A permite ativar ou desativar os LEDs PWM.

<div align="center">
  <img src="https://github.com/user-attachments/assets/33560ddb-57e7-43cc-a70d-c8fa7051a5e9" alt="GIF demonstrativo" width="300"/>
</div>

__Para este trabalho, os seguintes componentes e ferramentas se fazem necessários:__
1) Microcontrolador Raspberry Pi Pico W.
2) Ambiente de trabalho VSCode.
3) LEDs RGB
4) _Display_ SSD1306
5) 2 Botões Pull-Up
6) _Joystick_
7) Ferramenta educacional BitDogLab.

__O resultado do projeto pode ser assistido através deste link: [U4C8 - Conversores A/D]().__

## Instruções de Uso

### 1. Clone o repositório
Abra o terminal e execute o comando abaixo para clonar o repositório em sua máquina:
```bash
git clone https://github.com/LucasCarneiro3301/U4C8.git
```

### 2. Configure o ambiente de desenvolvimento
Certifique-se de que o [SDK do Raspberry Pi Pico](https://github.com/raspberrypi/pico-sdk) esteja instalado e configurado corretamente no seu sistema.

### 3. Conexão com a Rapberry Pico
1. Conecte o Raspberry Pi Pico ao seu computador via USB.
2. Inicie o modo de gravação pressionando o botão **BOOTSEL** e **RESTART**.
3. O Pico será montado como um dispositivo de armazenamento USB.
4. Execute através do comando **RUN** a fim de copiar o arquivo `U4C8.uf2` para o Pico.
5. O Pico reiniciará automaticamente e executará o programa.

### 4. Observações (IMPORTANTE !!!)
2. Manuseie a placa com cuidado.

## Recursos e Funcionalidades

### 1. Botões

| BOTÃO                            | DESCRIÇÃO                                     | 
|:----------------------------------:|:---------------------------------------------:|
| A                                  | Desativa os LEDs PWM                 | 
| B                                  | Reinicia para o modo de gravação               | 

### 2. PWM

O programa implementa modulação por largura de pulso para controlar a intensidade de luz dos LEDs vermelho e azul, conforme a posição do _joystick_.

### 3. LEDs PWM

O LED vermelho recebe nível de PWM correspondente à variação da posição do _joystick_ em relação ao eixo X. Já o LED azul recebe nível de PWM correspondente à variação da posição do _joystick_ em relação ao eixo Y

### 4. Joystick

Os pinos dos eixos X e Y são definidos como periféricos ADC. Dessa forma, retorna valores entre 0 e 4095. O valor 2048 corresponde ao centro do _joystick_.

### 5. Display SSD1306

Ilustra a posição atual (correspondente à variação do _joystick_) de um quadrado 8x8. Quando o programa reinicia para o modo de gravação, uma mensagem é exibida para o usuário.

### 6. Interrupções

O programa implementa interrupção nos botões A e B. Dessa forma, não é necessário ler continuamente o estado de cada botão.

### 7. Deboucing

O programa implementa tratamento de _boucing_ (via _software_). Dessa forma, evita leituras falsas de botões causadas por oscilações elétricas, garantindo que cada pressionamento seja registrado apenas uma vez
