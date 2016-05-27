/* Comunica��o UART entre MSP430 e ESP8266 */

#include "msp430g2553.h"

// Variaveis Globais
unsigned char data_rec;         // Dado recebido pela UART
unsigned int int_rec;           // Dado recebido pela UART
//unsigned char data_send = 0x06; // Dado enviado pela UART
int flag_pisca = 0;             // Usado para verificar se o buffer esta cheio
int x;                          // Flag x
//unsigned char buffer;           // Variavel para o Buffer

// Configurar uC
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
void send_text(const char *ptr)
{
    while (*ptr)
    {
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

/* Interrup��o do P1.3 */
#pragma vector = PORT1_VECTOR		
__interrupt void interr_P1(void){
    
    P1OUT ^= 0x01;              // Sinalizar interrup��o
  
    flag_pisca ^= 1;		// Inverte flag_pisca
    if (flag_pisca == 1){
        // Enviar msg pela serial
        send_text("hi there\n");
    }		
    else{
        // Enviar msg pela serial
        send_text("ItWorking!\n");
    }
    
    P1IFG = 0x00;		// Zera a flag de interrup��o da porta 1
    
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
    
}

