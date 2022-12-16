//Bibliotecas utilizadas 
#include <stdio.h> //Entrada/saída
#include <stdlib.h> //Alocação de memória
#include <time.h> //Manipulação de horário
#include <string.h> //Strings

//Constantes pré-definidas
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
void recebe_cartas(int quantidade_cartas, Jogador *bot);
Carta gerenciador_descartarCarta(Jogador *bot, int indice, char *naipe_atual);
void inicializa_baralho(Carta cartas_total[108]);
void desaloca_carta(Carta *c);
void gerenciador_comprarCarta(int quantidade_cartas, Jogador *bot,Carta cartas_total[108]);

//Função principal da partida
int main(void){
    int i, j;
    Carta cartas_total[108]; //Vetor de struct carta p/ o total de cartas do baralho
    Carta pilha_da_mesa[108]; //Vetor de struct para as cartas sob a mesa
    int count = 0, especial = 0; //Variável auxiliar count e carta especial
    int cartas_compradas = 0; //Variável auxiliar para cartas compradas
    Carta cartaSobMesa; //Struct do tipo carta para a carta sob a mesa
    Jogador meu_bot; //Struct do tipo jogador para o nosso bot
    Mao minha_mao; //Struct da mão do nosso bot
    char temp[MAX_LINE]; //Varíavel temporária
    char my_id[MAX_ID_SIZE]; //Id do nosso bot
    char complement2[MAX_LINE]; //Complemento 2
    char naipe_atual[MAX_LINE]; //Variável auxiliar para guardar o naipe atual

    // Limpa o buffer stdin, stdout e stderr 
    // Assim, nada é "guardado temporariamente"
    setbuf(stdin, NULL);  
    setbuf(stdout, NULL);  
    setbuf(stderr, NULL);
    srand(time(NULL));

    //-- COMEÇO --
    //Inicializa o baralho inicial
    inicializa_baralho(cartas_total);
      
    //Leitura dos jogadores
    scanf("PLAYERS %[^\n]\n", temp);

    //Leitura do ID do bot
    scanf("YOU %s\n", my_id);
    strcpy(meu_bot.id,my_id); //Guarda nosso id no struct "meu_bot"

    //Leitura da mão inicial do bot
    scanf("HAND %[^\n]\n", temp);
    minha_mao = mao_inicial(temp,cartas_total); 
    meu_bot.mao_do_jogador = minha_mao; //Guarda nossa mão inicial no struct "meu bot"
  
    //Leitura da primeira carta sob a mesa
    scanf("TABLE %s\n", temp);
    pilha_da_mesa[count++] = criarCarta(temp); //Criando a primeira carta e atribui ao vetor pilha da mesa
    //Atualiza a variavel naipe atual
    strcpy(naipe_atual, pilha_da_mesa[count-1].naipe_carta); 
    naipe_atual[strlen(naipe_atual)] = '\0';
    //Atualiza o status especial da carta sob a mesa
    status_especial(pilha_da_mesa[count-1], &especial);


    // === PARTIDA ===
    
    //Costantes pré-definidos da partida
    char id[MAX_ID_SIZE];
    char acao[MAX_ACTION];
    char complemento[MAX_LINE];

    //Laço para esperar a vez do bot
    while(1){
      //Laço para ler as jogadas antes da vez do nosso bot
    do{

      //Leitura da jogada do bot anterior
        scanf(" %s %s", acao, complemento); //Scanf para o gerenciador
      
        //Adiciona as cartas jogadas no vetor das pilhas da mesa
        if(strcmp(acao, "DISCARD") == 0){
          pilha_da_mesa[count++] = criarCarta(complemento);
          strcpy(naipe_atual, pilha_da_mesa[count-1].naipe_carta);
          
          //Retira a carta jogada no vetor que guarda o total de cartas
          ler_total_de_cartas(cartas_total, pilha_da_mesa[count-1]); 

          
          if(strstr(pilha_da_mesa[count-1].valor_carta,"A")!= NULL || strstr(pilha_da_mesa[count-1].valor_carta,"C")!= NULL){
            scanf(" %s", complement2);
            strcpy(naipe_atual, complement2); //Copia o naipe escolhido quando for "compra 4" ou "muda a cor" para o auxiliar naipe atual
          }
           status_especial(pilha_da_mesa[count-1], &especial); //Atualiza o status do especial
           naipe_atual[strlen(naipe_atual)] = '\0';
        }

      //Verifica se responderam corretamente a alguma carta especial 
        if(especial == 0 && strcmp(acao, "BUY") == 0 && 
          (strcmp(complemento, "2") == 0 || strcmp(complemento, "4") == 0) || strcmp(acao, "INVALID ACTION") == 0){ 
          especial = 0;
        }

    }while(strcmp(acao, "TURN") || strcmp(complemento, my_id)); //Fim do laço que lia as jogadas anteriores a vez do bot
      
    // === Vez do meu bot ===
    
    //Inicializa ultima carta jogada na mesa
    cartaSobMesa = inicializar_carta(pilha_da_mesa[count-1].valor_carta, pilha_da_mesa[count-1].naipe_carta); 

    //Verificação de quantas cartas meu bot deve comprar (se necessário)
    cartas_compradas = compra_cartas(cartaSobMesa, naipe_atual, &especial, &meu_bot); 

    //Copia o naipe da carta sob a mesa para a variavel naipe atual
    strcpy(cartaSobMesa.naipe_carta, naipe_atual); 

    if(!cartas_compradas){ //Se não for necessário comprar carta
    
        int indice = escolhe_carta(cartaSobMesa, &meu_bot); //Retorna o indice da carta que irá ser jogada

      //Envia ação de descartar para o gerenciador
      //Atualiza o naipe a mão do bot
        pilha_da_mesa[count++] = gerenciador_descartarCarta(&meu_bot, indice, naipe_atual); 
      
      //Atualiza o status de especial
        status_especial(pilha_da_mesa[count-1], &especial); 
          
    }else{ //Se for necessário comprar carta
      //Retorna alguma frase
        frase();
      //Envia ação de comprar para o gerenciador e atualiza a mão do bot
        gerenciador_comprarCarta(cartas_compradas, &meu_bot, cartas_total); 
  }
}

  //Libera a memória que estava sendo ocupada
  for(i = 0; i < count; i++){
    desaloca_carta(&pilha_da_mesa[i]);
  }
  free(minha_mao.cartas_do_jogador);

    return 0;
}

