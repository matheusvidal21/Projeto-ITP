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
    char *valor_carta;
    char *naipe_carta;
}Carta;

typedef struct{  //Struct para a mão dos jogadores
    Carta *cartas_do_jogador;
    int quantidade_cartas;
}Mao;

typedef struct{   //Struct para o jogador
    char id[MAX_ID_SIZE];
    Mao mao_do_jogador;
}Jogador;

//Funções para modularização do código
void debug(char *message){ fprintf(stderr, "%s ", message); }
Carta criarCarta(char *temp);
void ler_total_de_cartas(Carta cartas_total[108], Carta pedaco);
Mao mao_inicial(char *temp, Carta cartas_total[108]);
Carta inicializar_carta(char *valor_carta, char *naipe_carta);
void frase();
int naipe_check(char *naipe_carta, Mao maoJogador, int *indice);
int valor_check(char *valor_carta, Mao maoJogador, int *indice);
char *seleciona_naipe_frequente(Jogador *bot);
void exclui_carta(Jogador *bot, int indice);
void adiciona_carta(Jogador *bot, Carta c);
void status_especial(Carta c, int *especial);
int escolhe_carta(Carta c, Jogador *bot);
int compra_cartas(Carta c, char *naipe, int *especial, Jogador *bot);
void recebe_cartas(int qtdCartas, Jogador *bot);
Carta gerenciador_descartarCarta(Jogador *bot, int indice, char *naipe_atual);
void inicializa_baralho(Carta cartas_total[108]);
void desaloca_carta(Carta *c);
void gerenciador_comprarCarta(int qtdCartas, Jogador *bot,Carta cartas_total[108]);

//Função principal da partida
int main(void){
    int i, j;
    Carta cartas_total[108]; //Total de cartas do baralho
    Carta pilha_da_mesa[115]; //Vetor de struct para as cartas da mesa
    int count = 0, especial = 0; //Variável count e para especial
    int cartas_compradas = 0; //Variável auxiliar para cartas compradas
    Carta recebida; //Struct para as cartas recebidas
    Jogador meu_bot; //Struct do meu bot
    Mao minha_mao; //Struct da mão do meu bot
    char temp[MAX_LINE];  
    char my_id[MAX_ID_SIZE]; 
    char complement2[MAX_LINE];
    char naipe_atual[MAX_LINE]; //Variável auxiliar para guardar o atual naipe 


     // Limpa o buffer stdin, stdout e stderr 
    // Assim, nada é "guardado temporariamente"
    setbuf(stdin, NULL);  
    setbuf(stdout, NULL);  
    setbuf(stderr, NULL);
    srand(time(NULL));

    //Inicializa o baralho inicial
    inicializa_baralho(cartas_total);
      
    //Leitura dos jogadores
    scanf("PLAYERS %[^\n]\n", temp);

    //Leitura do ID do bot
    scanf("YOU %s\n", my_id);
    strcpy(meu_bot.id,my_id);

    //Leitura da mão inicial do bot
    scanf("HAND %[^\n]\n", temp);
    minha_mao = mao_inicial(temp,cartas_total); 
    meu_bot.mao_do_jogador = minha_mao;
  
    //Leitura da primeira carta sob a mesa
    scanf("TABLE %s\n", temp);
    pilha_da_mesa[count++] = criarCarta(temp);
    //Atualiza o naipe atual
    strcpy(naipe_atual, pilha_da_mesa[count-1].naipe_carta); 
    naipe_atual[strlen(naipe_atual)] = '\0';
    status_especial(pilha_da_mesa[count-1], &especial);


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
          pilha_da_mesa[count++] = criarCarta(complemento);
          strcpy(naipe_atual, pilha_da_mesa[count-1].naipe_carta);
          
          //Retira a carta jogada no vetor que guarda o total de cartas
          ler_total_de_cartas(cartas_total, pilha_da_mesa[count-1]);

          if(strstr(pilha_da_mesa[count-1].valor_carta,"A")!= NULL || strstr(pilha_da_mesa[count-1].valor_carta,"C")!= NULL){
            scanf(" %s", complement2);
            strcpy(naipe_atual, complement2);
          }
           status_especial(pilha_da_mesa[count-1], &especial);
           naipe_atual[strlen(naipe_atual)] = '\0';
        }


      //Verificar se responderam a alguma carta especial
        if(especial == 1 && strcmp(acao, "BUY")==0 && 
          (strcmp(complemento, "2")==0 || strcmp(complemento, "4")==0) || strcmp(acao, "INVALID ACTION") == 0){ 
          especial = 0;
        }

    }while (strcmp(acao, "TURN") || strcmp(complemento, my_id));
      
     //Vez do meu bot
    
    //Inicializar a carta que meu bot recebeu
    recebida = inicializar_carta(pilha_da_mesa[count-1].valor_carta, pilha_da_mesa[count-1].naipe_carta); 

    //Verificação de quantas cartas meu bot deve comprar (se necessário)
    cartas_compradas = compra_cartas(recebida, naipe_atual, &especial, &meu_bot); 

    //Atualiza o naipe da carta recebida
    strcpy(recebida.naipe_carta, naipe_atual); 

    if(!cartas_compradas){
      //Recebe a carta que o bot vai jogar
        int indice = escolhe_carta(recebida, &meu_bot);

      //Envia ação de descartar para o gerenciador
      //Atualiza o naipe a mão do bot
        pilha_da_mesa[count++] = gerenciador_descartarCarta(&meu_bot, indice, naipe_atual); 
      
      //Atualiza o status de especial
        status_especial(pilha_da_mesa[count-1], &especial); 
          
    }else{
      //Retorna alguma frase
        frase();
      //Envia ação de comprar para o gerenciador e atualiza a mão do bot
        gerenciador_comprarCarta(cartas_compradas, &meu_bot,cartas_total); 
  }
}

  //Desaloca porção de memória 
  for(i = 0; i < count; i++){
    desaloca_carta(&pilha_da_mesa[i]);
  }
  free(minha_mao.cartas_do_jogador);

    return 0;
}

