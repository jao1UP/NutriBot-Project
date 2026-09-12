#ifndef ROBOT_EYES_H
#define ROBOT_EYES_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

enum EyeExpression {
  EXPR_NORMAL,
  EXPR_HAPPY,
  EXPR_SCARED,
  EXPR_SLEEPY
};

class RobotEyes {
  private:
    Adafruit_SSD1306* disp;
    int screenWidth, screenHeight;
    
    // Configurações dos olhos
    int eyeWidth = 28;
    int eyeHeight = 32;
    int cornerRadius = 6;
    int leftEyeX = 32;
    int rightEyeX = 96;
    int centerY = 32;

    // Variáveis de animação suave
    float currentHeight;
    float targetHeight;
    float pupilOffsetX = 0;
    float pupilOffsetY = 0;
    float targetPupilX = 0;
    float targetPupilY = 0;
    
    unsigned long lastBlink = 0;
    unsigned long nextBlinkInterval = 3000;
    bool blinking = false;

    unsigned long lastLook = 0;
    unsigned long nextLookInterval = 2000;

    EyeExpression currentExpr = EXPR_NORMAL;

  public:
    RobotEyes(Adafruit_SSD1306* displayInstance) {
      disp = displayInstance;
      screenWidth = disp->width();
      screenHeight = disp->height();
      currentHeight = eyeHeight;
      targetHeight = eyeHeight;
    }

    void setExpression(EyeExpression expr) {
      currentExpr = expr;
      switch(expr) {
        case EXPR_NORMAL:
          eyeHeight = 32;
          break;
        case EXPR_HAPPY:
          eyeHeight = 18; // Olhos apertadinhos de alegria
          break;
        case EXPR_SCARED:
          eyeHeight = 38; // Olhos arregalados
          break;
        case EXPR_SLEEPY:
          eyeHeight = 10; // Quase fechados
          break;
      }
    }

    void update() {
      unsigned long currentMillis = millis();

      // 1. Lógica de piscar automático
      if (!blinking && currentMillis - lastBlink > nextBlinkInterval) {
        blinking = true;
        targetHeight = 4; // Quase fechado
        lastBlink = currentMillis;
        nextBlinkInterval = random(2000, 5000);
      } else if (blinking && currentHeight <= 6) {
        targetHeight = eyeHeight; // Volta ao normal
        blinking = false;
      }

      // 2. Lógica de olhar para os lados aleatoriamente
      if (currentExpr == EXPR_NORMAL && currentMillis - lastLook > nextLookInterval) {
        targetPupilX = random(-6, 7);
        targetPupilY = random(-4, 5);
        lastLook = currentMillis;
        nextLookInterval = random(1500, 3500);
      } else if (currentExpr == EXPR_SCARED) {
        targetPupilX = 0; // Fica travado no centro quando assustado
        targetPupilY = 0;
      }

      // Suavização de movimento (Lerp) para os olhos e piscadas ficarem fluidos
      currentHeight += (targetHeight - currentHeight) * 0.3;
      pupilOffsetX += (targetPupilX - pupilOffsetX) * 0.2;
      pupilOffsetY += (targetPupilY - pupilOffsetY) * 0.2;

      // Desenho no display Adafruit
      disp->clearDisplay();
      drawEye(leftEyeX, centerY);
      drawEye(rightEyeX, centerY);

      // Detalhes extras de expressão (Sobrancelhas se assustado)
      if (currentExpr == EXPR_SCARED) {
        disp->drawLine(leftEyeX - eyeWidth/2, centerY - eyeHeight/2 - 6, leftEyeX + eyeWidth/2, centerY - eyeHeight/2 - 2, SSD1306_WHITE);
        disp->drawLine(rightEyeX - eyeWidth/2, centerY - eyeHeight/2 - 2, rightEyeX + eyeWidth/2, centerY - eyeHeight/2 - 6, SSD1306_WHITE);
      }

      disp->display();
    }

  private:
    void drawEye(int x, int y) {
      int h = (int)currentHeight;
      if (h < 2) h = 2;
      
      // Desenha o corpo do olho arredondado
      disp->fillRoundRect(x - eyeWidth/2, y - h/2, eyeWidth, h, cornerRadius, SSD1306_WHITE);

      // Desenha a pupila preta se o olho estiver aberto o suficiente
      if (h > 10) {
        int pX = x + (int)pupilOffsetX;
        int pY = y + (int)pupilOffsetY;
        int maxOffset = eyeWidth/2 - 6;
        pX = constrain(pX, x - maxOffset, x + maxOffset);
        pY = constrain(pY, y - h/2 + 4, y + h/2 - 4);
        
        disp->fillCircle(pX, pY, 5, SSD1306_BLACK);
      }
    }
};

#endif