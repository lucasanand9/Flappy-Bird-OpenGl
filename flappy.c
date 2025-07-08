//gcc flappy.c -lglut -lGLU -lGL -lm && ./a.out

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// -------------------
// Varivaveis de Textura
// -------------------
GLuint birdTextureID; // ID da textura do pássaro
int birdTextureWidth, birdTextureHeight, birdTextureChannels;
GLuint pipeTextureID;
int pipeTextureWidth, pipeTextureHeight, pipeTextureChannels;

// -------------------
// Constantes do Jogo
// -------------------
#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 800

#define GRAVITY -0.16f
#define FLAP_FORCE 5.0f
#define PIPE_SPEED 2.0f

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

//variaveis background
float montanhasScrollX = 0.0f;
float cidadeScrollX = 0.0f;

// Estado do Jogo
int isGameOver = 0;
int score = 0;
int maxScore = 0;
float speedGameDifficulty = 0;
int lastDifficultyIncreaseScore = 0;


// -------------------
// Funções de Desenho
// -------------------

void drawBird() {
    glBindTexture(GL_TEXTURE_2D, birdTextureID);
    glPushMatrix();
    glTranslatef(100, birdY, 0);
    glColor3f(1.0, 1.0, 1.0); 
    glBegin(GL_QUADS);
        // Canto inferior esquerdo da textura e do quadrado
        glTexCoord2f(0.0, 0.0); glVertex2f(-15, -15);
        // Canto inferior direito
        glTexCoord2f(1.0, 0.0); glVertex2f(15, -15);
        // Canto superior direito
        glTexCoord2f(1.0, 1.0); glVertex2f(15, 15);
        // Canto superior esquerdo
        glTexCoord2f(0.0, 1.0); glVertex2f(-15, 15);
    glEnd();
    glPopMatrix();
    glBindTexture(GL_TEXTURE_2D, 0);
}

void drawPipes() {
    glColor3f(1.0, 1.0, 1.0);
    glBindTexture(GL_TEXTURE_2D, pipeTextureID);
    glPushMatrix();
    glTranslatef(pipeX, 0, 0);
    //cano de baixo
    float bottomPipeTop = pipeGapY - (pipe_gap_size / 2);
    glBegin(GL_QUADS);
        // Mapeia os 4 cantos da imagem nos 4 cantos do retângulo
        glTexCoord2f(0.0, 0.0); glVertex2f(0, 0);                 // Canto inferior esquerdo
        glTexCoord2f(1.0, 0.0); glVertex2f(PIPE_WIDTH, 0);          // Canto inferior direito
        glTexCoord2f(1.0, 1.0); glVertex2f(PIPE_WIDTH, bottomPipeTop); // Canto superior direito
        glTexCoord2f(0.0, 1.0); glVertex2f(0, bottomPipeTop);       // Canto superior esquerdo
    glEnd();
    //cano de cima
    float topPipeBottom = pipeGapY + (pipe_gap_size / 2);
    glBegin(GL_QUADS);
        // Usa o mesmo mapeamento, mas para as coordenadas do cano de cima
        glTexCoord2f(0.0, 1.0); glVertex2f(0, topPipeBottom);     // Canto inferior esquerdo da textura -> Canto inferior esquerdo do quadrado
        glTexCoord2f(1.0, 1.0); glVertex2f(PIPE_WIDTH, topPipeBottom); // Canto inferior direito da textura -> Canto inferior direito do quadrado
        glTexCoord2f(1.0, 0.0); glVertex2f(PIPE_WIDTH, SCREEN_HEIGHT); // Canto superior direito da textura -> Canto superior direito do quadrado
        glTexCoord2f(0.0, 0.0); glVertex2f(0, SCREEN_HEIGHT);       // Canto superior esquerdo da textura -> Canto superior esquerdo do quadrado
    glEnd();
    glPopMatrix();
    glBindTexture(GL_TEXTURE_2D, 0);
}

void drawScore() {
    char scoreText[32];
    sprintf(scoreText, "Score: %d\nRecord: %d", score, maxScore);
    glColor3f(1.0, 1.0, 1.0);
    glRasterPos2f(10, SCREEN_HEIGHT - 30); // Posição no canto superior esquerdo
    for (int i = 0; scoreText[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, scoreText[i]);
    }
}

void loadTextures() {
     // Habilita o uso de texturas e blending
     glEnable(GL_TEXTURE_2D);
     glEnable(GL_BLEND);
     glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
     stbi_set_flip_vertically_on_load(1);
 
     // Carrega a textura do PÁSSARO
     unsigned char *imageData = stbi_load("flappy-bird-sprite.png", &birdTextureWidth, &birdTextureHeight, &birdTextureChannels, 4);
     if (imageData == NULL) {
         printf("Erro ao carregar a imagem da textura do passaro!\n");
         return;
     }
     glGenTextures(1, &birdTextureID);
     glBindTexture(GL_TEXTURE_2D, birdTextureID);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, birdTextureWidth, birdTextureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
     stbi_image_free(imageData);
 
     // Carrega a textura do CANO
     imageData = stbi_load("pipe.png", &pipeTextureWidth, &pipeTextureHeight, &pipeTextureChannels, 4);
     if (imageData == NULL) {
         printf("Erro ao carregar a imagem da textura do cano!\n");
         return;
     }
     glGenTextures(1, &pipeTextureID);
     glBindTexture(GL_TEXTURE_2D, pipeTextureID);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pipeTextureWidth, pipeTextureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
     stbi_image_free(imageData);

 }

// -------------------
// Funções de Callback do GLUT
// -------------------

// Função principal de desenho
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();    
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
    gluOrtho2D(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT);
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
            if (score > 0 && score % 10 == 0 && score != lastDifficultyIncreaseScore ){
                speedGameDifficulty +=  PIPE_SPEED*0.1;
                lastDifficultyIncreaseScore = score;
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

        // --- ATUALIZAÇÃO DO PARALLAX ---
        // As montanhas se movem a 20% da velocidade dos canos
        montanhasScrollX -= PIPE_SPEED * 0.2f;
        // A cidade se move a 50% da velocidade dos canos
        cidadeScrollX -= PIPE_SPEED * 0.5f;

        // Para evitar que a imagem "acabe", resetamos a posição quando ela se moveu o suficiente
        if (montanhasScrollX < -SCREEN_WIDTH) {
            montanhasScrollX = 0;
        }
        if (cidadeScrollX < -SCREEN_WIDTH) {
            cidadeScrollX = 0;
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
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Projeto C.G. - Flappy Bird Basico");
    loadTextures();

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