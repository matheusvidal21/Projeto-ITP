#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

//Constantes
#define MAX_LINE 100
#define MAX_ACTION 10
#define MAX_ID_SIZE 10

//Structs para os bots

typedef struct{  //Struct para as cartas
    char *valorCarta;
    char *valorNaipe;
}Carta;

typedef struct{  //Struct para a mão dos jogadores
    Carta *cartasDoJogador;
    int qtdDeCartas;
}Mao;

typedef struct{   //Struct para o jogador
    char id[MAX_ID_SIZE];
    Mao maoDoJogador;
}Jogador;

//Funções para modularização do código
void debug(char *message){ fprintf(stderr, "%s ", message); }
Carta gerarCarta(char *mensagem);
void acompanhaTotal(Carta totalDeCartas[108], Carta pedaco);
Mao maoInicial(char *mensagem, Carta totalDeCartas[108]);
Carta inicializaCarta(char *valorCarta, char *valorNaipe);
void retornaFrase();
int verificaNaipe(char *valorNaipe, Mao maoJogador, int *indice);
int verificaValor(char *valorCarta, Mao maoJogador, int *indice);
char *naipeFrequente(Jogador *bot);
void retiraCarta(Jogador *bot, int indice);
void adicionaCarta(Jogador *bot, Carta c);
void atualizaEspecial(Carta c, int *especial);
int selecionaCarta(Carta c, Jogador *bot);
int compraCartas(Carta c, char *naipe, int *especial, Jogador *bot);
void recebeCartas(int qtdCartas, Jogador *bot);
Carta acaoDescarta(Jogador *bot, int indice, char *auxNaipe);
void inicializaBaralho(Carta totalDeCartas[108]);
void desalocaCarta(Carta *c);
void acaoCompra(int qtdCartas, Jogador *bot,Carta totalDeCartas[108]);

//Função principal da partida
int main(void){
    int i, j;
    Carta totalDeCartas[108]; //2 baralhos com 58 cartas cada
    Carta pilhaSobMesa[115]; //Vetor de struct para as cartas da mesa
    int contador = 0, especial = 0; //Variável count e para especial
    int cartasCompradas = 0; //Variável auxiliar para cartas compradas
    Carta recebida; //Struct para as cartas recebidas
    Jogador botT; //Struct do meu bot
    Mao minhaMao; //Struct da mão do meu bot
    char temp[MAX_LINE];  
    char my_id[MAX_ID_SIZE]; 
    char complemento2[MAX_LINE];
    char auxNaipe[MAX_LINE]; //Variável auxiliar para guardar o atual naipe 


     // Limpa o buffer stdin, stdout e stderr 
    // Assim, nada é "guardado temporariamente"
    setbuf(stdin, NULL);  
    setbuf(stdout, NULL);  
    setbuf(stderr, NULL);
    srand(time(NULL));

    //Inicializa o baralho inicial
    inicializaBaralho(totalDeCartas);
      
    //Leitura dos jogadores
    scanf("PLAYERS %[^\n]\n", temp);

    //Leitura do ID do bot
    scanf("YOU %s\n", my_id);
    strcpy(botT.id,my_id);

    //Leitura da mão inicial do bot
    scanf("HAND %[^\n]\n", temp);
    minhaMao = maoInicial(temp,totalDeCartas); 
    botT.maoDoJogador = minhaMao;
  
    //Leitura da primeira carta sob a mesa
    scanf("TABLE %s\n", temp);
    pilhaSobMesa[contador++] = gerarCarta(temp);
    //Atualiza o naipe atual
    strcpy(auxNaipe, pilhaSobMesa[contador-1].valorNaipe); 
    auxNaipe[strlen(auxNaipe)] = '\0';
    atualizaEspecial(pilhaSobMesa[contador-1], &especial);


    // === PARTIDA ===
    
    //Costantes da partida
    char id[MAX_ID_SIZE];
    char acao[MAX_ACTION];
    char complemento[MAX_LINE];

    //Laço para esperar a vez do bot
    while(1){

    do{

      //Leitura da jogada do bot anterior
        scanf(" %s %s", acao, complemento);
      
        //Adiciona as cartas jogadas no vetor das pilhas da mesa
        if(strcmp(acao, "DISCARD") == 0){
          pilhaSobMesa[contador++] = gerarCarta(complemento);
          strcpy(auxNaipe, pilhaSobMesa[contador-1].valorNaipe);
          
          //Retira a carta jogada no vetor que guarda o total de cartas
          acompanhaTotal(totalDeCartas, pilhaSobMesa[contador-1]);

          if(strstr(pilhaSobMesa[contador-1].valorCarta,"A")!= NULL || strstr(pilhaSobMesa[contador-1].valorCarta,"C")!= NULL){
            scanf(" %s", complemento2);
            strcpy(auxNaipe, complemento2);
          }
           atualizaEspecial(pilhaSobMesa[contador-1], &especial);
           auxNaipe[strlen(auxNaipe)] = '\0';
        }


      //Verificar se responderam a alguma carta especial
        if(especial == 1 && strcmp(acao, "BUY")==0 && 
          (strcmp(complemento, "2")==0 || strcmp(complemento, "4")==0) || strcmp(acao, "INVALID ACTION") == 0){ 
          especial = 0;
        }

    }while (strcmp(acao, "TURN") || strcmp(complemento, my_id));
      
     //Vez do meu bot
    
    //Inicializar a carta que meu bot recebeu
    recebida = inicializaCarta(pilhaSobMesa[contador-1].valorCarta, pilhaSobMesa[contador-1].valorNaipe); 

    //Verificação de quantas cartas meu bot deve comprar (se necessário)
    cartasCompradas = compraCartas(recebida, auxNaipe, &especial, &botT); 

    //Atualiza o naipe da carta recebida
    strcpy(recebida.valorNaipe, auxNaipe); 

    if(!cartasCompradas){
      //Recebe a carta que o bot vai jogar
        int indice = selecionaCarta(recebida, &botT);

      //Envia ação de descartar para o gerenciador
      //Atualiza o naipe a mão do bot
        pilhaSobMesa[contador++] = acaoDescarta(&botT, indice, auxNaipe); 
      
      //Atualiza o status de especial
        atualizaEspecial(pilhaSobMesa[contador-1], &especial); 
          
    }else{
      //Retorna alguma frase
        retornaFrase();
      //Envia ação de comprar para o gerenciador e atualiza a mão do bot
        acaoCompra(cartasCompradas, &botT,totalDeCartas); 
  }
}

  //Desaloca porção de memória 
  for(i = 0; i < contador; i++){
    desalocaCarta(&pilhaSobMesa[i]);
  }
  free(minhaMao.cartasDoJogador);

    return 0;
}

