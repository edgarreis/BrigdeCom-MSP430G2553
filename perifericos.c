/*

		 COLUMN (Coluna)

                                           
             P6.0  P6.1  P6.2  P6.3                               
               ^     ^     ^     ^                                 
               |     |     |     |                                 
            |-----|-----|-----|-----|           
  P6.4 ---> |  1  |  2  |  3  |  A  |       
            |-----|-----|-----|-----|       
  P6.5 ---> |  4  |  5  |  6  |  B  |                 
            |-----|-----|-----|-----|      ROW (linha)                    
  P6.6 ---> |  7  |  8  |  9  |  C  |                              
            |-----|-----|-----|-----|                              
  P6.7 ---> |  *  |  0  |  #  |  D  |                              
            |-----|-----|-----|-----|                              


unsigned int count = {'1','2','3','A',                                          Teclado                              
                      '4','5','6','B',
                      '7','8','9','C',
                      '*','0','#','D'};

        
                                                                                Anotações Gerias 

// UCAxIFG -> Registrador que sinaliza se há interrupção pendente.              - Pág 918 User Guide Slau208n
// UCRXIFG -> 0b - Sem Interrupção pendente / 1b - Interrupção pendente         - Pág 918 User Guide Slau208n
// UCAxIFG -> Registrador que habilita interrupção na Uart.



                                           
                                                                                                                                                                                                                                                                                          */
#include "msp430f5529.h"
#include "lcd_Msp430F5529.h"
 										// Keypad Port 
#define COL1 (0x10 & P6IN) 							// 0001 0000
#define COL2 (0x20 & P6IN)							// 0010 0000
#define COL3 (0x40 & P6IN)							// 0100 0000
#define COL4 (0x80 & P6IN)							// 1000 0000
#define keyport P6OUT
                                                                                                                                                                                  
unsigned int count = 26, k=0, i=0, j=0, n=0, t=0, Rec = 50;
char Id_Cart[]={"Id. Card:           "};


void ConfUart_U1_and_U2 (void){                                                 // Função que configura as Uart's 0 e 1.
  
    WDTCTL = WDTPW + WDTHOLD;                                                   // Para o watch dog.
    
    // Setando os pinos para usar as Uart's
    P3SEL |= BIT3 + BIT4; 							// P3.3,4 = USCI_A0 - Uart 0
    P4SEL |= BIT4 + BIT5; 							// P4.4,5 = USCI_A1 - Uart 1
    
    // Inicialização da Uart 0
    UCA0CTL1    |= UCSWRST;
    UCA0CTL1    |= UCSSEL_2;                                                    // SMCLK
    UCA0BR0     = 0x68;                                                         // 1MHz 9600
    UCA0BR1     = 0;                                                            // 1MHz 9600
    UCA0MCTL    |= UCBRS_1 + UCBRF_0;                                           // = 1 Modulação UCBRSx
    UCA0CTL1    &= ~UCSWRST;
    UCA0IE      |= UCRXIE;
    
    // Inicialização da Uart 1
    UCA1CTL1    |= UCSWRST;
    UCA1CTL1    |= UCSSEL_2;                                                    // SMCLK
    UCA1BR0     = 0x68;                                                         // 1MHz 9600
    UCA1BR1     = 0;                                                            // 1MHz 9600
    UCA1MCTL    |= UCBRS_1 + UCBRF_0;                                           // = 1 Modulação UCBRSx
    UCA1CTL1    &= ~UCSWRST;
    UCA1IE      |= UCRXIE;
    
 }
				
void Conf_pins_Keypad (void){                                                   // Função que configura as portas para o Keypad

   P6DIR = 0x0F; 								// Set P6.0 to 6.3 Output ,Set P6.4 to 6.7 Input.        1111 | 0000
   P6REN = 0xFF; 								// Set P6.0 to 6.7 Pull up Register enable. 	         1111 | 1111
   P6OUT = 0xF0; 								// Set P6.0 to 6.7 Out Register.                         1111 | 0000                  
    
 }