// === FUNÇÕES ===

//Recebe uma string (com valor e um naipe) e transforma em uma struct Carta
Carta criarCarta(char *temp){     
    Carta saida; //Struct do tipo carta que irá ser retornada
    int tamanho = strlen(temp);

    if (temp[0] == '1' && temp[1] == '0') //Caso especial quando for 10
    {
        char valor[3] = "10";
        //Alocação dinâmica para a carta
        saida.valor_carta = malloc(sizeof(char) * 3); 
        strcpy(saida.valor_carta, valor); //Copia o valor para o struct
        saida.naipe_carta = malloc(sizeof(char) * tamanho);
        //Copia o naipe para o struct
        for (int i = 2; i < tamanho; i++){
            saida.naipe_carta[i-2] = temp[i];
        }
    }else{
      //Alocação dinâmica para a carta
        saida.valor_carta = malloc(sizeof(char) * 2);
        saida.valor_carta[0] = temp[0]; //Copia o valor para o struct
        saida.valor_carta[1] = '\0';
        saida.naipe_carta = malloc(sizeof(char) * tamanho);
      //Copia o naipe para o struct
        for (int i = 1; i < tamanho; i++){
            saida.naipe_carta[i-1] = temp[i];
        }
    }

    return saida; //Retorna o struct com valor e naipe criado
}