// === FUNÇÕES ===

//Recebe uma string (com valor e um naipe) e transforma em uma struct Carta
Carta gerarCarta(char *mensagem){     
    Carta saida;
    int tamanho = strlen(mensagem);

    if (mensagem[0] == '1' && mensagem[1] == '0')
    {
        char valor[3] = "10";
        saida.valorCarta = malloc(sizeof(char) * 3);
        strcpy(saida.valorCarta, valor);
        saida.valorNaipe = malloc(sizeof(char) * tamanho);

        for (int i = 2; i < tamanho; i++){
            saida.valorNaipe[i-2] = mensagem[i];
        }
    }else{
        saida.valorCarta = malloc(sizeof(char)*2);
        saida.valorCarta[0] = mensagem[0];
        saida.valorCarta[1] = '\0';
        saida.valorNaipe = malloc(sizeof(char) * tamanho);

        for (int i = 1; i < tamanho; i++){
            saida.valorNaipe[i-1] = mensagem[i];
        }
    }

    return saida;
}

//Função para acompanhar (percorre) todas as cartas
void acompanhaTotal(Carta totalDeCartas[108], Carta pedaco){
  
  int i = 0,j = 0;
  for(i = 0;i < 108; i++){
    if(strcmp(pedaco.valorCarta, totalDeCartas[i].valorCarta) == 0 && strcmp(pedaco.valorNaipe, totalDeCartas[i].valorNaipe) == 0){
      for(j = i; j < 107; j++){
        totalDeCartas[j].valorCarta = totalDeCartas[j+1].valorCarta;
        totalDeCartas[j].valorNaipe = totalDeCartas[j+1].valorNaipe;
      }
      if(j == 107){
        totalDeCartas[j].valorCarta = "";
        totalDeCartas[j].valorNaipe = "";
      }
      break;
    }
  }
}

//Função para a leitura das cartas iniciais do bot
Mao maoInicial(char *mensagem, Carta totalDeCartas[108]){     
    Mao saida;
    Carta aux;
    char *pedaco, vetor[10][10];
    int tamanho, cont = 0;

    pedaco = strtok(mensagem, " ");
    while(pedaco != NULL){
        strcpy(vetor[cont],pedaco);
        cont += 1;
        pedaco = strtok(NULL, " ");
    }

    saida.cartasDoJogador = malloc(sizeof(Carta) * (cont-2));

    for (int i = 1; i < cont - 1; i++) {
        tamanho = strlen(vetor[i]);
        aux = gerarCarta(vetor[i]);
      
        //Retira o baralho recebido do nosso bot do vetor de todas a cartas
        acompanhaTotal(totalDeCartas, aux);
        saida.cartasDoJogador[i-1].valorCarta = malloc(sizeof(char) * 3);
        saida.cartasDoJogador[i-1].valorNaipe = malloc(sizeof(char) * 4);
        strcpy(saida.cartasDoJogador[i-1].valorCarta, aux.valorCarta);
        strcpy(saida.cartasDoJogador[i-1].valorNaipe, aux.valorNaipe);
        saida.qtdDeCartas = i;
    }
  
    free(aux.valorNaipe);
    free(aux.valorCarta);
    return saida;
}