void Imp_Lcd(void){                                                             // Bloco de impressão padrão - Teste 
                                                                   
                if ( count == 26 ){
                 
                  Line1;
                  string("       UFPR         ");
                  Line2;
                  string("Eng. Eletrica - P.I.");
                  Line3;
                  string("Edgar R. - Marcos F.");
                  Line4;
                  string("                    ");

                }                                                                                        
                else if ( count == 1 ){                                         // Bloco de impressão - Teste 

                  Line4; string("     Tecla  = 1     ");

                }
                else if ( count == 2 ){

                 Line4; string("     Tecla  = 2     ");

                }
                else if ( count == 3 ){

                  Line4; string("     Tecla  = 3     ");

                }
                else if ( count == 123 ){                                       // A = 123
             
                  Line4; string("     Tecla  = A     ");

                } 
                else if ( count == 4 ){

                  Line4; string("     Tecla  = 4     ");

                }
                else if ( count == 5 ){

                  Line4; string("     Tecla  = 5     ");

                }
                else if ( count == 6 ){

                  Line4; string("     Tecla  = 6     ");

                }
                else if ( count == 456 ){                                       // B = 456
                
                  Line4; string("     Tecla  = B     ");

                } 
                else if ( count == 7 ){

                  Line4; string("     Tecla  = 7     ");

                }
                else if ( count == 8 ){

                  Line4; string("     Tecla  = 8     ");

                }
                else if ( count == 9 ){

                  Line4; string("     Tecla  = 9     ");

                }
                else if ( count == 789 ){                                       // C = 789
               
                  Line4; string("     Tecla  = C     ");

                }
                else if ( count == 10 ){                                        // * = 10
                
                  Line4; string("     Tecla  = *     ");

                }
                else if ( count == 0 ){
                
                  Line4;  string("     Tecla  = 0     ");

                }
                else if ( count == 11 ){                                        // # = 11 
               
                  Line4; string("     Tecla  = #     ");

                }
                else if ( count == 101 ){                                       // D = 101
               
                  Line4; string("     Tecla  = D     ");

                }
                else if ( count == 200 ){                                        // Id = 200
                  
                  if(t == 1){
                    
                        Line4; string(Id_Cart);
                        t=0;
                   
                  }
               
                }
  
}