// === FUNÇÕES ===

//Recebe uma string (com valor e um naipe) e transforma em uma struct Carta
Carta criarCarta(char *temp){     
    Carta saida;
    int tamanho = strlen(temp);

    if (temp[0] == '1' && temp[1] == '0')
    {
        char valor[3] = "10";
        saida.valor_carta = malloc(sizeof(char) * 3);
        strcpy(saida.valor_carta, valor);
        saida.naipe_carta = malloc(sizeof(char) * tamanho);

        for (int i = 2; i < tamanho; i++){
            saida.naipe_carta[i-2] = temp[i];
        }
    }else{
        saida.valor_carta = malloc(sizeof(char)*2);
        saida.valor_carta[0] = temp[0];
        saida.valor_carta[1] = '\0';
        saida.naipe_carta = malloc(sizeof(char) * tamanho);

        for (int i = 1; i < tamanho; i++){
            saida.naipe_carta[i-1] = temp[i];
        }
    }

    return saida;
}

//Função para acompanhar (percorre) todas as cartas
void ler_total_de_cartas(Carta cartas_total[108], Carta pedaco){
  
  int i = 0,j = 0;
  for(i = 0;i < 108; i++){
    if(strcmp(pedaco.valor_carta, cartas_total[i].valor_carta) == 0 && strcmp(pedaco.naipe_carta, cartas_total[i].naipe_carta) == 0){
      for(j = i; j < 107; j++){
        cartas_total[j].valor_carta = cartas_total[j+1].valor_carta;
        cartas_total[j].naipe_carta = cartas_total[j+1].naipe_carta;
      }
      if(j == 107){
        cartas_total[j].valor_carta = "";
        cartas_total[j].naipe_carta = "";
      }
      break;
    }
  }
}

//Função para a leitura das cartas iniciais do bot
Mao mao_inicial(char *temp, Carta cartas_total[108]){     
    Mao saida;
    Carta aux;
    char *pedaco, vetor[10][10];
    int tamanho, cont = 0;

    pedaco = strtok(temp, " ");
    while(pedaco != NULL){
        strcpy(vetor[cont],pedaco);
        cont += 1;
        pedaco = strtok(NULL, " ");
    }

    saida.cartas_do_jogador = malloc(sizeof(Carta) * (cont-2));

    for (int i = 1; i < cont - 1; i++) {
        tamanho = strlen(vetor[i]);
        aux = criarCarta(vetor[i]);
      
        //Retira o baralho recebido do nosso bot do vetor de todas a cartas
        ler_total_de_cartas(cartas_total, aux);
        saida.cartas_do_jogador[i-1].valor_carta = malloc(sizeof(char) * 3);
        saida.cartas_do_jogador[i-1].naipe_carta = malloc(sizeof(char) * 4);
        strcpy(saida.cartas_do_jogador[i-1].valor_carta, aux.valor_carta);
        strcpy(saida.cartas_do_jogador[i-1].naipe_carta, aux.naipe_carta);
        saida.quantidade_cartas = i;
    }
  
    free(aux.naipe_carta);
    free(aux.valor_carta);
    return saida;
}