//Função para gerar qualquer carta recebida como parâmetro
Carta inicializaCarta(char *valorCarta, char *valorNaipe){
  char dados[MAX_LINE];  
  Carta c;

  strcpy(dados, valorCarta);
  strcat(dados, valorNaipe);
  c = gerarCarta(dados);

  return c;
}

//Função que retornar frases pré-definidas
void retornaFrase(){
  const char *frases[] = {"Carambolas", "Ta nervoso?",
                               "Onde você está escondendo as cartas?",
                               "Que mundo cruel!", "To quase hein"};

  int index = rand() % 5;
  int probabilidade = rand() % 100;

  if(probabilidade <= 30){
    printf("SAY %s\n", frases[index]);
  }
}

//Função para verificar se o naipe atual tem na mão do bot
int verificaNaipe(char *valorNaipe, Mao maoJogador, int *indice){;
  int i;
  for(i = 0; i<maoJogador.qtdDeCartas; i++){
    if(strcmp(maoJogador.cartasDoJogador[i].valorNaipe, valorNaipe)==0){
      if(indice!=NULL){
        *indice = i;
      }
      return 1;
    }
  }
  return 0;
}

//Função para verificar se o valor atual tem na mão do bot
int verificaValor(char *valorCarta, Mao maoJogador, int *indice){
  for(int i=0; i<maoJogador.qtdDeCartas; i++){
    if(strcmp(maoJogador.cartasDoJogador[i].valorCarta, valorCarta)==0){
      if(indice!=NULL){
        *indice = i;
      }
      return 1;
    }
  }
  return 0;
}

//Seleciona naipe com maior frequencia na mão do bot
char *naipeFrequente(Jogador *bot){ 
  char *listaNaipes[] = {"♥","♦","♣","♠"};
  int qtdNaipe[4] = {0};
  int maior, maiorIndice;

  maior = maiorIndice = 0;
   
   for(int i=0; i<4; i++){
      for(int j=0; j<bot->maoDoJogador.qtdDeCartas; j++){
        if(strcmp(listaNaipes[i], bot->maoDoJogador.cartasDoJogador[j].valorNaipe)==0){
          qtdNaipe[i] += 1;
        }
      }
   }

  for(int i=0; i<4; i++){
    if(qtdNaipe[i] > maior){
      maior = qtdNaipe[i];
      maiorIndice = i;
    }
  }

  return listaNaipes[maiorIndice];
}

//Função que retira a carta jogada da mão do bot (atualização)
void retiraCarta(Jogador *bot, int indice){
  Carta *aux;
  int count = 0;
  int qtdCartas = bot->maoDoJogador.qtdDeCartas;
    
  bot->maoDoJogador.cartasDoJogador[indice].valorCarta = NULL;
  bot->maoDoJogador.cartasDoJogador[indice].valorNaipe = NULL;
  aux = malloc(sizeof(Carta)*(qtdCartas-1));

  for(int i=0; i<qtdCartas; i++){
    if(bot->maoDoJogador.cartasDoJogador[i].valorCarta != NULL && bot->maoDoJogador.cartasDoJogador[i].valorNaipe != NULL){
      aux[count++] = bot->maoDoJogador.cartasDoJogador[i];
    }
  }
  bot->maoDoJogador.qtdDeCartas +=-1;
  free(bot->maoDoJogador.cartasDoJogador);
  bot->maoDoJogador.cartasDoJogador = NULL;
  bot->maoDoJogador.cartasDoJogador = aux;
   
} 

//Função para adicionar a carta na mão
void adicionaCarta(Jogador *bot, Carta c){
  int qtdAtual = bot->maoDoJogador.qtdDeCartas;
  bot->maoDoJogador.cartasDoJogador = realloc(bot->maoDoJogador.cartasDoJogador, sizeof(Carta)*(qtdAtual+1));
  bot->maoDoJogador.qtdDeCartas += 1;
  bot->maoDoJogador.cartasDoJogador[qtdAtual] = c;
}

//Função para atualizar a variável especial e verificar
void atualizaEspecial(Carta c, int *especial){
  if(strcmp(c.valorCarta, "C") == 0 || strcmp(c.valorCarta, "V") == 0){
    (*especial) = 1;
  }
}

