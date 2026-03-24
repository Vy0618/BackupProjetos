#include <Arduino.h>

///VErsão 3.0.

const int trigPin = 11;
const int echoPin = 10;
const int motor1_Frente = 42;          // Pino para controlar Motor 1 (rotação)
const int motor1_Tras = 40;            // Pino para controlar Motor 1 (avanço)
const int motor2_Frente = 41;          // pino de controle para motor 2
const int motor2_Tras = 43;            // pino de controle para motor 2
const int distancia_deteccao = 100;    // cm
const int distancia_alvo = 7;          // cm (ajustado para 7, igual ao usado)
const unsigned long tempo_giro = 5000; // ms
const int tempo_coleta = 4000;         // ms
const int tempo_entreLoops = 500;      // ms
const int distancia_minima = 2;        // cm
const int distancia_maxima = 400;      // cm
const int timeout_sensor = 30000;      // pulso de 5m
const int timeout_aproximacao = 30000; // ms

float lerDistancia()
{
    digitalWrite(trigPin, LOW);
    delayMicroseconds(4);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(12);
    digitalWrite(trigPin, LOW);

    long duracao = pulseIn(echoPin, HIGH, timeout_sensor);

    if (duracao == 0)
    {
        return -1;
    }

    float distancia = (duracao * 0.034) / 2;

    if (distancia < distancia_minima || distancia > distancia_maxima)
    {
        return -1;
    }

    return distancia;
}

void exibirDistancia(float distancia)
{
    if (distancia > 0)
    {
        Serial.print("Distância atual: ");
        Serial.print(distancia);
        Serial.print(" cm");
    }
    else
    {
        Serial.print("Leitura Inválida");
    }
}

void ligarMotoresAvanco()
{
    digitalWrite(motor1_Frente, HIGH);
    digitalWrite(motor2_Frente, HIGH);
}

void ligarMotoresRotacao()
{
    digitalWrite(motor1_Frente, HIGH);
    digitalWrite(motor2_Frente, LOW);
}

void desligarMotores()
{
    digitalWrite(motor1_Frente, LOW);
    digitalWrite(motor2_Frente, LOW);
    digitalWrite(motor1_Tras, LOW);
    digitalWrite(motor2_Tras, LOW);
}

void avancarRapido()
{
    ligarMotoresAvanco();
    delay(2000);
    desligarMotores();
    delay(500);
}

void aproximarEColetar()
{
    Serial.println("SE APROXIMANDO DO OBJETO\n");

    ligarMotoresAvanco();

    float distancia;
    unsigned long tempoInicio = millis();

    do
    {
        distancia = lerDistancia();

        if (distancia < 0)
        {
            delay(50);
            continue; // tenta novamente
        }

        Serial.print("Aproximando: ");
        Serial.print(distancia);
        Serial.println(" cm");

        if (millis() - tempoInicio > timeout_aproximacao)
        {
            Serial.println("Timeout na aproximação\n");

            desligarMotores();
            return;
        }

        delay(100);

    } while (distancia > distancia_alvo);

    desligarMotores();

    Serial.print("Alvo alcançado a ");
    Serial.print(distancia);
    Serial.println(" cm");

        delay(tempo_coleta);

    Serial.println("OBJETO COLETADO COM SUCESSO!");
}

bool girarProcurar()
{
    Serial.println("Girando 360° procurando objetos...\n");

    avancarRapido();

    ligarMotoresRotacao();

    unsigned long tempoInicio = millis();

    while (millis() - tempoInicio < tempo_giro)
    {
        float distancia = lerDistancia();

        if (distancia < 0)
        {
            delay(50);
            continue;
        }

        if (distancia < distancia_deteccao)
        {
            Serial.print("Objeto detectado a ");
            Serial.print(distancia);
            Serial.println("cm durante a rotação!\n");

            desligarMotores();
            return true;
        }
        delay(100);
    }

    desligarMotores();
    Serial.println("NENHUM OBJETO ENCONTRADO\n");

    return false;
}

void setup()
{
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    pinMode(motor1_Frente, OUTPUT);
    pinMode(motor1_Tras, OUTPUT);
    pinMode(motor2_Frente, OUTPUT);
    pinMode(motor2_Tras, OUTPUT);
}

void exibirInicializacao()
{
    Serial.println("=================================");
    Serial.println("   ECOFLUTUADOR - SISTEMA PET   ");
    Serial.println("=================================\n");
    Serial.println("Sistema inicializado com sucesso!");
}

void protocoloPrincipal()
{

    float distancia = lerDistancia();
    exibirDistancia(distancia);

    if (distancia < 0)
    {
        delay(500);
        return;
    }

    if (distancia < distancia_deteccao)
    {
        Serial.prinln("OBJETO DETECTADO!");
        aproximarEColetar();
        Serial.println("Ciclo de coleta concluído.");
        delay(2000);
    } else{
        Serial.println("Nenhum objeto encontrado. Iniciando varredura.");

        bool objetoEncontrado = girarProcurar();

        if(objetoEncontrado){
            Serial.println("Objeto encontrado durante varredura");
            delay(500);
        } else{
            Serial.println("Nenhum objeto encontrado. Nova varredura em 7 segundos.");
            delay(7000);
        }
    }
}

void loop(){
    protocoloPrincipal();
    delay(tempo_entreLoops);
}
