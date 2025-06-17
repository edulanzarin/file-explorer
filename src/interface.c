#include "interface.h"

/* Guarda como o terminal estava antes de mexermos */
static struct termios oldt;

/* Limpa a tela usando códigos especiais do terminal */
void limpar_tela() {
  printf("\033[2J\033[H"); // Esses códigos fazem a mágica
}

/* Mostra o cabeçalho bonitinho */
void exibir_cabecalho(const char *dir_atual) {
  printf("\033[1m"); // Liga o negrito
  printf("=== Explorador de Arquivos ===\n");
  printf("Diretório: %s\n", dir_atual);
  printf("\033[0m"); // Desliga formatação
}

/* Mostra os arquivos e pastas
   - Destaca o item selecionado
   - Colore pastas de azul
*/
void exibir_entradas(DirEntry *entries, int num_entradas, int selecionado) {
  printf("\n");
  for (int i = 0; i < num_entradas; i++) {
    // Destaca o selecionado
    if (i == selecionado) {
      printf("\033[7m"); // Inverte cores
    }
    // Colore pastas
    else if (entries[i].is_dir) {
      printf("\033[1;34m"); // Azul e negrito
    }

    printf("%s", entries[i].nome);

    // Coloca barra no fim de pastas
    if (entries[i].is_dir) {
      printf("/");
    }

    printf("\033[0m\n"); // Volta ao normal
  }
}

/* Configura o terminal pra ler teclas direto
   - Desliga o eco (não mostra o que digitamos)
   - Lê teclas sem esperar Enter
*/
void configurar_terminal() {
  struct termios newt;

  // Pega as configurações atuais
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;

  // Desliga modos padrão
  newt.c_lflag &= ~(ICANON | ECHO);

  // Aplica as mudanças
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}

/* Volta o terminal ao que era antes */
void restaurar_terminal() { tcsetattr(STDIN_FILENO, TCSANOW, &oldt); }