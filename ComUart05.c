/* Comunica��o UART entre MSP430 e ESP8266 */

#include "msp430g2553.h"
//#include <stdlib.h>     // itoa
//#include <stdint.h>     // int8_t, int16_t, ...

// Variaveis Globais
unsigned char data_rec;         // Dado recebido pela UART
unsigned int int_rec;           // Dado recebido pela UART
//unsigned char data_send = 0x06; // Dado enviado pela UART
int flag_pisca = 0;             // Usado para verificar se o buffer esta cheio
int x;                          // Flag x
int int_buffer_ADC10;           // Variavel para o Buffer do Conversor ADC10
int Tscaler = 0;                // Tempo do Escalonador do Timer
//unsigned char buffer;           // Variavel para o Buffer
char char_buffer_ADC10[5];

void itoa(long unsigned int inteiro, char* string, int base){
    // por http://www.strudel.org.uk/itoa/
    
    // checa se a base � v�lida
    if (base < 2 || base > 36) {
        *string = '\0';
    }
    
    char* ptr = string, *ptr1 = string, tmp_char;
    int tmp_inteiro;

    do {
        tmp_inteiro = inteiro;
        inteiro /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_inteiro - inteiro * base)];
    } while ( inteiro );
    
    // Aplica sinal negativo
    if (tmp_inteiro < 0) *ptr++ = '-';
    
    *ptr-- = '\0';
    
    while(ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
}

// Configurar uC
void config_uC(void)
{
    /* Desativa watchdog timer */
    WDTCTL = WDTPW + WDTHOLD;
    
    /* Configura��o do clock*/
    BCSCTL1 = CALBC1_8MHZ;      // Clock 8 MHZ para baund rate de 9600
    DCOCTL  = CALDCO_8MHZ;      // Clock 8 MHZ para baund rate de 9600
    
    /* Configura Sa�das */
    P1DIR |=  0x01;             // Output P1.0
    P1DIR |=  0x40;             // Output P1.6
    P2DIR |=  0x03;             // Output P2.0, P2.1
    P1OUT &= ~0x01;             // Forcar iniciar off
    P2OUT &= ~0x03;             // Forcar iniciar off
    
    /* Configura Entradas */
    P1REN = P1REN | 0x08;       // Habilita resistor de pullup/down no pino P1.3
    P1OUT = P1OUT | 0x08;       // Define resistor de pull up no pino P1.3
    P1IE  = 0x08;   	 	// Habilita interrup��o do bot�o P1.3
    P1IES = 0x08;		// Habilita interrup��o na borda de descida
    P1IFG = 0x00; 		// Zera a flag de interrup��oo da porta 1

    /* Comunica��o UART */
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
    
    /* Timer A Control */
    TA0CTL |=  (TASSEL_2 + ID_1 + MC_1);
    /* Timer A clock source select: 2 - SMCLK */
    /* Timer A input divider: 1 - /2 */
    /* Timer A mode control: 1 - Up to CCR0 */
    
    /* Timer A Capture/Compare Control 1 */
    TA0CCTL1 |= (OUTMOD_6);
    /* PWM output mode: 6 - PWM toggle/set */
    
    TACCTL0 |= CCIE;		   	/* Habilita captura/compara��o com rela��o a IFG */
         
    /* Timer A Capture/Compare 0 */
    TA0CCR0 = 64000;            // Valor M�ximo da Contagem (0xFA00 = 64000)
    
    /* Calculo Escalonador
    Timer para envio dos dados = 4s
    Tt0 = div / clk = 2 / 8M = 250ns
    
    250E-9s -> 250
    4E0s  -> 4E9
    
    N = 4E9 / 250 = 16E6
    Tscaler = 16E6 / 64E3 = 250
    */
    
    //TA0CCR1 = 0xFF;              // tempo em up recebe valor do Buffer
    
    
    /* Configura��o ADC */
    /* ADC10 Control 0 */
    ADC10CTL0|= (SREF_0 + ADC10SHT_2 + MSC + ADC10ON + ADC10IE);
    /* VR+ = AVCC and VR- = AVSS */
    /* 16 x ADC10CLKs */
    /* ADC10 Multiple SampleConversion */
    /* ADC10 On/Enable */
    /* ADC10 Interrupt Enalbe */
    
    /* ADC10 Control 1 */    
    ADC10CTL1 |= (INCH_5 + ADC10SSEL_0);
    /* Selects Channel 5 */
    /* ADC10OSC */       
    
    /* ADC10 Analog Enable 0 */
    ADC10AE0 |= BIT5;        // Pino P1.5
    
    __bis_SR_register (GIE);    // Enter LPM0, interrup��es habilitado
    
    ADC10CTL0 |= (ENC + ADC10SC);
    /* ADC10 Enable Conversion */
    /* ADC10SC */
    //__bis_SR_register (GIE);  // Enter LPM0, interrup��es habilitado
    
    __enable_interrupt();       // Habilitar Interrup��es    

}

