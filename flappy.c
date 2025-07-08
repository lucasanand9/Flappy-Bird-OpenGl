//gcc flappy.c -lglut -lGLU -lGL && ./a.out

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// -------------------
// Constantes do Jogo
// -------------------
#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 800

#define GRAVITY -0.08f
#define FLAP_FORCE 4.0f
#define PIPE_SPEED 2.0f

// Intervalo do nosso "game loop" em milissegundos
#define TIMER_MS 8 

// -------------------
// Variáveis Globais de Estado
// -------------------

// Variáveis do Pássaro
float birdY = SCREEN_HEIGHT / 2.0f;
float birdVelocity = 0.0f;

// Variáveis dos Canos
float pipeX = SCREEN_WIDTH + 50;
float pipeGapY = SCREEN_HEIGHT / 2.0f;
const float PIPE_WIDTH = 80.0f;
float pipe_gap_size = 200.0f;

// Estado do Jogo
int isGameOver = 0;
int score = 0;
int maxScore = 0;
float speedGameDifficulty = 0;

// -------------------
// Funções de Desenho
// -------------------

void drawBird() {
    // Salva a matriz de transformação atual
    glPushMatrix();
    
    // Move o quadrado para a posição correta
    glTranslatef(100, birdY, 0); 
    
    // Desenha o pássaro (um quadrado amarelo)
    glColor3f(1.0, 1.0, 0.0); // Amarelo
    glRectf(-15, -15, 15, 15); // Desenha um quadrado de 30x30
    
    // Restaura a matriz de transformação
    glPopMatrix();
}

void drawPipes() {
    glPushMatrix();
    glTranslatef(pipeX, 0, 0);

    glColor3f(0.0, 1.0, 0.0); // Verde

    int newPipeGap = rand() % 100;
    // Cano de baixo
    glRectf(0, 0, PIPE_WIDTH, pipeGapY - ((pipe_gap_size) / 2));
    
    // Cano de cima
    glRectf(0, pipeGapY + (pipe_gap_size / 2), PIPE_WIDTH, SCREEN_HEIGHT);

    glPopMatrix();
}

void drawScore() {
    char scoreText[32];

    // Converte o número inteiro 'score' para uma string formatada
    // A função sprintf funciona como o printf, mas salva o resultado em uma variável
    sprintf(scoreText, "Score: %d\nRecord: %d", score, maxScore);

    // Define a cor do texto (branco)
    glColor3f(1.0, 1.0, 1.0);

    // Define a posição na tela onde o texto começará a ser desenhado
    // As coordenadas (x, y) começam no canto inferior esquerdo da tela
    glRasterPos2f(10, SCREEN_HEIGHT - 30); // Posição no canto superior esquerdo

    // Loop para percorrer cada caractere da nossa string
    for (int i = 0; scoreText[i] != '\0'; i++) {
        // Desenha cada caractere na tela usando uma fonte pré-definida do GLUT
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, scoreText[i]);
    }
}

// -------------------
// Funções de Callback do GLUT
// -------------------

// Função principal de desenho
void display() {
    glClear(GL_COLOR_BUFFER_BIT); // Limpa o buffer de cor
    glLoadIdentity();             // Reinicia a matriz de transformação

    // Desenha os elementos do jogo
    drawPipes();
    drawBird();
    drawScore();

    glutSwapBuffers(); // Troca os buffers (essencial para animação suave)
}

// Função chamada quando a janela é redimensionada
void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT); // Projeção 2D
    glMatrixMode(GL_MODELVIEW);
}

// Função de lógica e animação (nosso "game loop")
void update(int value) {
    if (!isGameOver) {
        // 1. ATUALIZA A FÍSICA DO PÁSSARO
        birdVelocity += GRAVITY;
        birdY += birdVelocity;

        // 2. ATUALIZA A POSIÇÃO DOS CANOS
        pipeX -= PIPE_SPEED + speedGameDifficulty;

        // 3. VERIFICA SE O CANO SAIU DA TELA
        if (pipeX < -PIPE_WIDTH) {
            // Reinicia a posição do cano
            pipeX = SCREEN_WIDTH; 
            // Define uma nova altura aleatória para o buraco
            pipeGapY = (rand() % (SCREEN_HEIGHT - 300)) + 150;
            // Define um tamanho do gap do cano
            pipe_gap_size = (rand() %51) + 150;
            // Aumenta o score
            score++;
            // Se o score for um multiplo de 10, a velocidade do jogo vai aumentar
            if (score % 10 == 0 ){
                speedGameDifficulty +=  PIPE_SPEED*0.1;
                printf("Modificador de velocidade em +%f\n", speedGameDifficulty);
            }
        }
        // 4. VERIFICA COLISÃO COM O CHÃO/TETO
        if (birdY > SCREEN_HEIGHT || birdY < 0) {
            isGameOver = 1;
        }
        // 5. VERIFICA COLISÃO COM O CANO
        float birdTop = birdY + 15;
        float birdBottom = birdY - 15;
        float birdRight = 100 + 15;
        float birdLeft= 100 - 15;
        float pipeTop = pipeGapY + (pipe_gap_size / 2);
        float pipeBottom = pipeGapY - (pipe_gap_size / 2);
        float pipeLeft = pipeX;
        float pipeRight = pipeX + PIPE_WIDTH;
        if ((birdTop >= pipeTop || birdBottom <= pipeBottom) && birdRight >= pipeLeft && birdLeft <= pipeRight){
                isGameOver = 1;
        }
    }else{
        // 6. ATUALIZA O SCORE MAXIMO
        if (score > maxScore){
            maxScore = score;
        }
        // zera o modificador de velocidade
        speedGameDifficulty = 0;
    }
    glutPostRedisplay();
    glutTimerFunc(TIMER_MS, update, 0);
}

// Função para lidar com o teclado
void keyboard(unsigned char key, int x, int y) {
    if (key == ' ' && !isGameOver) { // Barra de espaço
        birdVelocity = FLAP_FORCE; // Aplica o "pulo"
    }
    if (key == 'r' && isGameOver) { // 'r' para reiniciar
        // Reinicia o estado do jogo
        birdY = SCREEN_HEIGHT / 2.0f;
        birdVelocity = 0.0f;
        pipeX = SCREEN_WIDTH + 50;
        isGameOver = 0;
        score = 0;
        printf("Jogo Reiniciado!\n");
    }
}

// -------------------
// Função Principal
// -------------------
int main(int argc, char** argv) {
    // Inicializa o GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); // Habilita double-buffering e cores RGB
    glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Projeto C.G. - Flappy Bird Basico");

    // Define a cor de fundo (um azul claro para o céu)
    glClearColor(0.5f, 0.8f, 1.0f, 1.0f);
    
    // Inicializa o gerador de números aleatórios
    srand(time(NULL));

    // Registra as funções de callback
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(TIMER_MS, update, 0); // Inicia o "game loop"

    // Inicia o loop principal do GLUT
    glutMainLoop();

    return 0;
}