#include <Arduino.h>
#include <Servo.h>


/*
Funcionamento do motor:

0° - motor parado
90° - motor a 50%
180° - motor a 100%

*/



const int trigPin = 11;
const int echoPin = 10;
const int ESC1_PIN = 9;   // Motor esquerdo (rotação)
const int ESC2_PIN = 10;  // Motor direito (avanço)

const int distancia_deteccao = 100;
const int distancia_alvo = 20;
const unsigned long tempo_giro = 5000;
const int tempo_coleta = 4000;
const int tempo_entreLoops = 500;
const int timeout_aproximacao = 30000;

// Velocidades (0-100%)
const int VEL_AVANCO = 70;
const int VEL_GIRO = 50;

Servo escEsquerdo;
Servo escDireito;


float lerDistancia() {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(4);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(12);
    digitalWrite(trigPin, LOW);

    long duracao = pulseIn(echoPin, HIGH, 30000);
    if (duracao == 0) return -1;

    float distancia = (duracao * 0.034) / 2;
    if (distancia < 2 || distancia > 400) return -1;

    return distancia;
}

void exibirDistancia(float distancia) {
    if (distancia > 0) {
        Serial.print("Distância: ");
        Serial.print(distancia);
        Serial.println(" cm");
    } else {
        Serial.println("Leitura inválida");
    }
}

/* Controla a intensidade do motor. Ele realiza calculos que convertem um parâmetro no código para valores que o motor brushless é capaz de entender. */

int pctParaAngulo(int pct) { 
    pct = constrain(pct, 0, 100);
    return map(pct, 0, 100, 0, 180);
}

void avancar() {
    int angulo = pctParaAngulo(VEL_AVANCO);
    escEsquerdo.write(angulo);
    escDireito.write(angulo);
}

void girar() {
    escEsquerdo.write(pctParaAngulo(VEL_GIRO));
    escDireito.write(0);
}

void parar() {
    escEsquerdo.write(0);
    escDireito.write(0);
}


void aproximarEColetar() {
    Serial.println("Aproximando...");
    avancar(VEL_AVANCO);
    
    float distancia;
    unsigned long inicio = millis();
    
    do {
        distancia = lerDistancia();
        if (distancia < 0) {
            delay(50);
            continue;
        }
        
        Serial.print("Dist: ");
        Serial.println(distancia);
        
        if (millis() - inicio > timeout_aproximacao) {
            Serial.println("Timeout!");
            parar();
            return;
        }
        delay(100);
    } while (distancia > distancia_alvo);
    
    parar();
    delay(tempo_coleta);
    Serial.println("Coletado!");
}

bool girarProcurar() {
    Serial.println("Girando...");
    
    // Avança um pouco antes de girar
    avancar(VEL_AVANCO);
    delay(2000);
    parar();
    delay(500);
    
    girar(VEL_GIRO);

    unsigned long inicio = millis();
    
    while (millis() - inicio < tempo_giro) {
        float distancia = lerDistancia();
        if (distancia < 0) {
            delay(50);
            continue;
        }
        
        if (distancia < distancia_deteccao) {
            Serial.print("Objeto a ");
            Serial.print(distancia);
            Serial.println(" cm!");
            parar();
            return true;
        }
        delay(100);
    }
    
    parar();
    Serial.println("Nada encontrado");
    return false;
}


void inicializarESCs() {
    escEsquerdo.attach(ESC1_PIN, 1000, 2000);
    escDireito.attach(ESC2_PIN, 1000, 2000);
    parar();
    delay(2000);
    Serial.println("ESCs prontos!");
}

void setup() {
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    
    Serial.begin(9600);
    inicializarESCs();
    
    Serial.println("EcoFlutuador v4 - Brushless");
    Serial.print("Detecção: ");
    Serial.print(distancia_deteccao);
    Serial.println(" cm");
    Serial.println("----------------------------");
}


void loop() {
    float dist = lerDistancia();
    exibirDistancia(dist);
    
    if (dist < 0) {
        delay(500);
        return;
    }
    
    if (dist < distancia_deteccao) {
        Serial.println("OBJETO DETECTADO!");
        aproximarEColetar();
        delay(2000);
    } else {
        if (!girarProcurar()) {
            delay(7000);
        }
    }
    
    delay(tempo_entreLoops);
}
