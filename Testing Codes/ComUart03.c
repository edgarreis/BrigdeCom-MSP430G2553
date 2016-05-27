/* Comunica��o UART entre MSP430 e ESP8266 */

#include "msp430g2553.h"

// Variaveis Globais
unsigned char data_rec;         // Dado recebido pela UART
int flag_pisca = 0;             // Usado para verificar se o buffer esta cheio
int flag_debauncing;            // Flag debauncing
int buffer_ADC10;               // Variavel para o Buffer do Conversor ADC10


/* Configura��o uC */
void config_uC(void)
{
    // Desativa watchdog timer
    WDTCTL = WDTPW + WDTHOLD;
    
    // Configura o clock
    BCSCTL1 = CALBC1_8MHZ;      // Clock 8 MHZ para baund rate de 9600
    DCOCTL  = CALDCO_8MHZ;      // Clock 8 MHZ para baund rate de 9600
    
    // Configura Sa�das
    P1DIR |=  0x01;             // Output P1.0
    P2DIR |=  0x03;             // Output P2.0, P2.1
    P1OUT &= ~0x01;             // Forcar iniciar off
    P2OUT &= ~0x03;             // Forcar iniciar off
    
    // Configura Entradas
    P1REN = P1REN | 0x08;       // Habilita resistor de pullup/down no pino P1.3
    P1OUT = P1OUT | 0x08;       // Define resistor de pull up no pino P1.3
    P1IE  = 0x08;   	 	// Habilita interrup��o do bot�o P1.3
    P1IES = 0x08;		// Habilita interrup��o na borda de descida
    P1IFG = 0x00; 		// Zera a flag de interrup��oo da porta 1
    
    P1DIR  |=  0x40;            // Saidas P1.6
    P1OUT  &= ~0x40;            // For�ar saida em zero
    P1SEL  |=  0x40;            // Habilitar TA0.1 Saidas P1.6
    
    // Comunica��o UART
    P1SEL    |= (BIT1 + BIT2);  // Selecionar Fun��o Uart nos pinos P1.1 e P1.2
    P1SEL2   |= (BIT1 + BIT2);  // Selecionar Fun��o Uart nos pinos P1.1 e P1.2
    
    UCA0CTL1 |= UCSWRST;        // habilitar Uart para configura��o
    UCA0CTL0  = 0x00;           // USCI A0 Control Register 0
    UCA0CTL1 |= UCSSEL_2;       // USCI A0 Control Register 1 >> USCI 0 Clock Source: 2 (0x80)
    UCA0MCTL  = UCBRS_3;        // Parte Fracionaria da divis�o
    UCA0BR0   = 0x41;           // USCI A0 Baud Rate 1 - Parte inteira  (0x341)
    UCA0BR1   = 0x03;           // USCI A0 Baud Rate 0 - Parte inteira  (0x341)
    UCA0CTL1 &= ~UCSWRST;       // Desabilitar Uart para configura��o
    IE2      |= UCA0RXIE;       // Interrupt Enable 2 >> (0x01)

    
/* Configura��o Timer0_A */
    
    // Timer A Control
    TA0CTL |=  (TASSEL_2 + ID_1 + MC_1); // Timer A clock source select: 2 - SMCLK
                                         // Timer A input divider: 1 - /2
                                         // Timer A mode control: 1 - Up to CCR0
    // Timer A Capture/Compare Control 1
    TA0CCTL1 |= (OUTMOD_6);              // PWM output mode: 6 - PWM toggle/set
    TACCTL0 |= CCIE;		   	 // Habilita captura/compara��o com rela��o a IFG
    
    // Timer A Capture/Compare 0
    TA0CCR0 = 1023;                      // Valor M�ximo da Contagem
    
/* Configura��o ADC */
    
    // ADC10 Control 0
    ADC10CTL0|= (SREF_0 + ADC10SHT_2 + MSC + ADC10ON + ADC10IE);        // VR+ = AVCC and VR- = AVSS
                                                                        // 16 x ADC10CLKs
                                                                        // ADC10 Multiple SampleConversion
                                                                        // ADC10 On/Enable
                                                                        // ADC10 Interrupt Enalbe                                                                    
    // ADC10 Control 1
    ADC10CTL1 |= (INCH_5 + ADC10SSEL_0); // Selects Channel 5
                                         // ADC10OSC
    // ADC10 Analog Enable 0
    ADC10AE0 |= BIT5;                    // Pino P1.5
    
    __bis_SR_register (GIE);             // Enter LPM0, interrup��es habilitado
    ADC10CTL0 |= (ENC + ADC10SC);        // ADC10 Enable Conversion
                                         // ADC10SC
    // For�ar em Zero Saidas
    P1OUT &= ~0x01;                      // Forcar iniciar off
    P2OUT &= ~0x03;                      // Forcar iniciar off
}