//Função para acompanhar (percorre) todas as cartas, utilizada para retirar as cartas do vetor total de cartas
void ler_total_de_cartas(Carta cartas_total[108], Carta pedaco){ 
  
  //For para percorrer o baralho (valor e naipe)
  int i = 0,j = 0;
  for(i = 0; i < 108; i++){
    if(strcmp(pedaco.valor_carta, cartas_total[i].valor_carta) == 0 && strcmp(pedaco.naipe_carta, cartas_total[i].naipe_carta) == 0){
      for(j = i; j < 107; j++){
        cartas_total[j].valor_carta = cartas_total[j + 1].valor_carta;
        cartas_total[j].naipe_carta = cartas_total[j + 1].naipe_carta;
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
    Mao saida; //Struct para mão inicial
    Carta aux; 
    char *pedaco, vetor[10][10];
    int tamanho, cont = 0;

    pedaco = strtok(temp, " "); //Função que divide a string

  //Separa a mão recebida em pedaços
    while(pedaco != NULL){
        strcpy(vetor[cont],pedaco);
        cont += 1;
        pedaco = strtok(NULL, " "); 
    }

    saida.cartas_do_jogador = malloc(sizeof(Carta) * (cont-2)); //Alocação dinâmica

    for (int i = 1; i < cont - 1; i++) {
        tamanho = strlen(vetor[i]);
        aux = criarCarta(vetor[i]);
      
        //Retira o baralho recebido do nosso bot do vetor de todas a cartas
        ler_total_de_cartas(cartas_total, aux);

        saida.cartas_do_jogador[i - 1].valor_carta = malloc(sizeof(char) * 3); //Alocação para as cartas
        saida.cartas_do_jogador[i - 1].naipe_carta = malloc(sizeof(char) * 4);
        strcpy(saida.cartas_do_jogador[i - 1].valor_carta, aux.valor_carta); //Copia o valor para o struct
        strcpy(saida.cartas_do_jogador[i - 1].naipe_carta, aux.naipe_carta); //Copia o naip para o struct
        saida.quantidade_cartas = i;
    }
  
    free(aux.naipe_carta);
    free(aux.valor_carta);
    return saida; //Retorna o struct da mão inicial do bot
}

//Função para gerar qualquer carta recebida como parâmetro
Carta inicializar_carta(char *valor_carta, char *naipe_carta){
  char dados[MAX_LINE];  
  Carta c;

  strcpy(dados, valor_carta);
  strcat(dados, naipe_carta);
  c = criarCarta(dados); //Passa os dados da carta como parâmetro pra função de criar carta

  return c; //Retorna a carta criada
}

//Função que retornar frases pré-definidas
void frase(){
  const char *frases[] = {"Carambolas", "Ta nervoso?",
                          "Onde você está escondendo as cartas?",
                          "Que mundo cruel!", "To quase hein"}; 

  int index = rand() % 5; //Função que gera números inteiros aleatórios entre 0 e 5 (número de frases)
  int probabilidade = rand() % 100; //Gerar número inteiro aleatório como probabilidade

  if(probabilidade <= 30){ //Se o número gerado for menor ou igual a 30, imprime a frase sorteada
    printf("SAY %s\n", frases[index]);
  }
}

//Função para verificar se o naipe recebido como parâmetro tem na mão do bot
int naipe_check(char *naipe_carta, Mao maoJogador, int *indice){;
  int i;
  //For para percecorrer a mão do bot
  for(i = 0; i < maoJogador.quantidade_cartas; i++){
    if(strcmp(maoJogador.cartas_do_jogador[i].naipe_carta, naipe_carta) == 0){ //Compara o naipe recebido com os naipes do bot
      if(indice != NULL){         
        *indice = i;
      }
      return 1;
    }
  }
  return 0;
}

//Função para verificar se o valor recebido como o parâmetro tem na mão do bot
int valor_check(char *valor_carta, Mao maoJogador, int *indice){
  int i;
  //For para percecorrer a mão do bot
  for(int i = 0; i < maoJogador.quantidade_cartas; i++){
    if(strcmp(maoJogador.cartas_do_jogador[i].valor_carta, valor_carta) == 0){ //Compara o valor recebido com os naipes do bot
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
  int quantidade_naipe[4] = {0};
  int maior = 0, maiorIndice = 0;

   //Percorre os naipes e a mão do bot para verificar a frequência de cada naipe
   for(int i = 0; i < 4; i++){
      for(int j=0; j<bot->mao_do_jogador.quantidade_cartas; j++){
        if(strcmp(listaNaipes[i], bot->mao_do_jogador.cartas_do_jogador[j].naipe_carta) == 0){
          quantidade_naipe[i] += 1;
    }
  }
}

  for(int i = 0; i < 4; i++){
    if(quantidade_naipe[i] > maior){
      maior = quantidade_naipe[i];
      maiorIndice = i; //Coloca o indice do naipe com mais quantidade na variável
    }
  }

  return listaNaipes[maiorIndice]; //Retorna o naipe com mais frequência
}

//Função que retira a carta jogada da mão do bot (atualização da mão)
void exclui_carta(Jogador *bot, int indice){
  Carta *aux;
  int i, count = 0;
  int quantidade_cartas = bot->mao_do_jogador.quantidade_cartas;
    
  bot->mao_do_jogador.cartas_do_jogador[indice].valor_carta = NULL;
  bot->mao_do_jogador.cartas_do_jogador[indice].naipe_carta = NULL;
  aux = malloc(sizeof(Carta)*(quantidade_cartas-1)); //Alocação para as cartas
  //Percorre a mão do bot
  for(i = 0; i < quantidade_cartas; i++){
    if(bot->mao_do_jogador.cartas_do_jogador[i].valor_carta != NULL && bot->mao_do_jogador.cartas_do_jogador[i].naipe_carta != NULL){
      aux[count++] = bot->mao_do_jogador.cartas_do_jogador[i];
    }
  }
  bot->mao_do_jogador.quantidade_cartas +=-1; //Tira um da quantidade de cartas do bot
  free(bot->mao_do_jogador.cartas_do_jogador); //Libera o espaço de memória na mão do bot
  bot->mao_do_jogador.cartas_do_jogador = NULL;
  bot->mao_do_jogador.cartas_do_jogador = aux;
   
} 

//Função para adicionar a carta na mão (atualização da mão)
void adiciona_carta(Jogador *bot, Carta c){
  int qtdAtual = bot->mao_do_jogador.quantidade_cartas; //Variável para a quantidade atual de cartas do bot
  bot->mao_do_jogador.cartas_do_jogador = realloc(bot->mao_do_jogador.cartas_do_jogador, sizeof(Carta)*(qtdAtual+1));
  bot->mao_do_jogador.quantidade_cartas += 1;
  bot->mao_do_jogador.cartas_do_jogador[qtdAtual] = c; //Adiciona a carta recebida na mão do bot
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
  strcpy(naipe, seleciona_naipe_frequente(bot)); //Seleciona o naipe mais frequente 
  naipe[strlen(naipe)] = '\0';

  //Verifica se o bot tem coringa para selecionar
  if(valor_check("C", bot->mao_do_jogador, &indice)){ 
    return indice; //Seleciona essa carta

  //Verifica se a carta sob a mesa tem o mesmo naipe que o naipe mais frequente e se o bot tem esse naipe 
  }else if(strcmp(c.naipe_carta, naipe) == 0 && naipe_check(c.naipe_carta, bot->mao_do_jogador, &indice)){
    return indice; //Seleciona essa carta

  //Verifica se o bot tem alguma carta com o mesmo valor da carta sob a mesa 
  }else if(valor_check(c.valor_carta, bot->mao_do_jogador, &indice)){
    return indice; //Seleciona essa carta

  //Verifica se o bot tem alguma carta com o mesmo naipe da carta sob a mesa   
  }else if(naipe_check(c.naipe_carta, bot->mao_do_jogador, &indice)){
    return indice; //Seleciona essa carta

  //No último caso, verifica se o bot tem a carta "muda naipe"  
  }else if(valor_check("A", bot->mao_do_jogador, &indice)){
    return indice; //Seleciona essa carta
  } 
}

//Função para comprar cartas se for necessário
int compra_cartas(Carta c, char *naipe, int *especial, Jogador *bot){
  //Verifica se a carta recebida é a "compra 4", se for retorna 4
  if(strcmp(c.valor_carta, "C") == 0 && (*especial) == 1){
    (*especial) = 0;
    return 4; //Deve comprar 4 cartas
  //Verifica se a carta recebida é a "compra 2", se for retorna 2
  }else if(strcmp(c.valor_carta, "V")==0 && (*especial) == 1){
    (*especial) = 0;
    return 2; //Deve comprar 2 cartas 
  //Verifica se o bot possui naipe e o valor da carta, se não possuir retorna 1
  }else if(!naipe_check(naipe, bot->mao_do_jogador, NULL) &&
           !valor_check(c.valor_carta, bot->mao_do_jogador, NULL)){
    return 1; //Deve comprar 1 carta
  }
  //Retorna 0 se não for necessário comprar cartas
  return 0;
}

//Função para receber cartas quando compradas e colocar na mão
void recebe_cartas(int quantidade_cartas, Jogador *bot){
    char cartas[quantidade_cartas][MAX_LINE];
    Carta c;
    //Percorre a mão do bot
    for(int i = 0; i < quantidade_cartas; i++){
        scanf(" %s\n", cartas[i]);
        c = criarCarta(cartas[i]); //Cria a carta recebida
        adiciona_carta(bot, c); //Adiciona a carta recebida na mão do bot 
    }
}

//Função para enviar ao gerenciador uma ação de descartar uma carta
Carta gerenciador_descartarCarta(Jogador *bot, int indice, char *naipe_atual){
  char naipe[MAX_LINE];
  Carta c = bot->mao_do_jogador.cartas_do_jogador[indice];
  strcpy(naipe_atual, c.naipe_carta);

  if(strcmp(c.valor_carta, "A") == 0 || strcmp(c.valor_carta, "C") == 0){ //Se a carta escolhida for 
      strcpy(naipe, seleciona_naipe_frequente(bot));                      //"muda cor" ou "compra 4"
      naipe[strlen(naipe)] = '\0';
      printf("DISCARD %s%s %s\n", c.valor_carta, c.naipe_carta, naipe); //Envia ação de descartar para o gerenciador
      strcpy(naipe_atual, naipe);
    }else{
      printf("DISCARD %s%s\n", c.valor_carta, c.naipe_carta); //Se for carta normal, passa somente dois parâmetros
    }
    naipe_atual[strlen(naipe_atual)] = '\0';

    exclui_carta(bot, indice); //Retira a carta da mão do jogador
    return c;
}

//Inicializa o primeiro baralho
void inicializa_baralho(Carta cartas_total[108]){
  char* naipes[4] = {"♥", "♦", "♣", "♠"};  //Vetor para os naipes
  char* valor[13] = {"A", "2", "3", "4","5","6","7","8","9","10","V","D","R"}; //Vetor para os valores

  int k = 0,q = 0;
  //Adiciona o baralho no vetor de cartas total
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
void gerenciador_comprarCarta(int quantidade_cartas, Jogador *bot,Carta cartas_total[108]){
  char cartas[quantidade_cartas][MAX_LINE];
  Carta c;

  printf("BUY %d\n", quantidade_cartas); //Manda para o gerenciador a ação de comprar cartas
                                        //e passa a quantidade


//Percorre as cartas
  for(int i = 0; i < quantidade_cartas; i++){ 
    scanf(" %s\n", cartas[i]);
    c = criarCarta(cartas[i]); //Cria a carta recebida
    //Acompanha total de cartas
    ler_total_de_cartas(cartas_total, c);
    adiciona_carta(bot, c); //Adiciona na mão do bot
  }
}