//Seleciona a carta na mão para jogar na mesa
int selecionaCarta(Carta c, Jogador *bot){ 
  
  int indice = 0; //indice da carta que vai ser selecionada
  char naipe[MAX_LINE];
  strcpy(naipe, naipeFrequente(bot));
  naipe[strlen(naipe)] = '\0';

  if(verificaValor("C", bot->maoDoJogador, &indice)){ 
    return indice;
  }else if(strcmp(c.valorNaipe, naipe) == 0 && verificaNaipe(c.valorNaipe, bot->maoDoJogador, &indice)){
    return indice;
  }else if(verificaValor(c.valorCarta, bot->maoDoJogador, &indice)){
    return indice;
  }else if(verificaNaipe(c.valorNaipe, bot->maoDoJogador, &indice)){
    return indice;
  }else if(verificaValor("A", bot->maoDoJogador, &indice)){
    return indice;
  } 
}

//Função para comprar cartas se for necessário
int compraCartas(Carta c, char *naipe, int *especial, Jogador *bot){
  if(strcmp(c.valorCarta, "C")==0 && (*especial) == 1){
    (*especial) = 0;
    return 4;
  }else if(strcmp(c.valorCarta, "V")==0 && (*especial) == 1){
    (*especial) = 0;
    return 2;
  }else if(!verificaNaipe(naipe, bot->maoDoJogador, NULL) &&
           !verificaValor(c.valorCarta, bot->maoDoJogador, NULL)){
    return 1;
  }
  return 0;
}

//Função para receber cartas quando compradas e colocar na mão
void recebeCartas(int qtdCartas, Jogador *bot){
    char cartas[qtdCartas][MAX_LINE];
    Carta c;

    for(int i=0; i<qtdCartas; i++){
        scanf(" %s\n", cartas[i]);
        c = gerarCarta(cartas[i]);
        adicionaCarta(bot, c);
    }
}

//Função para enviar ao gerenciador uma ação de descartar uma carta
Carta acaoDescarta(Jogador *bot, int indice, char *auxNaipe){
  char naipe[MAX_LINE];
  Carta c = bot->maoDoJogador.cartasDoJogador[indice];
  strcpy(auxNaipe, c.valorNaipe);

  if(strcmp(c.valorCarta, "A") == 0 || strcmp(c.valorCarta, "C") == 0){
      strcpy(naipe, naipeFrequente(bot));
      naipe[strlen(naipe)] = '\0';
      printf("DISCARD %s%s %s\n", c.valorCarta, c.valorNaipe, naipe);
      strcpy(auxNaipe, naipe);
    }else{
      printf("DISCARD %s%s\n", c.valorCarta, c.valorNaipe);
    }
    auxNaipe[strlen(auxNaipe)] = '\0';

    retiraCarta(bot, indice); //retira a carta da mao do jogador
    return c;
}

//Inicializa o primeiro baralho
void inicializaBaralho(Carta totalDeCartas[108]){
  char* naipes[4] = {"♥", "♦", "♣", "♠"}; 
  char* valor[13] = {"A", "2", "3", "4","5","6","7","8","9","10","V","D","R"}; 

  int k = 0,q = 0;
  
    for(int i = 0; i < 14; i++){
        if(k < 104){
          for(int j=0; j < 4; j++){
            for(q = 0; q < 2; q++){
              totalDeCartas[k].valorCarta = valor[i];
              totalDeCartas[k].valorNaipe = naipes[j];
              k++;
            }
          }
        }
        if(k > 103 && k < 108){
          for(int j=0; j <= 2; j++){
            for(q = 0; q < 2; q++){
              totalDeCartas[k].valorCarta = "C";
              totalDeCartas[k].valorNaipe = naipes[j];
              k++;
            }
            j++;
          }
        }
      }
}

//Função para desalocar porção da memória para a carta
void desalocaCarta(Carta *c){
    free((*c).valorCarta);
    free((*c).valorNaipe);
}

//Envia ação de comprar carta para o gerenciador
void acaoCompra(int qtdCartas, Jogador *bot,Carta totalDeCartas[108]){
  char cartas[qtdCartas][MAX_LINE];
  Carta c;

  printf("BUY %d\n", qtdCartas);
  
//Recebe cartas do gerenciador e adiciona na mão do bot
  for(int i=0; i<qtdCartas; i++){ 
    scanf(" %s\n", cartas[i]);
    c = gerarCarta(cartas[i]);
    //Acompanha total de cartas
    acompanhaTotal(totalDeCartas, c);
    adicionaCarta(bot, c);
  }
}