/* Envia Byte */
void send_byte( unsigned char byte_send )
{
    while(!(IFG2 & UCA0TXIFG)); // UCA0TXIFG seta quando o buffer esta cheio  
    UCA0TXBUF = byte_send;      // Enviar Bytes
}


/* Enviar Texto (Utilizando Ponteiro) */
//void send_text(const int8_t *ptr){
void send_text(const char  *ptr){
    while (*ptr){
        send_byte(*ptr);        // Correr ponteiro no byte a ser enviado
        ptr++;                  // Proxima letra da palavra
    }
}

/* Fun��o Principal */
int main (void){
    config_uC();  
    while(1);           // Loop Infinito

}  
         
/* Interrup��o Uart */
 #pragma vector = USCIAB0RX_VECTOR      // Interrup��o de recebimento
    __interrupt void USCI0RX_ISR (void)
{
    data_rec = UCA0RXBUF;     // Buffer recebe byte recebido      
    
    if(data_rec == 0x31){
        P2OUT |= 0x01;        // P2.0 ON
    }
    if(data_rec == 0x32){
        P2OUT &= ~0x01;       // P2.0 OFF
    }
    if(data_rec == 0x33){
        P2OUT |= 0x02;        // P2.1 ON
    }
    if(data_rec == 0x34){
        P2OUT &= ~0x02;       // P2.1 OFF
    }
       
}

/* Interrup��o TIMER0_A */
 #pragma vector = TIMER0_A0_VECTOR       // Interrup��o do Timer
 __interrupt void TIMER0_A (void) 
{
  Tscaler++;
  
  if( Tscaler == 250){
    
    P1OUT ^= BIT6;              // Sinalizar interrup��o
   
    // Envia ACD para UART
    itoa(int_buffer_ADC10, char_buffer_ADC10, 10);          // int to char base 10
    send_text(char_buffer_ADC10);
    send_byte('\n');
    //__delay_cycles(30000000);	// delay de 1ms
    
    Tscaler = 0;
  }
  
  TA0CCTL0 &=~ CCIFG;                  //Limpar flag de interrup��o 

}

/* Interrup��o do P1.3 */
#pragma vector = PORT1_VECTOR		
__interrupt void interr_P1(void){
    
    P1OUT ^= 0x01;              // Sinalizar interrup��o
  
    flag_pisca ^= 1;		// Inverte flag_pisca
    if (flag_pisca == 1){
        // Enviar msg pela serial
        send_text("Botton ON\n");
    }		
    else{
        // Enviar msg pela serial
        send_text("Botton OFF\n");
    }
    
    P1IFG = 0x00;		// Zera a flag de interrup��o da porta 1
    
    /*
    // DEBAUNCING
    __delay_cycles(8000);	// delay de 1ms
    x = 1;			// garante que ir� entrar no while
    
    while(x == 1)
    if((P1IN & 0x08) == 0){     // testa se o bot�o p1.3 est� pressionado
        x = 0;		        // muda estatus de x para sair do while
    }
    else{			// se n�o estiver pressionado volta a conferir
        x = 1;
    }
    __delay_cycles(8000);	// delay de 1ms
    */
    
}

/* Interrup��o ADC */
 #pragma vector = ADC10_VECTOR      // Interrup��o de recebimento
 __interrupt void interrupt_ADC (void)
{
  
    //P1OUT |= 0x41;
    
    int_buffer_ADC10 =  ADC10MEM;            // Buffer recebe byte recebido
  
    ADC10CTL0 |= (ENC + ADC10SC);         // Habilita e Inicializa convers�o
    /* ADC10 Enable Conversion */
    /* ADC10SC */
     
}