void Var_Keypad(void){                                                          // Bloco de varredura do teclado - Teste 
  
  for(k=0;k<4;k++){	

		keyport = ((0x01<<k) ^ 0xff);					// Varredura das linhas, da seguinte forma: 	p/ i=0 -> 1111 1110,  = 0xFE
										// Lembrando  q P6.4, P6.5 P6.5 e P6.7 são:	p/ i=1 -> 1111 1101,  = 0xFD
										// entradas.					p/ i=2 -> 1111 1011 e = 0xFB
										//						p/ i=3 -> 1111 0111.  = 0xF7

			if(!COL1){						// So vai entrar nesse if, quando qualquer tecla da coluna 1 (COL1) - 0001 0000 	
			 							// for pressionado.Os n da coluna 1 são: 1, 2, 3, e A.
         
                                if (P6IN == 0xEE)				// - 1
                              
                                    count = 1;
                                
				if (P6IN == 0xED)				// - 2
                               
                                    count = 2;
                                  
				if (P6IN == 0xEB)				// - 3
                                
                                    count = 3;
                                
				if (P6IN == 0xE7)				// - A
                                     
                                    count = 123;                                
                                
				while(!COL1);					// Prende o programa enquanto o botão estiver pressionado.				
                                
			} 		
			if(!COL2){						// So vai entrar nesse if, quando qualquer tecla da coluna 2 (COL2) - 0010 0000
			 							// for pressionado.Os n da coluna 2 são: 4, 5, 6 e B. 

                                if (P6IN == 0xDE)				// - 4
                               
                                  count = 4;
                                  
				if (P6IN == 0xDD)				// - 5
                               
                                  count = 5;
                                
				if (P6IN == 0xDB)				// - 6
                            
                                  count = 6; 
                                
				if (P6IN == 0xD7)				// - B = 456
                              
                                  count = 456;
                                                                
				while(!COL2);					// Prende o programa enquanto o botão estiver pressionado. 								
                                
 			}			
			if(!COL3){						// So vai entrar nesse if, quando qualquer tecla da coluna 3 (COL3) - 0100 0000
			 							// for pressionado.Os n da coluna 3 são: 7, 8, 9, e C.

                                if (P6IN == 0xBE)				// - 7
                                
                                  count = 7;  
                                
				if (P6IN == 0xBD)				// - 8
                              
                                  count = 8;
                                
				if (P6IN == 0xBB)				// - 9
                               
                                  count = 9;                                    
                                
				if (P6IN == 0xB7)				// - C = 789
                               
                                  count = 789;                                     
                                
                                
				while(!COL3);					// Prende o programa enquanto o botão estiver pressionado. 
                                
			}
			if(!COL4){						// So vai entrar nesse if, quando qualquer tecla da coluna 4 (COL4) - 1000 0000
			 							// for pressionado.Os n da coluna 4 são: *, 0, # e D.

                                if (P6IN == 0x7E)				// - * = 10
                             
                                  count = 10;                                   
                                
				if (P6IN == 0x7D)				// - 0
                               
                                  count = 0;                                    
                                 
				if (P6IN == 0x7B)				// - # = 11
                             
                                  count = 11;                                   
                                
				if (P6IN == 0x77)				// - D = 101
                              
                                  count = 101;                                     
                                                        
				while(!COL4);					// Prende o programa enquanto o botão estiver pressionado.
                        
			}                        
        }
 }


int main(){

    ConfUart_U1_and_U2();    
    Conf_pins_Keypad (); 
    Inic_ports();                                                               // Inicia as portas do display Lcd, Porta 2.
    lcd_init();                                                                 // Seta a s configurações do Lcd.   
    
    P1DIR |= BIT0;
    P1OUT &= ~BIT0;  
      
    __bis_SR_register (GIE);                                                    // Enter LPM0, interrupções habilitado
        
    while(1){    
      
    Imp_Lcd();
    Var_Keypad();
   
    }                                                                           // --> Fechamento do while(1) 
}                                                                               // --> Fechamento do main()

#pragma vector = USCI_A1_VECTOR                                                 // Interrupção de recebimento - UART1 --> Pinos P4.4 (Tx) e  P4.5 (Rx).
__interrupt void USCI_A1_ISR (void){                                            // Buffer responsável pelo recebimento: UCA1RXBUF

// unsigned int count = 26, k=0, i=0, j=0, n=0, t=0, Rec = 50;
// char Id_Cart[]={"Id. Card:           "};

  
   if( (UCA1RXBUF == 2 && i ==0) || Rec == 1){                                  //Enquanto a Serial receber dados (ID)         
        if ( i > 0 && i <=10 ){                                                 // Recolhe o somente a ID do cartão.
          Id_Cart[i+9] = UCA1RXBUF;
        }        
   }
   
   i++;                      
   Rec = 1;                                                                 // Indica que esta sendo recebido um cartão.
  
   if ( i == 14){
       Rec = 0;
       i   = 0;       
       count = 200;
       t=1;
   }   
   
   UCA1IFG &= 0;
   
}

/*
#pragma vector = USCI_A0_VECTOR                                                 // Interrupção de recebimento - UART0 --> Pinos P3.4 (Rx) e  P3.5 (Tx).
__interrupt void USCI_A0_ISR (void){                                            // Buffer responsável pelo recebimento: UCA0RXBUF
     
 
      
}
*/




