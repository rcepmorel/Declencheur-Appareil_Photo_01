/*DÉCLENCHEUR D'APPAREIL PHOTO ET DE FLASHS
* 
* Auteur : Richard Morel
*     2018-11-09
* 
* Modification
*     2018-11-17
*     - Mesure du voltage d'entrée
*     - Ajout du clignotement de la DEL bleue
*       Relié sur la broche D33
*    2018-11-30
*     - Ajout du déclencheur de relais
*       Relié sur le GPIO 32
*     - Ajout du détecteur du rayon laser pour déclencher le relais
*       sur la coupure du faisceau
*       Relié sur la GPIO 13
*     - Ajout d'un interrupteur pour déclencher le relais
*       Relié sur la GPIO 25
*     - La DEL bleue suit l'état du détecteur laser
*/

const int DELBLEUE = 33; // DEL bleue reliée au GPIO33

#include <driver/adc.h>   
//ADC1_CHANNEL_7 D35 35

float VoltMesure = 0;
float vpeDvValmx_0DB = 1.1/4095; // 12 bits de résolution
float vvpeDvValmx_2_5DB = 1.5/4095;
float vpeDvValmx_6DB = 2.0/4095;
float vpeDvValmx_11DB = 3.9/4095;

// valeur réelle diviser par VoltMesure lorsque calibrationDiviseurTSN égal à 1
float calibrationDiviseurTSN = 7.99/8.70;     

// valeur réelle diviser par VoltMesure lorsque calibrationDiviseurTsn égal à 1
float calibrationDiviseurTsnTrtPolyn = 7.99/8.07; 

#define pinDeclencheur 32        // Déclenche le relais ( Relais contact NO )
#define pinDetecteurLaser 13     // Détecte une MALT à l'entrée D13
#define pinBoutonDeclencheur 25  // Détecte une MALT à l'entrée D25

boolean etatDetecteurLaser; 
boolean etatBtnDeclencheur; 

int delaiDeReaction = 0;
int tempsRelaisActif = 1000; // Durée d'activation du relais en milliseconde
int delaiAvantRetour = 1000; // Délai en milliseconde avant de retourner suivre l'état des détecteurs

// ----------------------------------------------------------------------------- 
// FONCTION  FONCTION   FONCTION    FONCTION    FONCTION    FONCTION    FONCTION
// ----------------------------------------------------------------------------- 

void declencheRelais(){
  digitalWrite(pinDeclencheur, HIGH);  // Déclenche le relais
  delay(tempsRelaisActif); // Attente en milliseconde 
  digitalWrite(pinDeclencheur, LOW);   // Relâche le relais
  delay(delaiAvantRetour); // Attente en milliseconde                    
}

// ----------------------------------------------------------------------------- 
// SETUP   SETUP   SETUP   SETUP   SETUP   SETUP   SETUP   SETUP   SETUP
// ----------------------------------------------------------------------------- 
void setup() {
  Serial.begin(9600);
  
  pinMode(DELBLEUE, OUTPUT);

  pinMode(pinDeclencheur, OUTPUT );
  digitalWrite(pinDeclencheur, LOW);

  pinMode(pinDetecteurLaser, INPUT);
  pinMode(pinBoutonDeclencheur, INPUT_PULLUP);   // active la résistance de pull-up interne sur le +3v

  
  // ***** Configuration pour faire des mesures sur D35 (voltage de la source d'alimentation) ******
  adc1_config_width(ADC_WIDTH_BIT_12); // Définie la résolution (0 à 4095)
  

  adc1_config_channel_atten(ADC1_CHANNEL_7,ADC_ATTEN_DB_11); //Le voltage maximum au GPIO est de 3.3V

  Serial.println("PRÊT");
}

// ----------------------------------------------------------------------------- 
// LOOP     LOOP     LOOP     LOOP     LOOP     LOOP     LOOP     LOOP     LOOP 
// ----------------------------------------------------------------------------- 
void loop() {
  etatDetecteurLaser = digitalRead(pinDetecteurLaser);
  etatBtnDeclencheur = digitalRead(pinBoutonDeclencheur);
  digitalWrite(DELBLEUE, etatDetecteurLaser); 

  if (!etatDetecteurLaser){; // si l'état du détecteur est 0 (faisceau coupé)
    delay(delaiDeReaction); // délai avant d'activer le relais (en milliseconde)
    //delayMicroseconds(delaiDeReaction); // délai avant d'activer le relais (en microseconde)
    declencheRelais();
  }
  
  if (!etatBtnDeclencheur){; // si l'état du bouton est 0
    declencheRelais();
  }

 
  // Méthode de G6EJD  (applicable si l'atténuation est 11dB et la résolution est 12 bits)
  float reading = analogRead(35);
  float VoltAlimCtrlReduitTrtPolyn = -0.000000000000016 * pow(reading,4) + 0.000000000118171 * pow(reading,3)- 0.000000301211691 * pow(reading,2)+ 0.001109019271794 * reading + 0.034143524634089;
  float VoltMesureTrtPolyn = VoltAlimCtrlReduitTrtPolyn*122/22; // Diviseur de tension 100K, 22K
  VoltMesureTrtPolyn = VoltMesureTrtPolyn*calibrationDiviseurTsnTrtPolyn; // correction due aux valeurs imprécises du diviseur de tension
  
 // Serial.print(reading);Serial.print(" DigitalValueVoltAlimCtrl ");Serial.print(VoltAlimCtrlReduitTrtPolyn);Serial.print(" VoltAlimCtrlReduit  ");
 // Serial.print(VoltMesureTrtPolyn);Serial.println(" VoltMesure"); 
 // Serial.println(" "); 

  Serial.print(etatDetecteurLaser);
  Serial.print(" État détecteur Laser,");
  Serial.print(etatBtnDeclencheur);
  Serial.print(" État Interrupteur,");
  Serial.print(VoltMesureTrtPolyn);
  Serial.println(" Voltage d'alimentation,");

  
}