/* Envia Byte */
void send_byte( unsigned char byte_send )
{
    while(!(IFG2 & UCA0TXIFG));          // UCA0TXIFG seta quando o buffer esta cheio  
    UCA0TXBUF = byte_send;               // Enviar Bytes
}


/* Enviar Texto (Utilizando Ponteiro) */
void send_text(const char *ptr)
{
    while (*ptr)
    {
        send_byte(*ptr);                 // Correr ponteiro no byte a ser enviado
        ptr++;                           // Proxima letra da palavra
    }
}


int main (void){
  
    config_uC();                         // Configura��es do uC
    while(1);                             // Loop Infinito
}


/* Interrup��o Uart */
 #pragma vector = USCIAB0RX_VECTOR       // Interrup��o de recebimento
    __interrupt void USCI0RX_ISR (void)
{
    data_rec = UCA0RXBUF;                // Buffer recebe byte recebido   
}


/* Interrup��o ADC */
 #pragma vector = ADC10_VECTOR           // Interrup��o de recebimento
 __interrupt void interrupt_ADC (void)
{   
  
    buffer_ADC10 =  ADC10MEM;            // Buffer recebe byte recebido
    ADC10CTL0   &= ~ADC10IFG;            // Reset Flag Interrupt
                                         // ADC10 Interrupt Flag
    ADC10CTL0 |= (ENC + ADC10SC);        // Habilita e Inicializa convers�o
                                         // ADC10 Enable Conversion
                                         // ADC10SC
    /*  */
    //send_byte(buffer_ADC10);             // Enviar msg pela serial
    //send_text("\n");                     // Enviar msg pela serial
    
}
       

/* Interrup��o TIMER0_A */
 #pragma vector = TIMER0_A0_VECTOR       // Interrup��o do Timer
 __interrupt void TIMER0_A (void) 
{
    // Duty Cycle PWM
    TA0CCR1 = buffer_ADC10;              // tempo em up recebe valor do Buffer
    TA0CCTL0 &=~ CCIFG;                  // Limpar flag de interrup��o
    
    // Recebimento da UART
    if(data_rec == 0x31){
        P2OUT &= ~0x01;                  // P2.0 OFF
    }
    else if(data_rec == 0x32){
        P2OUT |=  0x01;                  // P2.0 ON
    }
    else if(data_rec == 0x33){
        P2OUT &= ~0x02;                  // P2.1 OFF
    }
    else if(data_rec == 0x34){
        P2OUT |=  0x02;                  // P2.1 ON
    }
}


/* Interrup��o do P1.3 */
#pragma vector = PORT1_VECTOR		
__interrupt void interr_P1(void){
    
    // Recebimento da UART
  
    P1OUT ^= 0x01;                       // Sinalizar interrup��o
  
    flag_pisca ^= 1;		         // Inverte flag_pisca
    if (flag_pisca == 1){
        send_text("Botton ON\n");        // Enviar msg pela serial
    }		
    else{
        send_text("Botton OFF\n");       // Enviar msg pela serial
    }
    
    // Converter int para char 
    //char char_buffer_ADC10 = buffer_ADC10 + '0';  
    //send_byte(char_buffer_ADC10);            // Envia valor do ACD via UART
    //send_text("/n");            // Envia valor do ACD via UART
    
    // Converter char para int 
    //int int_data_rec = data_rec - '0';
    
    /* Converter int para char 
    char char_buffer_ADC10 = '3' + buffer_ADC10;  
    
    __delay_cycles(400000); 	         // delay de 50ms
    send_byte(char_buffer_ADC10);        // Enviar msg pela serial
    send_text("\n");                     // Enviar msg pela serial
    
    P1IFG = 0x00;       		 // Zera a flag de interrup��o da porta 1
    
    // Debauncing
    __delay_cycles(8000); 	         // delay de 1ms
    flag_debauncing = 1;	         // garante que ir� entrar no while
    
    while(flag_debauncing == 1)
    if((P1IN & 0x08) == 0){              // testa se o bot�o p1.3 est� pressionado
        flag_debauncing = 0;	         // muda status de debauncing para sair do while
    }
    else{			         // se n�o estiver pressionado volta a conferir
        flag_debauncing = 1;
    }
    __delay_cycles(8000);	         // delay de 1ms
    */
}