//Função para gerar qualquer carta recebida como parâmetro
Carta inicializar_carta(char *valor_carta, char *naipe_carta){
  char dados[MAX_LINE];  
  Carta c;

  strcpy(dados, valor_carta);
  strcat(dados, naipe_carta);
  c = criarCarta(dados);

  return c;
}

//Função que retornar frases pré-definidas
void frase(){
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
int naipe_check(char *naipe_carta, Mao maoJogador, int *indice){;
  int i;
  for(i = 0; i<maoJogador.quantidade_cartas; i++){
    if(strcmp(maoJogador.cartas_do_jogador[i].naipe_carta, naipe_carta)==0){
      if(indice!=NULL){
        *indice = i;
      }
      return 1;
    }
  }
  return 0;
}

//Função para verificar se o valor atual tem na mão do bot
int valor_check(char *valor_carta, Mao maoJogador, int *indice){
  for(int i=0; i<maoJogador.quantidade_cartas; i++){
    if(strcmp(maoJogador.cartas_do_jogador[i].valor_carta, valor_carta)==0){
      if(indice!=NULL){
        *indice = i;
      }
      return 1;
    }
  }
  return 0;
}

//Seleciona naipe com maior frequencia na mão do bot
char *seleciona_naipe_frequente(Jogador *bot){ 
  char *listaNaipes[] = {"♥","♦","♣","♠"};
  int qtdNaipe[4] = {0};
  int maior, maiorIndice;

  maior = maiorIndice = 0;
   
   for(int i=0; i<4; i++){
      for(int j=0; j<bot->mao_do_jogador.quantidade_cartas; j++){
        if(strcmp(listaNaipes[i], bot->mao_do_jogador.cartas_do_jogador[j].naipe_carta)==0){
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
void exclui_carta(Jogador *bot, int indice){
  Carta *aux;
  int count = 0;
  int qtdCartas = bot->mao_do_jogador.quantidade_cartas;
    
  bot->mao_do_jogador.cartas_do_jogador[indice].valor_carta = NULL;
  bot->mao_do_jogador.cartas_do_jogador[indice].naipe_carta = NULL;
  aux = malloc(sizeof(Carta)*(qtdCartas-1));

  for(int i=0; i<qtdCartas; i++){
    if(bot->mao_do_jogador.cartas_do_jogador[i].valor_carta != NULL && bot->mao_do_jogador.cartas_do_jogador[i].naipe_carta != NULL){
      aux[count++] = bot->mao_do_jogador.cartas_do_jogador[i];
    }
  }
  bot->mao_do_jogador.quantidade_cartas +=-1;
  free(bot->mao_do_jogador.cartas_do_jogador);
  bot->mao_do_jogador.cartas_do_jogador = NULL;
  bot->mao_do_jogador.cartas_do_jogador = aux;
   
} 

//Função para adicionar a carta na mão
void adiciona_carta(Jogador *bot, Carta c){
  int qtdAtual = bot->mao_do_jogador.quantidade_cartas;
  bot->mao_do_jogador.cartas_do_jogador = realloc(bot->mao_do_jogador.cartas_do_jogador, sizeof(Carta)*(qtdAtual+1));
  bot->mao_do_jogador.quantidade_cartas += 1;
  bot->mao_do_jogador.cartas_do_jogador[qtdAtual] = c;
}

//Função para atualizar a variável especial e verificar
void status_especial(Carta c, int *especial){
  if(strcmp(c.valor_carta, "C") == 0 || strcmp(c.valor_carta, "V") == 0){
    (*especial) = 1;
  }
}

//Seleciona a carta na mão para jogar na mesa
int escolhe_carta(Carta c, Jogador *bot){ 
  
  int indice = 0; //indice da carta que vai ser selecionada
  char naipe[MAX_LINE];
  strcpy(naipe, seleciona_naipe_frequente(bot));
  naipe[strlen(naipe)] = '\0';

  if(valor_check("C", bot->mao_do_jogador, &indice)){ 
    return indice;
  }else if(strcmp(c.naipe_carta, naipe) == 0 && naipe_check(c.naipe_carta, bot->mao_do_jogador, &indice)){
    return indice;
  }else if(valor_check(c.valor_carta, bot->mao_do_jogador, &indice)){
    return indice;
  }else if(naipe_check(c.naipe_carta, bot->mao_do_jogador, &indice)){
    return indice;
  }else if(valor_check("A", bot->mao_do_jogador, &indice)){
    return indice;
  } 
}

//Função para comprar cartas se for necessário
int compra_cartas(Carta c, char *naipe, int *especial, Jogador *bot){
  if(strcmp(c.valor_carta, "C")==0 && (*especial) == 1){
    (*especial) = 0;
    return 4;
  }else if(strcmp(c.valor_carta, "V")==0 && (*especial) == 1){
    (*especial) = 0;
    return 2;
  }else if(!naipe_check(naipe, bot->mao_do_jogador, NULL) &&
           !valor_check(c.valor_carta, bot->mao_do_jogador, NULL)){
    return 1;
  }
  return 0;
}

//Função para receber cartas quando compradas e colocar na mão
void recebe_cartas(int qtdCartas, Jogador *bot){
    char cartas[qtdCartas][MAX_LINE];
    Carta c;

    for(int i=0; i<qtdCartas; i++){
        scanf(" %s\n", cartas[i]);
        c = criarCarta(cartas[i]);
        adiciona_carta(bot, c);
    }
}

//Função para enviar ao gerenciador uma ação de descartar uma carta
Carta gerenciador_descartarCarta(Jogador *bot, int indice, char *naipe_atual){
  char naipe[MAX_LINE];
  Carta c = bot->mao_do_jogador.cartas_do_jogador[indice];
  strcpy(naipe_atual, c.naipe_carta);

  if(strcmp(c.valor_carta, "A") == 0 || strcmp(c.valor_carta, "C") == 0){
      strcpy(naipe, seleciona_naipe_frequente(bot));
      naipe[strlen(naipe)] = '\0';
      printf("DISCARD %s%s %s\n", c.valor_carta, c.naipe_carta, naipe);
      strcpy(naipe_atual, naipe);
    }else{
      printf("DISCARD %s%s\n", c.valor_carta, c.naipe_carta);
    }
    naipe_atual[strlen(naipe_atual)] = '\0';

    exclui_carta(bot, indice); //retira a carta da mao do jogador
    return c;
}

//Inicializa o primeiro baralho
void inicializa_baralho(Carta cartas_total[108]){
  char* naipes[4] = {"♥", "♦", "♣", "♠"}; 
  char* valor[13] = {"A", "2", "3", "4","5","6","7","8","9","10","V","D","R"}; 

  int k = 0,q = 0;
  
    for(int i = 0; i < 14; i++){
        if(k < 104){
          for(int j=0; j < 4; j++){
            for(q = 0; q < 2; q++){
              cartas_total[k].valor_carta = valor[i];
              cartas_total[k].naipe_carta = naipes[j];
              k++;
            }
          }
        }
        if(k > 103 && k < 108){
          for(int j=0; j <= 2; j++){
            for(q = 0; q < 2; q++){
              cartas_total[k].valor_carta = "C";
              cartas_total[k].naipe_carta = naipes[j];
              k++;
            }
            j++;
          }
        }
      }
}

//Função para desalocar porção da memória para a carta
void desaloca_carta(Carta *c){
    free((*c).valor_carta);
    free((*c).naipe_carta);
}

//Envia ação de comprar carta para o gerenciador
void gerenciador_comprarCarta(int qtdCartas, Jogador *bot,Carta cartas_total[108]){
  char cartas[qtdCartas][MAX_LINE];
  Carta c;

  printf("BUY %d\n", qtdCartas);
  
//Recebe cartas do gerenciador e adiciona na mão do bot
  for(int i=0; i<qtdCartas; i++){ 
    scanf(" %s\n", cartas[i]);
    c = criarCarta(cartas[i]);
    //Acompanha total de cartas
    ler_total_de_cartas(cartas_total, c);
    adiciona_carta(bot, c);
  }
}



