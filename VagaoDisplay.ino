// VAGAO DISPLAY - Versao 1.1
// By Clederson T. Przybysz - clederson_p@yahoo.com.br
// expressoarduino.blogspot.com
// Criação: 27/09/2018 - Atualizacao 02/02/2019
// Notas Versão:
// 1.1: Troca para biblioteca MD_MAX72xx.h e MD_Parola.h para compatibilidade com módulos 4 em 1 de variados fabricantes  
// 1.0: Criacao codigo;
//   
//Copyright Notes Vagao Display:
// O SOFTWARE É FORNECIDO "NO ESTADO EM QUE SE ENCONTRAM", SEM GARANTIA DE QUALQUER TIPO, EXPRESSA OU IMPLÍCITA, MAS NÃO SE LIMITANDO ÀS GARANTIAS DE COMERCIALIZAÇÃO.  
// EM NENHUMA CIRCUNSTÂNCIA, O AUTOR/TITULAR DE DIREITOS AUTORAIS SE RESPONSABILIZA POR QUALQUER RECLAMAÇÃO, DANOS OU OUTRA RESPONSABILIDADE, 
// SEJA EM AÇÃO DE CONTRATO, DELITO OU DE OUTRA FORMA, DECORRENDO DESTE SOFTWARE OU RELACIONADO AO SEU  USO.
//

#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <SD.h>


//Parametros Fixos
#define  DATA_PIN    11   // DIN pin of MAX7219 module
#define  CLK_PIN     13   // CLK pin of MAX7219 module
#define  CS_PIN      10   // CS pin of MAX7219 module
#define  maxInUse     4   //Numero de Modulos MAX7219
#define  brilho       3   //Brilho Display 0 a 15
#define  HARDWARE_TYPE MD_MAX72XX::FC16_HW   // Modelo do Display: FC16_HW, PAROLA_HW, ICSTATION_HW ou GENERIC_HW 
#define  BUF_SIZE  70

// HARDWARE SPI
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, maxInUse);

byte LinhaSD=0;
byte TamanhoString=0;

//char TextoDisplay[30]; 
char curMessage[BUF_SIZE] = { "" };
char newMessage[BUF_SIZE] = { "Expresso Arduino" };
bool newMessageAvailable = true;


//Configuraçoes da Exibicao
uint8_t scrollSpeed = 70;    // default frame delay value
textEffect_t scrollEffect = PA_SCROLL_LEFT;
textPosition_t scrollAlign = PA_LEFT;
uint16_t scrollPause = 1; // in milliseconds


void setup() {
  //Inicia Serial
  Serial.begin(19200);

  //Inicia Modulos MAX7219
  P.begin();
  P.displayText(curMessage, scrollAlign, scrollSpeed, scrollPause, scrollEffect, scrollEffect);

  //Inicia Cartao SD
  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    return;
  }
  
}

void loop() {
  //Le Cartao SD
  //LeLinhaCartao();
  if (P.displayAnimate())
  {
    Serial.println("Le Cartao");
    LeLinhaCartao();
    if (newMessageAvailable)
    {
      strcpy(curMessage, newMessage);
      newMessageAvailable = false;
      Serial.println("Nova Mensagem");
    }
    P.displayReset();
  }
  //readSerial();
  
  //Exibe Mensagem do Linha do Cartao
  //printStringWithShift(TextoDisplay, velocidade); // (texto, velocidade)
  //Mostra Espaços entre as Linhas do Cartao
  //printStringWithShift("    ", velocidade);
  //Fim
  
}




//Le Linha do Cartao SD
void LeLinhaCartao() {
  File myFile;
  byte tmpLinha=0;
  byte LinhaCarregada=0;
  char StringRetorno[BUF_SIZE]; 

  //Abre Arquivo
  myFile = SD.open("display.txt");
  
  if (myFile) {
    char caracter;
    byte c=0;
    char inputString[100];
    //Loop Leitura de Todos Caracteres do Arquivo no Cartao
    while (myFile.available()) {
       caracter  = myFile.read();
       //Se não é Final da Linha (Caracter 13 ou 11) armazena inputString 
       if (caracter!='\n'&&caracter!='\r') {
        inputString[c]=caracter;
        c++;
       }
       //Se Encontrou Final da Linha (Caracter 13 ou 11) trata inputString 
       if ((caracter=='\n'||caracter=='\r')&&c>0) {
          //Armazena Primeira Linha do Arquivo como StringRetorno
          if (tmpLinha==0) {
            for (int i=0; i<c;i++) {
              StringRetorno[i]=inputString[i];
              StringRetorno[c]=0;
            } 
          }
          //Se a Linha Carrega é a Linha a Ser Exibida transfere inputString para StringRetorno
          if (tmpLinha==LinhaSD) {
            for (int i=0; i<c;i++) {
              StringRetorno[i]=inputString[i];
            }
            StringRetorno[c]=0;
            LinhaCarregada=1;
          }
          //reinicia inputString e incrementa numero da linha carregada
          c=0;
          tmpLinha++;
      }
      
      //Se Carregou a Linha Esperada Encerra Leitura do Cartao
      if (LinhaCarregada==1) {
        break;
      }
    }
    myFile.close();

    //Se Carregou a Linha Esperada Transfere StringRetorno para varivel TextoDisplay
    if (LinhaCarregada==1) {
      //Avanca Variavel para Proxima Linha do Arquivo
      LinhaSD++;
      for (int i=0; i<BUF_SIZE;i++) {
        newMessage[i]=StringRetorno[i];
        newMessageAvailable=true;
        if (StringRetorno[i]==0) {
          TamanhoString=i;
          break;
        }
      }
    }
    //Se Não Encontrou a Linha Esperada (Fim do Arquivo) transfere primeira linha armazenada em StringRetorno para varivel TextoDisplay
    else if (tmpLinha>0) {
      for (int i=0; i<BUF_SIZE;i++) {
        newMessage[i]=StringRetorno[i];
        newMessageAvailable=true;
        if (StringRetorno[i]==0) {
          TamanhoString=i;
          break;
        }
      }
      //Inicia Linha do Arquivo
      LinhaSD=1;
    }
  } else {
    // Mensagem se nao conseguir abrir o arquivo
    Serial.println("Erro ao abrir display.txt");
  }
}
